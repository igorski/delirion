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
#include "LFO.h"

LFO::LFO( double sampleRate )
{
    _sampleRate = static_cast<float>( sampleRate );
    _depth      = 1.f;
    _phase      = 0.f;

    setRate( MIN_LFO_RATE );
}

LFO::~LFO()
{

}

/* public methods */

float LFO::getRate()
{
    return _rate;
}

void LFO::setRate( float value )
{
    _rate = value;
    _phaseIncrement = _rate / _sampleRate;
}

float LFO::getDepth()
{
    return _depth;
}

void LFO::setDepth( float value )
{
    _depth = value;
}

float LFO::getPhase()
{
    return _phase;
}

void LFO::setPhase( float value )
{
    _phase = value;
}
