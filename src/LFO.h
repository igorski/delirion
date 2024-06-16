/*
 * Copyright (c) 2013-2018 Igor Zinken https://www.igorski.nl
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

// sine waveform used for the oscillator
static const int TABLE_SIZE = 128;
static const float TABLE[ TABLE_SIZE ] = { 0.f, 0.0490677f, 0.0980171f, 0.14673f, 0.19509f, 0.24298f, 0.290285f, 0.33689f, 0.382683f, 0.427555f, 0.471397f, 0.514103f, 0.55557f, 0.595699f, 0.634393f, 0.671559f, 0.707107f, 0.740951f, 0.77301f, 0.803208f, 0.83147f, 0.857729f, 0.881921f, 0.903989f, 0.92388f, 0.941544f, 0.95694f, 0.970031f, 0.980785f, 0.989177f, 0.995185f, 0.998795f, 1.f, 0.998795f, 0.995185f, 0.989177f, 0.980785f, 0.970031f, 0.95694f, 0.941544f, 0.92388f, 0.903989f, 0.881921f, 0.857729f, 0.83147f, 0.803208f, 0.77301f, 0.740951f, 0.707107f, 0.671559f, 0.634393f, 0.595699f, 0.55557f, 0.514103f, 0.471397f, 0.427555f, 0.382683f, 0.33689f, 0.290285f, 0.24298f, 0.19509f, 0.14673f, 0.0980171f, 0.0490677f, 1.22465e-16f, -0.0490677f, -0.0980171f, -0.14673f, -0.19509f, -0.24298f, -0.290285f, -0.33689f, -0.382683f, -0.427555f, -0.471397f, -0.514103f, -0.55557f, -0.595699f, -0.634393f, -0.671559f, -0.707107f, -0.740951f, -0.77301f, -0.803208f, -0.83147f, -0.857729f, -0.881921f, -0.903989f, -0.92388f, -0.941544f, -0.95694f, -0.970031f, -0.980785f, -0.989177f, -0.995185f, -0.998795f, -1.f, -0.998795f, -0.995185f, -0.989177f, -0.980785f, -0.970031f, -0.95694f, -0.941544f, -0.92388f, -0.903989f, -0.881921f, -0.857729f, -0.83147f, -0.803208f, -0.77301f, -0.740951f, -0.707107f, -0.671559f, -0.634393f, -0.595699f, -0.55557f, -0.514103f, -0.471397f, -0.427555f, -0.382683f, -0.33689f, -0.290285f, -0.24298f, -0.19509f, -0.14673f, -0.0980171f, -0.0490677f };

class LFO
{
    static const float MAX_LFO_RATE() { return 10.f; }
    static const float MIN_LFO_RATE() { return .1f; }

    public:
        LFO( float sampleRate );
        ~LFO();

        float getRate();
        void setRate( float value );

        // accumulators are used to retrieve a sample from the wave table

        float getAccumulator();
        void setAccumulator( float offset );

        /**
         * retrieve a value from the wave table for the current
         * accumulator position, this method also increments
         * the accumulator and keeps it within bounds
         */
        inline float peek()
        {
            // the wave table offset to read from
            int readOffset = ( _accumulator == 0.f ) ? 0 : ( int ) ( _accumulator / SR_OVER_LENGTH );

            // increment the accumulators read offset
            _accumulator += _rate;

            // keep the accumulator within the bounds of the sample frequency
            if ( _accumulator > _sampleRate ) {
                _accumulator -= _sampleRate;
            }
            // return the sample present at the calculated offset within the table
            return TABLE[ readOffset ];
        }

    private:
        float _sampleRate;
        float _rate;
        float _accumulator;   // is read offset in wave table buffer
        float SR_OVER_LENGTH;
};
