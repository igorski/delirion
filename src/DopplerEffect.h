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

#include <cmath>
#include <juce_audio_processors/juce_audio_processors.h>
#include "LFO.h"

class DopplerEffect
{
    const float TWO_PI = 2.f * juce::MathConstants<float>::pi;
    const float SPEED_OF_SOUND = 343.0f; // in m/s

    public:
        DopplerEffect( float sampleRate, int bufferSize );
        ~DopplerEffect();

        void setSpeed( float value );

        // applies the Doppler effect onto the provided buffer at provided channel
        // (Doppler effect applies onto individual (mono) channels, not groups)

        void apply( juce::AudioBuffer<float>& buffer, int channel );

    private:
        LFO* lfo;
        float _sampleRate;

        int recordInput( juce::AudioBuffer<float>& buffer, int channel );

        juce::AudioBuffer<float> recordBuffer;
        int recordBufferSize;
        int writePosition;

        inline float cubicInterpolate( float y0, float y1, float y2, float y3, float frac )
        {
            float a = ( -0.5f * y0 ) + ( 1.5f * y1 ) - ( 1.5f * y2 ) + ( 0.5f * y3 );
            float b = y0 - ( 2.5f * y1 ) + ( 2.0f * y2 ) - ( 0.5f * y3 );
            float c = ( -0.5f * y0 ) + ( 0.5f * y2 );
            float d = y1;

            return a * frac * frac * frac + b * frac * frac + c * frac + d;
        }
};
