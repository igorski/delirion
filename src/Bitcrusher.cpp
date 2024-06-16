/*
 * Copyright (c) 2013-2024 Igor Zinken https://www.igorski.nl
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
#include "Bitcrusher.h"
#include <math.h>

/* constructor */

BitCrusher::BitCrusher( float amount, float inputMix, float outputMix )
{
    setAmount   ( amount );
    setInputMix ( inputMix );
    setOutputMix( outputMix );
}

BitCrusher::~BitCrusher()
{
    // nowt
}

/* public methods */

void BitCrusher::apply( juce::AudioBuffer<float>& buffer, int channel )
{
    // sound should not be crushed ? do nothing
    if ( !isActive() ) {
        return;
    }

    auto* channelData = buffer.getWritePointer( channel );
    int bufferSize    = buffer.getNumSamples();
    
    int bitsPlusOne = _bits + 1;

    for ( int i = 0; i < bufferSize; ++i )
    {
        short input = ( short ) (( channelData[ i ] * _inputMix ) * SHRT_MAX );
        short prevent_offset = ( short )( -1 >> bitsPlusOne );
        input &= ( -1 << ( 16 - _bits ));
        channelData[ i ] = (( input + prevent_offset ) * _outputMix ) / SHRT_MAX;
    }
}

/* setters */

void BitCrusher::setAmount( float value )
{
    // note we invert the value as a higher value implies less bit rate reduction
    _amount = abs( value - 1.f );

    calcBits();
}

void BitCrusher::setInputMix( float value )
{
    _inputMix = juce::jlimit( 0.f, 1.f, value );
}

void BitCrusher::setOutputMix( float value )
{
    _outputMix = juce::jlimit( 0.f, 1.f, value );
}

/* private methods */

void BitCrusher::calcBits()
{
    // scale float to 1 - 16 bit range
    _bits = ( int ) juce::jmap( _amount, 0.f, 1.f, 1.f, 16.f );
}
