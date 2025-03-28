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

***********************************************************************************************************************/

// C++ INCLUDES
// =====================================================================================================================
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Astronomical/predictors/predictor_star_novas.h"
#include "LibDegorasSLR/Astronomical/utils/novas_utils.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace astro{
namespace predictors{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using namespace novas;
using namespace dpbase::timing::types;
using namespace astro::types;
using namespace geo::types;
using namespace dpbase::math::units;
// ---------------------------------------------------------------------------------------------------------------------

PredictorStarNovas::PredictorStarNovas(const Star &star, const SurfaceLocation<Degrees> &loc,
                                       int leap_secs, double ut1_utc_diff) :
    PredictorStarBase(star, loc, leap_secs, ut1_utc_diff)
{}

PredictionStar PredictorStarNovas::predict(const dpbase::timing::dates::JDateTime &jdt, bool refraction) const
{
    PredictionStar pred;
    pred.jdt = jdt;

    novas::getStarAltAzPos(this->star_, this->loc_, jdt, refraction, pred.altaz_coord,
                           this->leap_secs_, this->ut1_utc_diff_);

    return pred;
}

PredictionStarV PredictorStarNovas::predict(const dpbase::timing::dates::JDateTime &jdt_start,
                                            const dpbase::timing::dates::JDateTime &jdt_end,
                                            const dpbase::math::units::MillisecondsU &step, bool refraction) const
{

    // Container and auxiliar.
    dpbase::timing::dates::JDateTimeV interp_times;
    dpbase::math::units::Seconds step_sec = static_cast<long double>(step) * dpbase::math::units::kMsToSec;

    // Check for valid time interval.
    if(!(jdt_start <= jdt_end))
        throw std::invalid_argument("[LibDegorasSLR,Astronomical,PredictorStarBase::predict] Invalid interval.");

    // Calculates all the interpolation times.
    interp_times = dpbase::timing::dates::JDateTime::linspaceStep(jdt_start, jdt_end, step_sec);

    // Results container.
    PredictionStarV results(interp_times.size());

    for(size_t i = 0; i<interp_times.size(); i++)
    {
        results[i] = this->predict(interp_times[i], refraction);
    }

    // Return the container.
    return results;
}


}}} // END NAMESPACES
// =====================================================================================================================
