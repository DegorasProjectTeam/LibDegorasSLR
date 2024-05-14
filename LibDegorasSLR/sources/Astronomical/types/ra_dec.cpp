/***********************************************************************************************************************
 *   LibDegorasSLR (Degoras Project SLR Library).                                                                      *
 *                                                                                                                     *
 *   A modern and efficient C++ base library for Satellite Laser Ranging (SLR) software and real-time hardware         *
 *   related developments. Developed as a free software under the context of Degoras Project for the Spanish Navy      *
 *   Observatory SLR station (SFEL) in San Fernando and, of course, for any other station that wants to use it!        *
 *                                                                                                                     *
 *   Copyright (C) 2024 Degoras Project Team                                                                           *
 *                      < Ángel Vera Herrera, avera@roa.es - angeldelaveracruz@gmail.com >                             *
 *                      < Jesús Relinque Madroñal >                                                                    *
 *                                                                                                                     *
 *   This file is part of LibDegorasSLR.                                                                               *
 *                                                                                                                     *
 *   Licensed under the European Union Public License (EUPL), Version 1.2 or subsequent versions of the EUPL license   *
 *   as soon they will be approved by the European Commission (IDABC).                                                 *
 *                                                                                                                     *
 *   This project is free software: you can redistribute it and/or modify it under the terms of the EUPL license as    *
 *   published by the IDABC, either Version 1.2 or, at your option, any later version.                                 *
 *                                                                                                                     *
 *   This project is distributed in the hope that it will be useful. Unless required by applicable law or agreed to in *
 *   writing, it is distributed on an "AS IS" basis, WITHOUT ANY WARRANTY OR CONDITIONS OF ANY KIND; without even the  *
 *   implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the EUPL license to check specific   *
 *   language governing permissions and limitations and more details.                                                  *
 *                                                                                                                     *
 *   You should use this project in compliance with the EUPL license. You should have received a copy of the license   *
 *   along with this project. If not, see the license at < https://eupl.eu/ >.                                         *
 **********************************************************************************************************************/

/** ********************************************************************************************************************
 * @file astro_types.cpp
 * @brief
 * @author Degoras Project Team
 * @copyright EUPL License
***********************************************************************************************************************/

// C++ INCLUDES
// =====================================================================================================================
#include <cmath>
// =====================================================================================================================

// PROJECT INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Astronomical/types/ra_dec.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace astro{
namespace types{
// =====================================================================================================================

RA::RA(int hour, int min, double sec) :
    hour(hour),
    min(min),
    sec(sec)
{
    this->ra = hour + min / 60. + sec / 3600.;
}

RA::RA(double ra) :
    ra(ra)
{
    double integer, fract;
    fract = std::modf(ra, &integer);

    this->hour = static_cast<int>(integer);

    this->sec = std::modf(fract * 60, &integer) * 60;

    this->min = static_cast<int>(integer);
}

RA::operator double() const
{
    return this->ra;
}

bool RA::checkRA(int h, int min, double sec)
{
    return h >= 0 && h <= 23 && min >= 0 && min <= 59 && sec >= 0. && sec < 60.;
}

Dec::Dec(int deg, int min, double sec):
    deg(deg),
    min(min),
    sec(sec)
{
    this->dec = deg + min / 60. + sec / 3600.;
}

Dec::Dec(double dec) :
    dec(dec)
{
    double integer, fract;
    fract = std::modf(dec, &integer);

    this->deg = static_cast<int>(integer);

    this->sec = std::modf(fract * 60, &integer) * 60;

    this->min = static_cast<int>(integer);
}

Dec::operator double() const
{
    return this->dec;
}

bool Dec::checkDec(int deg, int min, double sec)
{
    return deg > -90 && deg < 90 && min >= 0 && min <= 59 && sec >= 0. && sec < 60.;
}

}}} // END NAMESPACES
// =====================================================================================================================
