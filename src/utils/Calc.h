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

namespace Calc
{
    /**
     * convert given value in seconds to the appropriate
     * value in samples (at the provided sampling rate)
     */
    inline int secondsToBuffer( float seconds, float sampleRate )
    {
        return static_cast<int>( seconds * sampleRate );
    }

    inline float bufferToSeconds( int bufferSize, float sampleRate )
    {
        return static_cast<float>( bufferSize ) / sampleRate;
    }
}