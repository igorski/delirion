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
#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

class CubicInterpolator
{
    public:
        CubicInterpolator() {}

        inline float interpolate( float y0, float y1, float y2, float y3, float x )
        {
            return y1 + 0.5f * x * ( y2 - y0 + x * ( 2.0f * y0 - 5.0f * y1 + 4.0f * y2 - y3 + x * ( 3.0f * ( y1 - y2 ) + y3 - y0 )));
        }

        inline float getInterpolatedSample( juce::AudioBuffer<float>& buffer, int bufferSize, int index, float frac )
        {
            int prevIndex1 = ( index - 1 + bufferSize ) % bufferSize;
            int prevIndex2 = ( index - 2 + bufferSize ) % bufferSize;
            int nextIndex1 = ( index + 1 ) % bufferSize;
            int nextIndex2 = ( index + 2 ) % bufferSize;

            float y0 = ( prevIndex2 < 0 || prevIndex2 >= bufferSize ) ? lastNextNextSample : buffer.getSample( 0, prevIndex2 );
            float y1 = ( prevIndex1 < 0 || prevIndex1 >= bufferSize ) ? lastNextSample : buffer.getSample( 0, prevIndex1 );
            float y2 = ( index < 0 || index >= bufferSize ) ? lastSample : buffer.getSample( 0, index );
            float y3 = ( nextIndex1 < 0 || nextIndex1 >= bufferSize ) ? 0.0f : buffer.getSample( 0, nextIndex1 );

            lastSample         = buffer.getSample( 0, index );
            lastNextSample     = buffer.getSample( 0, nextIndex1 );
            lastNextNextSample = buffer.getSample( 0, nextIndex2 );

            return interpolate( y0, y1, y2, y3, frac );
        }

    private:
        float lastSample         = 0.f;
        float lastNextSample     = 0.f;
        float lastNextNextSample = 0.f;
};
