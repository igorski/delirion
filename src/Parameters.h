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

namespace Parameters {
    static juce::String LOW_LFO_ODD      = "lowLfoOdd";
    static juce::String LOW_LFO_EVEN     = "lowLfoEven";
    static juce::String LOW_LFO_LINK     = "lowLfoLink";
    static juce::String MID_LFO_ODD      = "midLfoOdd";
    static juce::String MID_LFO_EVEN     = "midLfoEven";
    static juce::String MID_LFO_LINK     = "midLfoLink";
    static juce::String HI_LFO_ODD       = "hiLfoOdd";
    static juce::String HI_LFO_EVEN      = "hiLfoEven";
    static juce::String HI_LFO_LINK      = "hiLfoLink";
    static juce::String DISTORTION_MIX   = "distMix";
    static juce::String LOW_BAND         = "lowBand";
    static juce::String MID_BAND         = "midBand";
    static juce::String HI_BAND          = "hiBand";
    static juce::String WET_DRY_MIX      = "wetDryMix";
    static juce::String REVERB_FREEZE    = "reverbFreeze";
    static juce::String INVERT_DIRECTION = "invertDirection";
    static juce::String BEAT_SYNC        = "beatSync";
    
    namespace Ranges {
        static float LOW_BAND_MIN = 20.f;
        static float LOW_BAND_MAX = 500.f;
        static float MID_BAND_MIN = 500.f;
        static float MID_BAND_MAX = 3000.f;
        static float HI_BAND_MIN  = 5000.f;
        static float HI_BAND_MAX  = 11025.f;
    }

    namespace Config {

        // LFO speed is defined in Hz

        static float LFO_MIN_RATE = 0.1f;
        static float LFO_MAX_RATE = 1.f;
        
        static float DISTORTION_AMT_DEF = 0.5f;
        static float DISTORTION_WET_DEF = DISTORTION_AMT_DEF;

        static float LOW_BAND_DEF = 200.f;
        static float MID_BAND_DEF = 1000.f;
        static float HI_BAND_DEF  = 5000.f;

        static float WET_DRY_MIX_DEF = 1.f; // 100 % wet

        static float REVERB_WIDTH_DEF  = 0.15f;
        static float REVERB_SIZE_DEF   = 1.f;
        static float REVERB_FREEZE_TIMEOUT = 0.15f; // in seconds

        static bool INVERT_DIR_DEF = true;

        static const int NUM_COMBS     = 8;
        static const int NUM_ALLPASSES = 4;
        static const int COMB_TUNINGS[ NUM_COMBS ] = { 1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617 };
        static const int ALLPASS_TUNINGS[ NUM_ALLPASSES ] = { 556, 441, 341, 225 };
    }
}
