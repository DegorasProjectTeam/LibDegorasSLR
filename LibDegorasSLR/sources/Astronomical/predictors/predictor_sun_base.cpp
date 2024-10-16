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
 * @file predictor_sun_base.cpp
 * @brief
 * @author Degoras Project Team.
 * @copyright EUPL License
***********************************************************************************************************************/

// C++ INCLUDES
// =====================================================================================================================
#include <omp.h>
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_init.h"
#include "LibDegorasSLR/Astronomical/predictors/predictor_sun_base.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace astro{
namespace predictors{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using namespace dpbase::timing::dates;
using namespace geo::types;
using namespace dpbase::math::units;
using namespace astro::types;
// ---------------------------------------------------------------------------------------------------------------------

PredictorSunBase::PredictorSunBase(const GeodeticPointDeg &obs_geod) :
    obs_geo_pos_(obs_geod.convertAngles<Radians>())
{}

PredictionSunV PredictorSunBase::predict(const J2000DateTime &j2000_start, const J2000DateTime &j2000_end,
                                         const MillisecondsU &step, bool refraction) const
{
    // Initialization guard.
    DegorasInitGuard guard;

    // Container and auxiliar.
    J2000DateTimeV interp_times;
    Seconds step_sec = static_cast<long double>(step) * dpbase::math::units::kMsToSec;

    // Check for valid time interval.
    if(!(j2000_start <= j2000_end))
        throw std::invalid_argument("[LibDegorasSLR,Astronomical,PredictorSun::predict] Invalid interval.");

    // Calculates all the interpolation times.
    interp_times = J2000DateTime::linspaceStep(j2000_start, j2000_end, step_sec);

    // Results container.
    PredictionSunV results(interp_times.size());

    // Parallel calculation.
    #pragma omp parallel for
    for(size_t i = 0; i<interp_times.size(); i++)
    {
        results[i] = this->predict(interp_times[i], refraction);
    }

    // Return the container.
    return results;
}

PredictorSunBase::~PredictorSunBase(){}

}}} // END NAMESPACES
// =====================================================================================================================
