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
 * @file predictor_star.cpp
 * @brief
 * @author Degoras Project Team.
 * @copyright EUPL License
 * @version
***********************************************************************************************************************/

// C++ INCLUDES
// =====================================================================================================================
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Astronomical/predictors/predictor_star.h"
#include "LibDegorasSLR/Astronomical/novas_utils.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace astro{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using namespace timing::types;
// ---------------------------------------------------------------------------------------------------------------------

dpslr::astro::PredictorStar::PredictorStar(const types::Star &star,
                                           const geo::types::SurfaceLocation<Degrees> &loc,
                                           int leap_secs,
                                           double ut1_utc_diff) :
    star_(star),
    loc_(loc),
    leap_secs_(leap_secs),
    ut1_utc_diff_(ut1_utc_diff)
{

}

PredictorStar::StarPrediction PredictorStar::predict(const timing::types::JDateTime &jdt) const
{
    PredictorStar::StarPrediction pred;
    pred.jdt = jdt;

    astro::novas::getStarAltAzPos(this->star_, this->loc_, jdt, pred.altaz_coord,
                                  this->leap_secs_, this->ut1_utc_diff_);

    return pred;
}

PredictorStar::StarPredictions PredictorStar::predict(const timing::types::JDateTime &jdt_start,
                                                      const timing::types::JDateTime &jdt_end,
                                                      math::units::MillisecondsU step) const
{
    // Container and auxiliar.
    JDateTimeV interp_times;
    math::units::Seconds step_sec = static_cast<long double>(step) * math::units::kMsToSec;

    // Check for valid time interval.
    if(!(jdt_start <= jdt_end))
        throw std::invalid_argument("[LibDegorasSLR,Astronomical,PredictorSun::fastPredict] Invalid interval.");

    // Calculates all the interpolation times.
    interp_times = JDateTime::linspaceStep(jdt_start, jdt_end, step_sec);

    // Results container.
    StarPredictions results(interp_times.size());

    // Parallel calculation.
#pragma omp parallel for
    for(size_t i = 0; i<interp_times.size(); i++)
    {
        results[i] = this->predict(interp_times[i]);
    }

    // Return the container.
    return results;
}


}} // END NAMESPACES
// =====================================================================================================================
