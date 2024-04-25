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
 * @file prediction_slr_data.h
 * @author Degoras Project Team
 * @brief This file contains the declaration of the SLR data structs used by PredictionSLR container.
 * @copyright EUPL License
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Timing/dates/datetime_types.h"
#include "LibDegorasSLR/libdegorasslr_global.h"
#include "LibDegorasSLR/Geophysics/types/geocentric_point.h"
#include "LibDegorasSLR/Geophysics/types/geocentric_velocity.h"
#include "LibDegorasSLR/Astronomical/types/alt_az_pos.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace slr{
namespace predictors{
// =====================================================================================================================

/**
 * @brief This struct contains the SLR instant range data computed by PredictorSLR predictor.
 *
 * This struct contains the SLR instant range computed data by PredictorSLR predictor, when applying any mode of
 * `PredictorSLR::PredictionMode` (because it is the most basic information that can be obtained from the predictor).
 *
 * The distance and flight time values may include corrections such as the eccentricity correction at the object and
 * at the ground, the signal delay (station calibration), and systematic and random observation errors. If the
 * corrections are not included, the corresponding optional parameters will not be accessible in the higher level
 * structure (`PredictionSLR`).
 *
 * @warning In this case, the tropospheric correction is never included becaouse the algorithm not calculates the
 * altazimuth position of the object related to the local observer position. However, the precision is enought for
 * real time systems like range gate generator devices or fast precalculations.
 *
 * @see Enumeration PredictorSLR::PredictionMode
 * @see Struct `PredictionSLR`
 */
struct LIBDPSLR_EXPORT InstantRange
{
    // Default constructor and destructor, copy and movement constructor and operators.
    M_DEFINE_CTOR_DEF_COPY_MOVE_OP_COPY_MOVE_DTOR(InstantRange)

    /**
     * @brief Represents the InstantRange struct as a JSON-formatted string.
     * @return The JSON-formatted string representation of InstantRange data.
     */
    std::string toJsonStr() const;

    // Struct data.
    timing::dates::MJDateTime mjdt;          ///< Modified julian datetime asociated to the data.
    math::units::Meters range_1w;          ///< One way range in math::units::Meters (mm precision -> 3 decimals).
    math::units::Seconds tof_2w;           ///< Two way flight time in math::units::Seconds (ps precision -> 12 decimals).
    geo::types::GeocentricPoint geo_pos;  ///< Object geocentric interpolated positions in math::units::Meters (x, y, z).
};

/**
 * This struct contains the computed data when applying the PredictionMode::INSTANT_VECTOR mode. The distance
 * and flight time values may include all types of corrections. If they are not included, the corresponding
 * optional paramath::units::Meters will not be accessible in the higher level structure (PredictorSLR::PredictionResult).
 *
 * @warning In this case, all the correction could be included.
 *
 * @see PredictionMode::INSTANT_VECTOR
 * @see PredictorSLR::PredictionResult
 */
struct LIBDPSLR_EXPORT InstantData : public InstantRange
{
    // Default constructor and destructor, copy and movement constructor and operators.
    M_DEFINE_CTOR_DEF_COPY_MOVE_OP_COPY_MOVE_DTOR(InstantData)

    /**
     * @brief Constructor.
     */
    InstantData(InstantRange&&);

    // Associated object geocentric vectors.
    // TODO
    geo::types::GeocentricVelocity geo_vel;   ///< Geocentric interpolated velocity in meters/second.

    // Azimuth and elevation for the instant vector.
    astro::types::AltAzPos altaz_coord;        ///< Local computed altazimuth coordinates in degrees (4 decimals).

    /**
     * @brief Represents the InstantData struct as a JSON-formatted string.
     * @return The JSON-formatted string representation of InstantData.
     */
    std::string toJsonStr() const;
};

/**
 * This struct contains the computed data when applying the PredictionMode::OUTBOUND_VECTOR mode. The distance
 * and flight time values may include all types of corrections. If they are not included, the corresponding
 * optional paramath::units::Meters will not be accessible in the higher level structure (PredictorSLR::PredictionResult).
 *
 * @warning In this case, all the correction could be included.
 *
 * @see PredictionMode::OUTBOUND_VECTOR
 * @see PredictorSLR::PredictionResult
 */
struct LIBDPSLR_EXPORT OutboundData : public InstantData
{
    // Default constructor and destructor, copy and movement constructor and operators.
    M_DEFINE_CTOR_DEF_COPY_MOVE_OP_COPY_MOVE_DTOR(OutboundData)
};

/**
 * This struct contains the computed data when applying the PredictionMode::INBOUND_VECTOR mode. The distance
 * and flight time values may include all types of corrections. If they are not included, the corresponding
 * optional paramath::units::Meters will not be accessible in the higher level structure (PredictorSLR::PredictionResult).
 *
 * @warning In this case, all the correction could be included.
 *
 * @see PredictionMode::INBOUND_VECTOR
 * @see PredictorSLR::PredictionResult
 */
struct LIBDPSLR_EXPORT InboundData
{
    // Default constructor and destructor, copy and movement constructor and operators.
    M_DEFINE_CTOR_DEF_COPY_MOVE_OP_COPY_MOVE_DTOR(InboundData)

    // Datetime members.
    timing::dates::MJDateTime mjdt;          ///< Modified julian datetime.

    // Range (1 way) and time of flight (2 way).
    math::units::Meters range_1w;          ///< One way range in math::units::Meters (mm precission -> 3 decimals).
    math::units::Seconds tof_2w;           ///< Two way flight time in math::units::Seconds (ps precission -> 12 decimals).

    /**
     * @brief Represents the InboundData struct as a JSON-formatted string.
     * @return The JSON-formatted string representation of InboundData.
     */
    std::string toJsonStr() const;
};

}}} // END NAMESPACES
// =====================================================================================================================
