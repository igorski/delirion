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

/**
 * A class that interpolates changes to oscillator speed (rate) values
 * to create a smoother transition
 */
class RateInterpolator
{
    public:
        RateInterpolator( float initialValue, float aSmoothingFactor )
            : currentValue( initialValue ), targetValue( initialValue ), smoothingFactor( aSmoothingFactor ) {}

        inline float setValue( float newValue )
        {
            targetValue   = newValue;
            currentValue += smoothingFactor * ( targetValue - currentValue );

            return currentValue;
        }

    private:
        float currentValue;
        float targetValue;
        float smoothingFactor;
};
