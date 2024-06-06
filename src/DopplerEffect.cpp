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

/* constructor/destructor */

DopplerEffect::DopplerEffect( float sampleRate, int bufferSize )
{
    _sampleRate = sampleRate;

    lfo = new LFO( sampleRate );
    // lfo->setRate( 0.01f );

    recordBufferSize = 40 * bufferSize; // TODO calculate from param in seconds
    recordBuffer.setSize( 1, recordBufferSize );

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

void DopplerEffect::apply( juce::AudioBuffer<float>& buffer, int channel )
{
    int newWritePointer = recordInput( buffer, channel );

    auto* channelData = buffer.getWritePointer( channel );
    int bufferSize    = buffer.getNumSamples();

    for ( int i = 0; i < bufferSize; ++i ) {

        /*
        // --- TODO make it work without warble
        lfoPhase += lfoFrequency / _sampleRate;
        if (lfoPhase >= 1.0f)
            lfoPhase -= 1.0f;

        // Calculate LFO value
        float lfoValue = std::sin( TWO_PI * lfoPhase );

        // Map LFO value to a distance range (e.g., 1.0 to 10.0 meters)
        float distance = juce::jmap(lfoValue, -1.0f, 1.0f, 1.0f, 10.0f);

        float listenerVelocity = distance * lfoFrequency * TWO_PI; // Circular motion velocity
        // --- E.O. TODO make work with LFO code below
        */

        // move the LFO and convert its position a "distance in meters"
        
        float distance = juce::jmap( lfo->peek(), -1.f, 1.0f, 1.0f, 10.0f );

        // apply circular motion to the listener to approximate their movement

        float listenerVelocity = distance * lfo->getRate() * TWO_PI;

        float dopplerFactor = juce::jlimit( 0.5f, 2.f, ( SPEED_OF_SOUND - listenerVelocity ) / SPEED_OF_SOUND );

        // calculate the index of the sample from the record buffer

        float resampledIndex = ( writePosition + i - bufferSize ) / dopplerFactor;
        if ( resampledIndex < 0 ) {
            resampledIndex += recordBufferSize;
        }
        int index  = static_cast<int>( resampledIndex ) % recordBufferSize;
        float frac = juce::jlimit( 0.f, 1.f, resampledIndex - index );

        // interpolate sample to write
        
        int nextIndex = ( index + 1 ) % recordBufferSize;
        float sampleValue = recordBuffer.getSample( 0, index ) * ( 1.0f - frac ) +
                            recordBuffer.getSample( 0, nextIndex ) * frac;

        if ( std::isnan( sampleValue )) sampleValue = 0.f; // QQQ do we need this nonsense ?

        juce::jlimit( -1.f, 1.f, sampleValue );
                            
        // int prevIndex2 = ( index - 2 + recordBufferSize ) % recordBufferSize;
        // int prevIndex1 = ( index - 1 + recordBufferSize ) % recordBufferSize;
        
        // // Perform cubic interpolation
        // float sampleValue = cubicInterpolate(
        //     recordBuffer.getSample( 0, prevIndex2 ),
        //     recordBuffer.getSample( 0, prevIndex1 ),
        //     recordBuffer.getSample( 0, index ),
        //     recordBuffer.getSample( 0, nextIndex ),
        //     frac
        // );
        
        channelData[ i ] = sampleValue;
    }

    writePosition = newWritePointer;
}

/* private methods */

int DopplerEffect::recordInput( juce::AudioBuffer<float>& buffer, int channel )
{
    auto* channelData = buffer.getReadPointer( channel );
    int bufferSize    = buffer.getNumSamples();

    for ( int i = 0; i < bufferSize; ++i ) {
        recordBuffer.setSample( 0, writePosition + i, channelData[ i ]);
    }
    return ( writePosition + bufferSize ) % recordBufferSize;
}