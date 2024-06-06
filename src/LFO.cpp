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
#include "LFO.h"

LFO::LFO( float sampleRate ) {
    _rate          = LFO::MIN_LFO_RATE();
    _accumulator   = 0.f;
    _sampleRate    = sampleRate;
    SR_OVER_LENGTH = sampleRate / ( float ) TABLE_SIZE;
}

LFO::~LFO() {

}

/* public methods */

float LFO::getRate()
{
    return _rate;
}

void LFO::setRate( float value )
{
    _rate = value;
}

void LFO::setAccumulator( float value )
{
    _accumulator = value;
}

float LFO::getAccumulator()
{
    return _accumulator;
}
