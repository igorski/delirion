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

DopplerEffect::DopplerEffect( double sampleRate, int bufferSize ) : rateInterpolator( 1.0f, INTERPOLATION_SPEED ), speedInterpolator( 1.f, INTERPOLATION_SPEED ), lfo( sampleRate )
{
    lfo.setDepth( LFO_DEPTH );

    _sampleRate = static_cast<float>( sampleRate );
    _bufferSize = bufferSize;

    crossfadeSize = static_cast<float>( Calc::secondsToBuffer( CROSSFADE_DURATION, _sampleRate ));
    crossfadeSamplesLeft = 0;
    crossfadedSamples = 0;

    float maxDelay = ( MAX_OBSERVER_DISTANCE / SPEED_OF_SOUND ) / MIN_DOPPLER_RATE;

// @todo can we do something with MAX_LFO_CYCLE_DURATION once more ?
    setRecordingLength( maxDelay * 20 );
    // setRecordingLength( MAX_LFO_CYCLE_DURATION ); // recording should last for a single cycle at the lowest rate
    
    maxRecordBufferSize = recordBufferSize; // recordBufferSize has been calculated by setRecordingLength()
    recordBuffer.setSize( 1, maxRecordBufferSize );
    recordBuffer.clear(); // fills buffer with silence

    readPosition = 0;
    writePosition = 0;
    totalRecordedSamples = 0;
    processedSamples = 0;
}

DopplerEffect::~DopplerEffect()
{
    // nowt...
}

/* public methods */

void DopplerEffect::setProperties( float speed, bool invert, bool sync )
{
    float scaledSpeed = juce::jmap( speed, 0.f, 1.f, Parameters::Config::LFO_MIN_RATE, Parameters::Config::LFO_MAX_RATE );
    lfo.setRate( scaledSpeed );

    invertDirection = invert;
    syncToBeat = sync;
}

void DopplerEffect::setRecordingLength( float durationInSeconds )
{
    int durationInSamples = Calc::secondsToBuffer( durationInSeconds, _sampleRate );
/*
    int durationInSamples = Calc::secondsToBuffer(
        juce::jmap( durationInSeconds, 0.f, 1.f, Parameters::Config::REC_DURATION_MIN, Parameters::Config::REC_DURATION_MAX ),
        _sampleRate
    );*/

    if ( durationInSamples < 0 ) {
        return;
    }

    if ( maxRecordBufferSize > 0 && durationInSamples > maxRecordBufferSize ) {
        durationInSamples = maxRecordBufferSize;
    }

    // make multiple of block bufferSize
    float fBufferSize = static_cast<float>( _bufferSize );
    recordBufferSize  = static_cast<int>( ceil( static_cast<float>( durationInSamples ) / fBufferSize ) * fBufferSize );
    fRecordBufferSize = static_cast<float>( recordBufferSize );

    if ( writePosition > recordBufferSize ) {
        writePosition = 0;
    }
}

void DopplerEffect::updateTempo( double tempo, int timeSigNominator, int timeSigDenominator )
{
    juce::ignoreUnused( timeSigNominator );

    float fullMeasureDuration = ( 60.f / static_cast<float>( tempo )) * timeSigDenominator; // seconds per measure
    float fullMeasureSamples  = static_cast<float>( Calc::secondsToBuffer( fullMeasureDuration, _sampleRate ));
    samplesPerBeat = static_cast<int>( ceil( fullMeasureSamples / timeSigDenominator ));
    
    // Calculate the number of samples needed to perform an upwards Doppler shift, as this requires
    // reading "forward in time", e.g.: the buffer needs to be prefilled before we can start reading.
    
    minRequiredSamplesInvert = static_cast<int>( _sampleRate / 32.f ); // subset
    minRequiredSamples       = static_cast<int>( _sampleRate * MAX_LFO_CYCLE_DURATION * MAX_DOPPLER_RATE ); // full size buffer
   
    // convert the value to be a multiple of a single beat
    minRequiredSamples += ( minRequiredSamples % samplesPerBeat ); // ensure its larger than a single beat so it exceeds the above min
    minRequiredSamples = std::min( recordBufferSize, minRequiredSamples ); // keep within buffer bounds

    resetRecordBuffer();
}

