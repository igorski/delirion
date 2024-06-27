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
#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

class LFO
{
    public:
        // LFO range in Hz
        static constexpr float MAX_LFO_RATE = 10.f;
        static constexpr float MIN_LFO_RATE = 0.1f;

        LFO( double sampleRate );
        ~LFO();

        float getRate();
        void setRate( float value );

        float getDepth();
        void setDepth( float value );

        float getPhase();
        void setPhase( float value );

        /**
         * retrieve a value from the wave table for the current
         * phase position, this method also increments
         * the phase and keeps it within bounds
         */
        inline float peek()
        {
            float lfoValue = std::sin( TWO_PI * _phase ) * _depth;
            
            _phase += _phaseIncrement;

            if ( _phase >= 1.0f ) {
                _phase -= 1.0f;
            }
            return lfoValue;
        }

    private:
        const float TWO_PI  = 2.f * juce::MathConstants<float>::pi;

        float _sampleRate;
        float _rate;
        float _depth;
        float _phase;
        float _phaseIncrement;
};
