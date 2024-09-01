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
#include <limits>
// #include "../interpolator/CubicInterpolator.h"
#include "../interpolator/RateInterpolator.h"
#include "../oscillator/LFO.h"
#include "../../Parameters.h"

class DopplerEffect
{
    const float MIN_DOPPLER_RATE       = 0.5f;
    const float MAX_DOPPLER_RATE       = 2.0f;
    const float MIN_OBSERVER_DISTANCE  = 1.f;
    const float MAX_OBSERVER_DISTANCE  = 10.f;
    const float SPEED_OF_SOUND         = 343.0f; // in m/s
    const float TWO_PI                 = 2.f * juce::MathConstants<float>::pi;
    const float DC_OFFSET_FILTER       = 0.995f;
    const float MAX_LFO_CYCLE_DURATION = 1.0f / Parameters::Config::LFO_MIN_RATE; // duration of the slowest LFO cycle in seconds
    const float LFO_DEPTH              = ( 1.f / MAX_OBSERVER_DISTANCE ) * 0.025f;
    const float INTERPOLATION_SPEED    = 0.005f; // 0.0005f is interesting as it provides a tape slowdown effect
    const float CROSSFADE_DURATION     = 0.01f; // in seconds

    public:
        DopplerEffect( double sampleRate, int bufferSize );
        ~DopplerEffect();

        void setProperties( float speed, bool invert, bool sync );
        void setRecordingLength( float durationInSeconds );
        void updateTempo( double tempo, int timeSigNominator, int timeSigDenominator );
        void onSequencerStart();

        // applies the Doppler effect onto the provided buffer at provided channel
        // (Doppler effect applies onto individual (mono) channels, not groups)

        void apply( juce::AudioBuffer<float>& buffer, int channel );

    private:
        // CubicInterpolator cubicInterpolator;
        RateInterpolator rateInterpolator;
        RateInterpolator speedInterpolator;
        LFO lfo;
        bool interpolateRate = true;
        
        void recordInput( juce::AudioBuffer<float>& buffer, int channel );
        void resetRecordBuffer();
        void onPostApply( int readBuffers );

        inline float getResampledValue( float dopplerRate, int readPos, int readOffset )
        {
            float resampledIndex;

            // calculate the read index of the sample inside the record buffer

            if ( invertDirection ) {
                resampledIndex = ( readPos + readOffset ) * dopplerRate;
            } else {
                resampledIndex = ( readPos + readOffset ) / dopplerRate;
            }
    
            // ensure the resampleIndex remains within record bounds

            resampledIndex = fmod( resampledIndex, fRecordBufferSize );
            if ( resampledIndex < 0 ) {
                resampledIndex += fRecordBufferSize;
            }
            int index  = static_cast<int>( resampledIndex );
            float frac = resampledIndex - static_cast<float>( index );

            // calculate sample value using (more accurate) cubic interpolation

            // float sampleValue = cubicInterpolator.getInterpolatedSample( recordBuffer, recordBufferSize, index, frac );

            // calculate sample value using (faster) linear interpolation
            
            int nextIndex = ( index + 1 ) % recordBufferSize;
            float sampleValue = recordBuffer.getSample( 0, index ) * ( 1.0f - frac ) +
                                recordBuffer.getSample( 0, nextIndex ) * frac;

            return sampleValue;
        }

        inline int getSyncedReadPosition()
        {
            return writePosition - ( invertDirection ? minRequiredSamplesInvert : minRequiredSamples );
        }
        
        juce::AudioBuffer<float> recordBuffer;
        float fRecordBufferSize;
        int recordBufferSize;
        int maxRecordBufferSize = 0;
        int readPosition;
        int writePosition;
        bool invertDirection = true;
        bool syncToBeat = false;
        
        bool readFromRecordBuffer = false;
        int totalRecordedSamples;
        int processedSamples;
        int minRequiredSamples;
        int minRequiredSamplesInvert;
        int samplesPerBeat = std::numeric_limits<int>::max();

        float previousSampleValue   = 0.0f;
        float previousFilteredValue = 0.0f;
        float crossfadeSize;
        int crossfadeSamplesLeft;
        int crossfadedSamples;

        float _sampleRate;
        int   _bufferSize;
};