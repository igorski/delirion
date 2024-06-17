/**
 * Based on freeverb by Jezar at Dreampoint (June 2000)
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

class Comb
{
    public:
        Comb();
        void setBuffer( float *buf, int size );
        inline float process( float input )
        {
            float output = _buffer[ _bufIndex ];
            // undenormalise( output );

            _filterStore = ( output * _damp2 ) + ( _filterStore * _damp1 );
            // undenormalise( _filterStore );

            _buffer[_bufIndex] = input + ( _filterStore * _feedback );
            if ( ++_bufIndex >= _bufSize ) {
                _bufIndex = 0;
            }
            return output;
        }
        void mute();
        float getDamp();
        void setDamp( float val );
        float getFeedback();
        void setFeedback( float val );

    private:
        float  _feedback;
        float  _filterStore;
        float  _damp1;
        float  _damp2;
        float* _buffer;
        int _bufSize;
        int _bufIndex;
};