void DopplerEffect::onSequencerStart()
{
    lfo.setPhase( 0.f );
    resetRecordBuffer();
}

void DopplerEffect::apply( juce::AudioBuffer<float>& buffer, int channel )
{
    recordInput( buffer, channel );
    
    int bufferSize = buffer.getNumSamples();
    
    if ( !readFromRecordBuffer ) {
        // we first need n amount of samples recorded before we can start applying the effect
        totalRecordedSamples += bufferSize;

        if ( invertDirection && totalRecordedSamples < minRequiredSamplesInvert ) {
            return;
        }

        if ( totalRecordedSamples >= minRequiredSamples ) {
            readFromRecordBuffer = true;
        } else if ( !invertDirection ) {
            return; // need to fill up the record buffer even more before doing upward shifts
        }
    }

    float lfoRate = lfo.getRate();

    if ( lfoRate == 0.f ) {
        return onPostApply( bufferSize ); // nothing else to do
    }

    auto* channelData = buffer.getWritePointer( channel );

    float distanceMultiplier = lfoRate * TWO_PI;

    for ( int i = 0; i < bufferSize; ++i ) {

        bool doCrossfade = crossfadeSamplesLeft > 0;
        
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
       
        float sampleValue = getResampledValue( dopplerRate, readPosition, i );

        if ( doCrossfade ) {
            float nextValue = getResampledValue( dopplerRate, getSyncedReadPosition(), i );
            float mixFactor = crossfadedSamples / crossfadeSize;

            sampleValue = ( 1.0f - mixFactor ) * sampleValue + mixFactor * nextValue;

            ++crossfadedSamples;

            if ( --crossfadeSamplesLeft == 0 ) {
                readPosition = getSyncedReadPosition(); // crossfade complete, commit readPosition
            }
        }
        
        // Apply a high-pass filter to remove DC offset
         
        float filteredValue   = sampleValue - previousSampleValue + DC_OFFSET_FILTER * previousFilteredValue;
        previousSampleValue   = sampleValue;
        previousFilteredValue = filteredValue;

        channelData[ i ] = filteredValue;

        // beat detection

        if ( ++processedSamples >= samplesPerBeat ) {
            processedSamples = 0;

            if ( syncToBeat ) {
                crossfadeSamplesLeft = static_cast<int>( crossfadeSize );
                crossfadedSamples = 0;
            }
        }
    }
    onPostApply( bufferSize );
}

/* private methods */

void DopplerEffect::recordInput( juce::AudioBuffer<float>& buffer, int channel )
{
    auto* channelData = buffer.getReadPointer( channel );
    int bufferSize    = buffer.getNumSamples();
    int writeEnd      = writePosition + bufferSize; // corresponds to the write index at start of the next iteration

    bool shouldWrap = writeEnd >= recordBufferSize;

    if ( shouldWrap ) {
        // in certain situations (odd buffer size or in case host changes buffer size between process block
        // calls) it is possible the end of the current recording iteration will exceed the record buffer size
        // we can use a modulo operator to stay within bounds, but manually managing the loops overcomes
        // potentially expensive divisor operations on the CPU

        int samplesUntilWrap = recordBufferSize - writePosition;
        
        for ( int i = 0; i < samplesUntilWrap; ++i, ++writePosition ) {
            recordBuffer.setSample( 0, writePosition, channelData[ i ]);
        }

        writePosition = 0;
        for ( int i = samplesUntilWrap; i < bufferSize; ++i, ++writePosition ) {
            recordBuffer.setSample( 0, writePosition, channelData[ i ]);
        }
    } else {
        for ( int i = 0; i < bufferSize; ++i, ++writePosition ) {
            recordBuffer.setSample( 0, writePosition, channelData[ i ]);
        }
    }
}

void DopplerEffect::resetRecordBuffer()
{
    recordBuffer.clear();

    readFromRecordBuffer = false;
    totalRecordedSamples = 0;
    processedSamples     = 0;

    writePosition = 0;
    readPosition  = writePosition;
}

void DopplerEffect::onPostApply( int readBuffers )
{
    processedSamples += readBuffers;
    readPosition += readBuffers;
}
