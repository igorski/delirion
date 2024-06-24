/*
 * Copyright (c) 2024 Igor Zinken https://www.igorski.nl
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "DopplerEffect.h"
#include "../../utils/Calc.h"

/* constructor/destructor */

DopplerEffect::DopplerEffect( float sampleRate, int bufferSize )
{
    lfo = new LFO( sampleRate );
    lfo->setDepth(( 1.f / MAX_OBSERVER_DISTANCE ) * 0.025f );

// @todo make scaleable 2 * bufferSize (of 256) sounds nice, calc this and grab it as minimum
    int idealRecordSize = 40 * bufferSize;//Calc::secondsToBuffer(( LFO::MAX_LFO_RATE * 4 ) / 1000, sampleRate );
    recordBufferSize    = idealRecordSize + idealRecordSize % bufferSize; // make multiple of bufferSize
    fRecordBufferSize   = static_cast<float>( recordBufferSize );

    minRequiredSamples = bufferSize * static_cast<int>( ceil( MAX_DOPPLER_RATE ));

    recordBuffer.setSize( 1, recordBufferSize );
    recordBuffer.clear(); // fills buffer with silence

    readPosition  = 0;
    writePosition = 0;
}

DopplerEffect::~DopplerEffect()
{
    delete lfo;

    recordBuffer.clear();
}

/* public methods */

void DopplerEffect::setSpeed( float value )
{
    lfo->setRate( value );
}

void DopplerEffect::resetOscillators()
{
    lfo->setPhase( 0.f );

    readFromRecordBuffer = false;
    totalRecordedSamples = 0;

    if ( writePosition >= minRequiredSamples ) {
        readPosition = writePosition - minRequiredSamples;
    }
}

void DopplerEffect::apply( juce::AudioBuffer<float>& buffer, int channel )
{
    recordInput( buffer, channel );

    int bufferSize = buffer.getNumSamples();

    // @todo make this smart (at lower oscillator speeds we can work in realtime, maybe?)
    
    if ( !readFromRecordBuffer ) {
        totalRecordedSamples += bufferSize;

        if ( totalRecordedSamples >= minRequiredSamples ) {
            readFromRecordBuffer = true;
        } else {
            return; // need to fill up the record buffer
        }
    }

    if ( lfo->getRate() == 0.f ) {
        updateReadPosition( bufferSize );
        return; // nothing else to do
    }

    auto* channelData = buffer.getWritePointer( channel );

    float distanceMultiplier = lfo->getRate() * TWO_PI;

    for ( int i = 0; i < bufferSize; ++i ) {

        // move the LFO and convert its position to a "distance in meters"

        float observerDistance = juce::jmap( lfo->peek(), -1.0f, 1.0f, MIN_OBSERVER_DISTANCE, MAX_OBSERVER_DISTANCE );
        
        // apply circular motion to the listener to approximate their movement

        float observerSpeed = observerDistance * distanceMultiplier;

        float dopplerRate = juce::jlimit( MIN_DOPPLER_RATE, MAX_DOPPLER_RATE, ( SPEED_OF_SOUND - observerSpeed ) / SPEED_OF_SOUND );

        // calculate the index of the sample from the record buffer

        float resampledIndex = ( readPosition + i ) / dopplerRate;
        if ( resampledIndex < 0 ) {
            resampledIndex += recordBufferSize;
        }
        // ensure the resampleIndex remains within record bounds (prevents frac from exceeding max -1.f to +1.f sample values)
        resampledIndex = fmod( resampledIndex, fRecordBufferSize );
    
        int index  = static_cast<int>( resampledIndex ) % recordBufferSize;
        float frac = resampledIndex - static_cast<float>( index );

        // calculate sample value using cubic interpolation

        // float sampleValue = interpolator.getInterpolatedSample( recordBuffer, recordBufferSize, index, frac );

        // calculate sample value using linear interpolation
        int nextIndex = ( index + 1 ) % recordBufferSize;
        float sampleValue = recordBuffer.getSample( 0, index ) * ( 1.0f - frac ) +
                            recordBuffer.getSample( 0, nextIndex ) * frac;

        // Apply a high-pass filter to remove DC offset
         
        float filteredValue   = sampleValue - previousSampleValue + DC_OFFSET_FILTER * previousFilteredValue;
        previousSampleValue   = sampleValue;
        previousFilteredValue = filteredValue;
        
        channelData[ i ] = filteredValue;
    }
    updateReadPosition( bufferSize );
}

/* private methods */

void DopplerEffect::recordInput( juce::AudioBuffer<float>& buffer, int channel )
{
    auto* channelData = buffer.getReadPointer( channel );
    int bufferSize    = buffer.getNumSamples();

    for ( int i = 0; i < bufferSize; ++i ) {
        recordBuffer.setSample( 0, writePosition + i, channelData[ i ]);
    }
    writePosition = ( writePosition + bufferSize ) % recordBufferSize;
}

void DopplerEffect::updateReadPosition( int bufferSize )
{
    // note we omit the modulo operator here (when going from end of recordBuffer back to the beginning
    // the resampleIndex counter would be calculated to start from beginning to, causing glitches in playback)
    readPosition = ( readPosition + bufferSize );// % recordBufferSize;
}