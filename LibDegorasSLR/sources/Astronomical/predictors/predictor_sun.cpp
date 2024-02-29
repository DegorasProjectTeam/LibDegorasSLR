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
 * @file predictor_sun.cpp
 * @brief
 * @author Degoras Project Team.
 * @copyright EUPL License
 * @version
***********************************************************************************************************************/

// C++ INCLUDES
// =====================================================================================================================
#include <cmath>
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Astronomical/predictors/predictor_sun.h"
#include "LibDegorasSLR/Mathematics/math.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace astro{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using namespace timing::types;
// ---------------------------------------------------------------------------------------------------------------------

dpslr::astro::PredictorSun::PredictorSun(const geo::types::GeodeticPoint<long double> &obs_geod)
{
    // Convert latitude and longitude to radians.
    this->obs_lat_ = obs_geod.lat.get(decltype(obs_geod.lat)::Unit::RADIANS);
    this->obs_lon_ = obs_geod.lon.get(decltype(obs_geod.lon)::Unit::RADIANS);
    this->obs_alt_ = obs_geod.alt;
}

PredictorSun::SunPrediction PredictorSun::fastPredict(const J2000DateTime& j2000, bool refraction) const
{
    // Store de J2000 datetime.
    long double j2000_dt = j2000.datetime();

    // Local sidereal time.
    long double sidereal = 4.894961213L + 6.300388099L * j2000_dt + this->obs_lon_;

    // Mean longitude and anomaly of the sun.
    long double mean_long = j2000_dt * 1.720279239e-2L + 4.894967873L;
    long double mean_anom = j2000_dt * 1.720197034e-2L + 6.240040768L;

    // Ecliptic longitude of the sun.
    long double eclip_long = mean_long + 3.342305518e-2L * std::sin(mean_anom)
                             + 3.490658504e-4L * std::sin(2 * mean_anom);

    // Obliquity of the ecliptic
    long double obliquity = 0.4090877234L - 6.981317008e-9L * j2000_dt;

    // Right ascension of the sun and declination.
    long double rasc = std::atan2(std::cos(obliquity) * std::sin(eclip_long), std::cos(eclip_long));
    long double decl = std::asin(std::sin(obliquity) * std::sin(eclip_long));

    // Hour angle of the sun
    long double hour_ang = sidereal - rasc;

    // Local elevation of the sun.
    long double elevation = std::asin(std::sin(decl) * std::sin(this->obs_lat_) +
                                      std::cos(decl) * std::cos(this->obs_lat_) * std::cos(hour_ang));

    // Local azimuth of the sun.
    long double azimuth = std::atan2(-std::cos(decl) * std::cos(this->obs_lat_) * std::sin(hour_ang),
                                     std::sin(decl) - std::sin(this->obs_lat_) * std::sin(elevation));

    // Convert azimuth and elevation to degrees and normalize.
    elevation = math::normalizeVal(math::units::radToDegree(elevation), -180.0L, 180.0L);
    azimuth = math::normalizeVal(math::units::radToDegree(azimuth), 0.0L, 360.0L);

    // Very simple refraction correction but enought for several applications.
    if (refraction && (elevation >= -1 * (0.26667L + 0.5667L)))
    {
        long double targ = math::units::degToRad((elevation + (10.3L / (elevation + 5.11L))));
        elevation += (1.02L / std::tan(targ)) / 60.0L;
    }

    // Final data.
    PredictorSun::SunPrediction prediction;
    prediction.altaz_coord.az = azimuth;
    prediction.altaz_coord.el = elevation;
    prediction.j2dt = j2000;

    // FORCE FOR DEBUG.
    prediction.altaz_coord.az = 225;
    prediction.altaz_coord.el = 70;

    // Retur the final position.
    return prediction;
}

PredictorSun::SunPredictions PredictorSun::fastPredict(
                                    const J2000DateTime &j2000_start, const J2000DateTime &j2000_end,
                                    MillisecondsU step_ms, bool refraction) const
{
    // Container and auxiliar.
    J2000DateTimes interp_times;
    Seconds step_sec = static_cast<long double>(step_ms) * math::units::kMsToSec;

    // Check for valid time interval.
    if(!(j2000_start <= j2000_end))
        throw std::invalid_argument("[LibDegorasSLR,Astronomical,PredictorSun::fastPredict] Invalid interval.");

    // Calculates all the interpolation times.
    interp_times = J2000DateTime::linspaceStep(j2000_start, j2000_end, step_sec);

    // Results container.
    SunPredictions results(interp_times.size());

    // Parallel calculation.
    #pragma omp parallel for
    for(size_t i = 0; i<interp_times.size(); i++)
    {
        results[i] = this->fastPredict(interp_times[i], refraction);
    }

    // Return the container.
    return results;
}

}} // END NAMESPACES
// =====================================================================================================================
