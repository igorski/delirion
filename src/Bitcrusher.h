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

class BitCrusher {

    public:
        BitCrusher( float amount, float inputMix, float outputMix );
        ~BitCrusher();

        void apply( juce::AudioBuffer<float>& buffer, int channel );

        void setAmount( float value ); // range between -1 to +1
        void setInputMix( float value );
        void setOutputMix( float value );

        inline bool isActive() {
            return _bits < 16;
        }

    private:
        int _bits; // we scale the amount to integers in the 1-16 range
        float _amount;
        float _inputMix;
        float _outputMix;

        void calcBits();
};
