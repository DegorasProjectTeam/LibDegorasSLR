/***********************************************************************************************************************
 *   LibDPSLR (Degoras Project SLR Library): A libre base library for SLR related developments.                        *                                      *
 *                                                                                                                     *
 *   Copyright (C) 2023 Degoras Project Team                                                                           *
 *                      < Ángel Vera Herrera, avera@roa.es - angeldelaveracruz@gmail.com >                             *
 *                      < Jesús Relinque Madroñal >                                                                    *
 *                                                                                                                     *
 *   This file is part of LibDPSLR.                                                                                    *
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
 * @file astro_utils.h
 * @brief This file contains the implementation of several astronomical usefull functions.
 * @author Degoras Project Team
 * @copyright EUPL License
 2306.1
***********************************************************************************************************************/

// C++ INCLUDES
// =====================================================================================================================
#include "math.h"
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include <LibDegorasSLR/Astronomical/utils/astro_utils.h>
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace astro{
// =====================================================================================================================

void azElToRaDec(long double az, long double el, long double lmst, long double lat, long double &ra, long double &dec)
{
    // Calculate declination.
    dec = std::asin(std::sin(el) * std::sin(lat) + std::cos(el) * std::cos(lat) * std::cos(az));

    // Calculate intermediate variables for further calculations.
    long double sinv = -(std::sin(az) * std::cos(el) * std::cos(lat)) / (std::cos(lat) * std::cos(dec));
    long double cosv = (std::sin(el) - std::sin(lat) * std::sin(dec)) / (std::cos(lat) * std::cos(dec));

    // Calculate local hour angle.
    long double lha = std::atan2(sinv, cosv);

    // Calculate right ascension.
    ra = lmst - lha;
}

void raDecToAzEl(long double ra, long double dec, long double lmst, long double lat, long double &az, long double &el)
{
    // Local hour angle
    long double lha = lmst - ra;

    el = std::asin(std::sin(dec) * std::sin(lat) + std::cos(dec) * std::cos(lat) * std::cos(lha));
    long double sinv = -std::sin(lha) * std::cos(dec) * std::cos(lat) / (std::cos(el) * std::cos(lat));
    long double cosv = (std::sin(dec) - std::sin(el) * std::sin(lat)) / (std::cos(el) * std::cos(lat));
    az = std::atan2(sinv, cosv);
}


}} // END NAMESPACES.
// =====================================================================================================================
