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
    if ( lfo->getRate() == 0.f ) {
        return; // nothing to do
    }
    int newWritePointer = recordInput( buffer, channel );

    auto* channelData = buffer.getWritePointer( channel );
    int bufferSize    = buffer.getNumSamples();

    for ( int i = 0; i < bufferSize; ++i ) {

        // move the LFO and convert its position to a "distance in meters"
        
        float observerDistance = juce::jmap( lfo->peek(), -1.f, 1.0f, 1.0f, 10.0f );

        // apply circular motion to the listener to approximate their movement

        float observerSpeed = observerDistance * lfo->getRate() * TWO_PI;

        float dopplerRate = juce::jlimit( 0.5f, 2.f, ( SPEED_OF_SOUND - observerSpeed ) / SPEED_OF_SOUND );

        // calculate the index of the sample from the record buffer

        float resampledIndex = ( writePosition + i - bufferSize ) / dopplerRate;
        if ( resampledIndex < 0 ) {
            resampledIndex += recordBufferSize;
        }
        int index  = static_cast<int>( resampledIndex ) % recordBufferSize;
        float frac = juce::jlimit( 0.f, 1.f, resampledIndex - index );

        // interpolate the sample to write
        
        int nextIndex = ( index + 1 ) % recordBufferSize;
        float sampleValue = recordBuffer.getSample( 0, index ) * ( 1.0f - frac ) +
                            recordBuffer.getSample( 0, nextIndex ) * frac;

        // @todo the below can be used to sanitize values in case we are overflowing somewhere

        if ( std::isnan( sampleValue )) sampleValue = 0.f;
        juce::jlimit( -1.f, 1.f, sampleValue );

        // @todo interpolation to prevent glitches
                            
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