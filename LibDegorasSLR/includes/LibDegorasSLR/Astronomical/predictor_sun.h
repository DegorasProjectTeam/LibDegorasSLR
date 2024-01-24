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
#include "LibDegorasSLR/libdpslr_global.h"
#include "LibDegorasSLR/Mathematics/math.h"
#include "LibDegorasSLR/Mathematics/units.h"
#include "LibDegorasSLR/Geo/common/geo_types.h"
// =====================================================================================================================


// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace astro{
// =====================================================================================================================

template <typename T = double, typename = typename std::enable_if<std::is_floating_point<T>::value>::type>
struct SunPosition
{
    T azimuth;
    T elevation;
};

// Simple algorithm (VSOP87 algorithm is much more complicated). 0.01 degree accuracy, up to 2099. Only for non scientific purposes.
//    Inspiration from: http ://stjarnhimlen.se/comp/tutorial.html#5
// Book: Sun Position: Astronomical Algorithm in 9 Common Programming Languages

template <typename T = double, typename = typename std::enable_if<std::is_floating_point<T>::value>::type>
class LIBDPSLR_EXPORT PredictorSun
{

public:

    PredictorSun(const geo::common::GeodeticPoint<T>& obs_geod)
    {
        // Convert latitude and longitude to radians.
        this->obs_lat_ = math::units::degToRad(obs_geod.lat);
        this->obs_lon_ = math::units::degToRad(obs_geod.lon);
        this->obs_alt_ = obs_geod.alt;
    }


    SunPosition<T> fastPredict(T j2000, bool refr) const
    {
        // Local sidereal time.
        T sidereal = 4.894961213L + 6.300388099L * j2000 + this->obs_lon_;

        // Mean longitude and anomaly of the sun.
        T mean_long = j2000 * 1.720279239e-2L + 4.894967873L;
        T mean_anom = j2000 * 1.720197034e-2L + 6.240040768L;

        // Ecliptic longitude of the sun.
        T eclip_long = mean_long + 3.342305518e-2L * std::sin(mean_anom)
                                 + 3.490658504e-4L * std::sin(2 * mean_anom);

        // Obliquity of the ecliptic
        T obliquity = 0.4090877234L - 6.981317008e-9L * j2000;

        // Right ascension of the sun and declination.
        T rasc = std::atan2(std::cos(obliquity) * std::sin(eclip_long), std::cos(eclip_long));
        T decl = std::asin(std::sin(obliquity) * std::sin(eclip_long));

        // Hour angle of the sun
        T hour_ang = sidereal - rasc;

        // Local elevation and azimuth of the sun.
        T elevation = std::asin(std::sin(decl) * std::sin(this->obs_lat_) + std::cos(decl) *
                                                                           std::cos(this->obs_lat_) * std::cos(hour_ang));
        T azimuth = std::atan2(-std::cos(decl) * std::cos(this->obs_lat_) * std::sin(hour_ang),
                                    std::sin(decl) - std::sin(this->obs_lat_) * std::sin(elevation));

        // Convert azimuth and elevation to degrees and normalize.
        elevation = math::normalizeVal(math::units::radToDegree(elevation), -180.0L, 180.0L);
        azimuth = math::normalizeVal(math::units::radToDegree(azimuth), 0.0L, 360.0L);

        // Very simple refraction correction.
        if (refr && (elevation >= -1 * (0.26667L + 0.5667L)))
        {
            T targ = math::units::degToRad((elevation + (10.3L / (elevation + 5.11L))));
            elevation += (1.02L / tan(targ)) / 60.0L;
        }

        SunPosition<T> position;
        position.azimuth = azimuth;
        position.elevation = elevation;

        return position;
    }

private:

    T obs_lat_; ///< Geodetic observer latitude in radians.
    T obs_lon_; ///< Geodetic observer longitude in radians.
    T obs_alt_; ///< Observer altitude in meters.

};

}}


