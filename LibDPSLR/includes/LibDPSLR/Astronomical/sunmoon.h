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
 * @file sunmoon.h
 * @brief This file contains definition of functions to deal with sun and moon position.
 * @author Degoras Project Team.
 * @copyright EUPL License
 * @version 2305.1
 * @todo Moon calculation.
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
//======================================================================================================================
#include <cmath>
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include "LibDPSLR/libdpslr_global.h"
#include "LibDPSLR/Mathematics/units.h"
// =====================================================================================================================


// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace astro{
// =====================================================================================================================


// ========== FUNCTIONS ================================================================================================

// Simple algorithm (VSOP87 algorithm is much more complicated). 0.01 degree accuracy, up to 2099. Only for non scientific purposes.
//    Inspiration from: http ://stjarnhimlen.se/comp/tutorial.html#5
// Book: Sun Position: Astronomical Algorithm in 9 Common Programming Languages
LIBDPSLR_EXPORT void simpleSunPosition(double j2000, double lat, double lon, bool refr, double& az, double& el)
{
    // Convert latitude and longitude to radians.
    double rlat = math::units::rad(lat);
    double rlon = math::units::rad(lon);

    // Local sidereal time.
    long double sidereal = 4.894961213 + 6.300388099 * j2000 + rlon;

    // Mean longitude and anomaly of the sun.
    long double mean_long = j2000 * 1.720279239e-2 + 4.894967873;
    long double mean_anom = j2000 * 1.720197034e-2 + 6.240040768;

    // Ecliptic longitude of the sun.
    long double eclip_long = mean_long + 3.342305518e-2 * std::sin(mean_anom) + 3.490658504e-4 * std::sin(2 * mean_anom);

    // Obliquity of the ecliptic
    long double obliquity = 0.4090877234 - 6.981317008e-9 * j2000;

    // Right ascension of the sun and declination.
    long double rasc = std::atan2(std::cos(obliquity) * std::sin(eclip_long), std::cos(eclip_long));
    long double decl = std::asin(std::sin(obliquity) * std::sin(eclip_long));

    // Hour angle of the sun
    long double hour_ang = sidereal - rasc;

    // Local elevation and azimuth of the sun.
    double elevation = std::asin(std::sin(decl) * std::sin(rlat) + std::cos(decl) *
                                 std::cos(rlat) * std::cos(hour_ang));
    double azimuth = std::atan2(-std::cos(decl) * std::cos(rlat) * std::sin(hour_ang),
                                std::sin(decl) - std::sin(rlat) * std::sin(elevation));

    // Convert azimuth and elevation to degrees and normalize.
    elevation = math::normalizeVal(math::units::deg(elevation), -180.0, 180.0);
    azimuth = math::normalizeVal(math::units::deg(azimuth), 0.0, 360.0);

    // Very simple refraction correction (not necessary).
    // Pressure and temperature could be added but for our
    // applications greater precision is not necessary.
    if(refr && (elevation >= -1 * (0.26667 + 0.5667)))
    {
        double targ = math::units::rad((elevation + (10.3/(elevation + 5.11))));
        elevation += (1.02/tan(targ)) / 60.0;
    }

    // Return azimuth and elevation in degrees
    az = azimuth;
    el = elevation;
}

}}


