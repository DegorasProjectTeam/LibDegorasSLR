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
 * @file predictor_sun.h
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
#include "LibDegorasSLR/Geophysics/types/geodetic_point.h"
#include "LibDegorasSLR/Timing/types/time_types.h"
#include "LibDegorasSLR/Astronomical/sun_utils/sun_position.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace astro{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using dpslr::timing::types::J2DateTime;
using dpslr::timing::types::MJDate;
using dpslr::timing::types::SoD;
using dpslr::timing::types::MJDateTime;
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief The PredictorSun class provides functionality to predict the position of the Sun.
 *
 * This class utilizes astronomical algorithms to calculate the azimuth and elevation of the Sun at a given time
 * and observer's geodetic coordinates.
 *
 * @warning At this moment, only the function fastPredict (0.01 degree accuracy) is implemented.
 */
class LIBDPSLR_EXPORT PredictorSun
{

public:

    struct SunPrediction
    {
        // Default constructor.
        SunPrediction() = default;

        // Datetime members.
        MJDate mjd;              ///< Modified Julian Date in days.
        SoD sod;                 ///< Second of day in that Modified Julian Date.
        MJDateTime mjdt;         ///< Modified Julian DateTime (day & fraction).
        J2DateTime j2dt;         ///< J2000 DateTime (day & fraction).

        // Sun position.
        SunPosition position;    ///< Predicted Sun position.
    };

    /// Alias for a vector of SunPrediction.
    using SunPredictions = std::vector<SunPrediction>;

    /**
     * @brief Constructs a PredictorSun object with the given observer's geodetic coordinates.
     * @param obs_geod The geodetic coordinates of the observer.
     */
    PredictorSun(const geo::types::GeodeticPoint<long double>& obs_geod);

    /**
     * @brief Predicts the position of the Sun at a specific time using a fast algorithm.
     *
     * Using a simple algorithm (VSOP87 algorithm is much more complicated), this function predicts
     * the Sun position with a 0.01 degree accuracy up to 2099. It can perform also a simple
     * atmospheric refraction correction.
     *
     * @param j2000 The Julian Date (J2000) of the prediction.
     * @param refraction Flag indicating whether to apply atmospheric refraction correction.
     * @return The predicted SunPrediction.
     *
     * @note The calculation algorithm was extracted from: 'Book: Sun Position: Astronomical Algorithm
     * in 9 Common Programming Languages'.
     */
    SunPrediction fastPredict(const J2DateTime& j2000, bool refraction = false) const;

    /**
     * @brief Predicts Sun positions within a time range with a specified time step using a fast algorithm.
     *
     * Using a simple algorithm (VSOP87 algorithm is much more complicated), this function predicts in parallel
     * all Sun position within a time range with a specified time step, with a 0.01 degree accuracy up to 2099.
     * It can perform also a simple atmospheric refraction correction.
     *
     * @param j2000_start The start Julian Date (J2000) of the prediction range.
     * @param j2000_end The end Julian Date (J2000) of the prediction range.
     * @param step_ms The time step in milliseconds between predictions.
     * @param refraction Flag indicating whether to apply atmospheric refraction correction.
     * @return A vector of SunPrediction objects representing predicted sun positions at each step.
     *
     * @throws std::invalid_argument If the interval is invalid.
     *
     * @note The calculation algorithm was extracted from: 'Book: Sun Position: Astronomical Algorithm
     * in 9 Common Programming Languages'.
     */
    SunPredictions fastPredict(const J2DateTime& j2000_start, const J2DateTime& j2000_end,
                               unsigned step_ms, bool refraction = false) const;

private:

    long double obs_lat_;  ///< Geodetic observer latitude in radians.
    long double obs_lon_;  ///< Geodetic observer longitude in radians.
    long double obs_alt_;  ///< Observer altitude in meters.
};

}} // END NAMESPACES.
// =====================================================================================================================