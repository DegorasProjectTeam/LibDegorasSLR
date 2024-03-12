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
 * @file prediction_data_slr.h
 * @author Degoras Project Team.
 * @brief This file contains the declaration of the types used by `PredictorSLR` utility.
 * @copyright EUPL License.
 * @version
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Timing/types/datetime_types.h"
#include "LibDegorasSLR/libdegorasslr_global.h"
#include "LibDegorasSLR/Geophysics/types/geocentric_point.h"
#include "LibDegorasSLR/Geophysics/types/geocentric_velocity.h"
#include "LibDegorasSLR/Astronomical/types/astro_types.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace slr{
// =====================================================================================================================

/**
 * This struct contains the computed data when applying the `PredictorSLR::PredictionMode::ONLY_INSTANT_RANGE` mode.
 * The distance and flight time values may include corrections such as the eccentricity correction at the object and
 * at the ground, the signal delay (station calibration), and the systematic and random observation errors. If the
 * corrections are not included, the corresponding optional paramath::units::Meters will not be accessible in the higher level
 * structure (`PredictionSLR`).
 *
 * @warning In this case, the tropospheric correction is never included becaouse the algorithm not calculates the
 * altazimuth position of the object related to the local observer position. However, the precision is enought for
 * real time systems like range gate generator devices.
 *
 * @see Enumeration `PredictorSLR::PredictionMode`.
 * @see Struct `PredictionSLR`.
 */
struct LIBDPSLR_EXPORT InstantRange
{
    M_DEFINE_CTOR_DEF_COPY_MOVE_OP_COPY_MOVE_DTOR_DEF(InstantRange)

    /**
     * @brief Represents the InstantRange struct as a JSON-formatted string.
     * @return The JSON-formatted string representation of InstantRange data.
     */
    std::string toJsonStr() const;

    // Struct data.
    timing::types::MJDateTime mjdt;          ///< Modified julian datetime asociated to the data.
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
    M_DEFINE_CTOR_DEF_COPY_MOVE_OP_COPY_MOVE_DTOR_DEF(InstantData)

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
    M_DEFINE_CTOR_DEF_COPY_MOVE_OP_COPY_MOVE_DTOR_DEF(OutboundData)
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
    M_DEFINE_CTOR_DEF_COPY_MOVE_OP_COPY_MOVE_DTOR_DEF(InboundData)

    // Datetime members.
    timing::types::MJDateTime mjdt;          ///< Modified julian datetime.

    // Range (1 way) and time of flight (2 way).
    math::units::Meters range_1w;          ///< One way range in math::units::Meters (mm precission -> 3 decimals).
    math::units::Seconds tof_2w;           ///< Two way flight time in math::units::Seconds (ps precission -> 12 decimals).

    /**
     * @brief Represents the InboundData struct as a JSON-formatted string.
     * @return The JSON-formatted string representation of InboundData.
     */
    std::string toJsonStr() const;
};

/**
 * @brief
 * This container has all the data returned by the predictor. The InstantRange always will be disponible. The
 * rest of the data will be available or not depending on the selected computing mode. The azimuth and elevation
 * difference between receive and transmit direction at instant time paramath::units::Meters will only be available in
 * the PredictionMode::OUTBOUND_VECTOR and PredictionMode::INBOUND_VECTOR modes. You can check the corrections
 * applied by accessing the corresponding paramath::units::Meters.
 *
 */
struct LIBDPSLR_EXPORT PredictionSLR
{
    // Constructors.
    PredictionSLR() = default;
    PredictionSLR(const PredictionSLR&) = default;
    PredictionSLR(PredictionSLR&&) = default;

    // Operators.
    PredictionSLR& operator=(const PredictionSLR&) = default;
    PredictionSLR& operator=(PredictionSLR&&) = default;

    // Result containers for the different modes.
    InstantRange instant_range;           ///< Result range for the instant time in the ONLY_INSTANT_RANGE mode.
    Optional<InstantData> instant_data;   ///< Result data for the instant time (instant vectors).
    Optional<OutboundData> outbound_data; ///< Result data for the bounce time (outbound vectors).
    Optional<InboundData> inbound_data;   ///< Result data for the arrival time (inbound vector).

    // Difference between receive and transmit direction at instant time.
    Optional<math::units::Degrees> diff_az;   ///< Azimuth difference between outbound and instant vectors (4 decimals).
    Optional<math::units::Degrees> diff_el;   ///< Elevation difference between outbound and instant vectors (4 decimals).

    // Corrections applied.
    Optional<math::units::Picoseconds> cali_del_corr; ///< Station calibration delay correction (picomath::units::Seconds, 2 way).
    Optional<math::units::Meters> objc_ecc_corr;      ///< Eccentricity correction at the object (math::units::Meters, 1 way, usually CoM).
    Optional<math::units::Meters> grnd_ecc_corr;      ///< Eccentricity correction at the ground (math::units::Meters, usually not used).
    Optional<math::units::Meters> corr_tropo;         ///< Tropospheric path delay correction (math::units::Meters, 1 way).
    Optional<math::units::Meters> syst_rnd_corr;      ///< Other systematic and random error corrections (math::units::Meters, 1 way).

    // Error code.
    int error;  ///< Error that may have occurred. Zero is always reserved for NOT_ERROR.

    /**
     * @brief Represents all the prediction result as a JSON formated string.
     */
    std::string toJsonStr() const;
};

/// Alias for PredictionSLR vector.
using PredictionSLRV = std::vector<PredictionSLR>;

}} // END NAMESPACES
// =====================================================================================================================
