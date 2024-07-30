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

DopplerEffect::DopplerEffect( double sampleRate, int bufferSize ) : rateInterpolator( 1.0f, 0.1f ), speedInterpolator( 1.f, 0.01f ), lfo( sampleRate )
{
    lfo.setDepth(( 1.f / MAX_OBSERVER_DISTANCE ) * 0.025f );

    _sampleRate = static_cast<float>( sampleRate );
    _bufferSize = bufferSize;

    setRecordingLength( MAX_LFO_CYCLE_DURATION ); // recording should last for a single cycle at the lowest rate
    maxRecordBufferSize = recordBufferSize; // recordBufferSize has been calculated by setRecordingLength()

    // Calculate the number of samples needed based on max doppler rate adjustments
    // in order to perform an upwards Doppler shift we need to read forward in time, meaning that
    // the buffer needs to be prefilled before we can start reading. To be completely safe, the amount
    // should be equal to: static_cast<int>( sampleRate * MAX_LFO_CYCLE_DURATION * MAX_DOPPLER_RATE );
    // though this requires a large pre-record buffer. We can optimise this at the risk of having
    // an occasional glitch
    
    minRequiredSamples = static_cast<int>( sampleRate * MIN_DOPPLER_RATE );
    
    recordBuffer.setSize( 1, maxRecordBufferSize );
    recordBuffer.clear(); // fills buffer with silence

    readPosition  = 0;
    writePosition = 0;
}

DopplerEffect::~DopplerEffect()
{
    // nowt...
}

/* public methods */

void DopplerEffect::setProperties( float speed, bool invert )
{
    float scaledSpeed = juce::jmap( speed, 0.f, 1.f, Parameters::Config::LFO_MIN_RATE, Parameters::Config::LFO_MAX_RATE );
    lfo.setRate( scaledSpeed );

    invertDirection = invert;
}

void DopplerEffect::setRecordingLength( float normalizedRange )
{
    int durationInSamples = Calc::secondsToBuffer(
        juce::jmap( normalizedRange, 0.f, 1.f, Parameters::Config::REC_DURATION_MIN, Parameters::Config::REC_DURATION_MAX ),
        _sampleRate
    );

    if ( durationInSamples < 0 ) {
        return;
    }

    if ( maxRecordBufferSize > 0 && durationInSamples > maxRecordBufferSize ) {
        durationInSamples = maxRecordBufferSize;
    }

    recordBufferSize  = durationInSamples + durationInSamples % _bufferSize; // make multiple of block bufferSize
    fRecordBufferSize = static_cast<float>( recordBufferSize );

    // TODO check this logic if we are going to make this parameter automatable, this crackles like crazy
    // when lowering the duration from the max 1.f value (readPosition probably needs attention too...)

    if ( writePosition > recordBufferSize ) {
        writePosition = 0;
    }
}

void DopplerEffect::resetOscillators()
{
    lfo.setPhase( 0.f );

    readFromRecordBuffer = false;
    totalRecordedSamples = 0;

    readPosition = writePosition; // will first buffer for another minRequiredSamples before reading starts
}

void DopplerEffect::apply( juce::AudioBuffer<float>& buffer, int channel )
{
    recordInput( buffer, channel );

    int bufferSize = buffer.getNumSamples();

    if ( !readFromRecordBuffer ) {
        totalRecordedSamples += bufferSize;

        if ( totalRecordedSamples >= minRequiredSamples ) {
            readFromRecordBuffer = true;
        } else if ( !invertDirection ) {
            return; // need to fill up the record buffer before doing upward shifts
        }
    }

    float lfoRate = lfo.getRate();

    if ( lfoRate == 0.f ) {
        updateReadPosition( bufferSize );
        return; // nothing else to do
    }

    auto* channelData = buffer.getWritePointer( channel );

    float distanceMultiplier = lfoRate * TWO_PI;
    float resampledIndex;

    for ( int i = 0; i < bufferSize; ++i ) {

        // move the LFO and convert its position to a "distance in meters"

        float observerDistance = juce::jmap( lfo.peek(), -1.0f, 1.0f, MIN_OBSERVER_DISTANCE, MAX_OBSERVER_DISTANCE );
        
        // apply circular motion to the listener to approximate their movement

        float observerSpeed = observerDistance * distanceMultiplier;

        if ( interpolateRate ) {
            observerSpeed = speedInterpolator.setValue( observerSpeed );
        }
        float dopplerRate = juce::jlimit( MIN_DOPPLER_RATE, MAX_DOPPLER_RATE, ( SPEED_OF_SOUND - observerSpeed ) / SPEED_OF_SOUND );

        if ( interpolateRate ) {
            dopplerRate = rateInterpolator.setValue( dopplerRate );
        }

        // calculate the read index of the sample inside the record buffer

        if ( invertDirection ) {
            resampledIndex = ( readPosition + i ) * dopplerRate;
        } else {
            resampledIndex = ( readPosition + i ) / dopplerRate;
        }

        // ensure the resampleIndex remains within record bounds (prevents frac from exceeding max -1.f to +1.f sample values)

        resampledIndex = fmod( resampledIndex, fRecordBufferSize );
        if ( resampledIndex < 0 ) {
            resampledIndex += recordBufferSize;
        }

        int index  = static_cast<int>( resampledIndex );// % recordBufferSize;
        float frac = resampledIndex - static_cast<float>( index );

        // calculate sample value using (more accurate) cubic interpolation

        // float sampleValue = cubicInterpolator.getInterpolatedSample( recordBuffer, recordBufferSize, index, frac );

        // calculate sample value using (faster) linear interpolation
        
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
    // note we omit the modulo operator here (when going from the end of recordBuffer back to the beginning,
    // the resampleIndex-counter would be calculated to start from beginning as well, causing glitches in playback)
    readPosition = ( readPosition + bufferSize );// % recordBufferSize;
}