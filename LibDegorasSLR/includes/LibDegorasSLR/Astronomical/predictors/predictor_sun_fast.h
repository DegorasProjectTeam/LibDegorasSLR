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
 * @file predictor_sun_fast.h
 * @brief
 * @author Degoras Project Team.
 * @copyright EUPL License
 * @version
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
//======================================================================================================================
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
#include "LibDegorasSLR/Astronomical/predictors/predictor_sun.h"
#include "LibDegorasSLR/Geophysics/types/geodetic_point.h"
#include "LibDegorasSLR/Timing/types/base_time_types.h"
#include "LibDegorasSLR/Astronomical/types/astro_types.h"
#include "LibDegorasSLR/Mathematics/types/vector3d.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace astro{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using timing::types::J2000DateTime;
using timing::types::J2000DateTimeV;
using timing::types::MJDate;
using timing::types::SoD;
using timing::types::MJDateTime;
using geo::types::GeodeticPoint;
using math::units::MillisecondsU;
using math::types::Vector3D;
using astro::types::AltAzPos;
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief The PredictorSunFast class provides functionality to predict the position of the Sun using a fast algorithm.
 *
 * This class utilizes astronomical algorithms to calculate the azimuth and elevation of the Sun at a given time
 * and observer's geodetic coordinates. This algorithm as a 0.01 degree accuracy.
 */
class LIBDPSLR_EXPORT PredictorSunFast : public PredictorSun
{

public:

    /**
     * @brief Constructs a PredictorSunFast object with the given observer's geodetic coordinates.
     * @param obs_geod The geodetic coordinates of the observer.
     */
    PredictorSunFast(const GeodeticPoint<Degrees>& obs_geod);

    PredictorSunFast(const PredictorSunFast&) = default;
    PredictorSunFast(PredictorSunFast&&) = default;
    PredictorSunFast& operator =(const PredictorSunFast&) = default;
    PredictorSunFast& operator =(PredictorSunFast&&) = default;

    /**
     * @brief Predicts the position of the Sun at a specific time using a fast algorithm.
     *
     * Using a simple algorithm (VSOP87 algorithm is much more complicated), this function predicts the Sun position
     * with a 0.01 degree accuracy up to 2099. It can perform also a simple atmospheric refraction correction. The
     * time precision, internally, is decreased to milliseconds (for this type of prediction it is enough).
     *
     * @param j2000 The J2000DateTime object representing the J2000 date and time of the prediction.
     * @param refraction Flag indicating whether to apply atmospheric refraction correction.
     * @return The predicted SunPrediction.
     *
     * @note Reimplemented from: 'Book: Sun Position: Astronomical Algorithm in 9 Common Programming Languages'.
     */
    SunPrediction predict(const J2000DateTime& j2000, bool refraction) const override;

    /**
     * @brief Predicts Sun positions within a time range with a specified time step using a fast algorithm.
     *
     * Using a simple algorithm (VSOP87 algorithm is much more complicated), this function predicts in parallel
     * all Sun position within a time range with a specified time step, with a 0.01 degree accuracy up to 2099.
     * It can perform also a simple atmospheric refraction correction. The time precision, internally, is decreased to
     * milliseconds (for this type of prediction it is enough).
     *
     * @param j2000_start The J2000 start datetime of the prediction range.
     * @param j2000_end The J2000 end datetime of the prediction range.
     * @param step The time step in milliseconds between predictions.
     * @param refraction Flag indicating whether to apply atmospheric refraction correction.
     * @return A vector of SunPrediction objects representing predicted sun positions at each step.
     *
     * @throws std::invalid_argument If the interval is invalid.
     */
    SunPredictionV predict(const J2000DateTime& j2000_start, const J2000DateTime& j2000_end,
                           MillisecondsU step, bool refraction) const override;

};

}} // END NAMESPACES.
// =====================================================================================================================
