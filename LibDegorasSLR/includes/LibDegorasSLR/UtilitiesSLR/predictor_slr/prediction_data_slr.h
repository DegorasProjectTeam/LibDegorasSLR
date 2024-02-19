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
 * @file ranges_data.h
 * @author Degoras Project Team.
 * @brief
 * @copyright EUPL License.
 * @version
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
#include "LibDegorasSLR/FormatsILRS/cpf/cpf.h"
#include "LibDegorasSLR/Geophysics/types/geodetic_point.h"
#include "LibDegorasSLR/Geophysics/types/geocentric_point.h"
#include "LibDegorasSLR/Mathematics/containers/matrix.h"
#include "LibDegorasSLR/Mathematics/containers/vector3d.h"
#include "LibDegorasSLR/Geophysics/meteo.h"
#include "LibDegorasSLR/Timing/types/time_types.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace utils{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using dpslr::geo::types::GeocentricPoint;
using dpslr::geo::types::GeodeticPoint;
using dpslr::ilrs::cpf::CPF;
using dpslr::math::Matrix;
using dpslr::math::Vector3D;
using dpslr::geo::meteo::WtrVapPressModel;
using dpslr::timing::MJDate;
using dpslr::timing::SoD;
using dpslr::timing::MJDateTime;
// ---------------------------------------------------------------------------------------------------------------------

/**
 * This struct contains the computed data when applying the PredictionMode::ONLY_INSTANT_RANGE mode. The distance
 * and flight time values may include corrections such as the eccentricity correction at the object and at the
 * ground, the signal delay (station calibration), and the systematic and random observation errors. If the
 * corrections are not included, the corresponding optional parameters will not be accessible in the higher level
 * structure (PredictorSLR::PredictionResult).
 *
 * @warning In this case, the tropospheric correction is never included.
 *
 * @see PredictionMode::ONLY_INSTANT_RANGE
 * @see PredictorSLR::PredictionResult
 */
struct LIBDPSLR_EXPORT InstantRange
{
    /**
     * @brief Default constructor.
     */
    InstantRange() = default;

    /**
     * @brief Default copy constructor.
     */
    InstantRange(const InstantRange&) = default;

    /**
     * @brief Default movement constructor.
     */
    InstantRange(InstantRange&&) = default;

    /**
     * @brief Default copy assingment operator.
     * @return Reference to itself.
     */
    InstantRange& operator=(const InstantRange&) = default;

    /**
     * @brief Default move assignment operator.
     * @return Reference to itself.
     */
    InstantRange& operator=(InstantRange&&) = default;

    // Datetime members.
    MJDate mjd;                  ///< Modified julian date in days.
    SoD sod;               ///< Second of day in that MJD (ps precission -> 12 decimals).
    MJDateTime mjdt;              ///< Modified julian datetime (day & fraction -> 12 decimals).

    // Range (1 way) and time of flight (2 way).
    long double range_1w;          ///< One way range in meters (mm precission -> 3 decimals).
    long double tof_2w;            ///< Two way flight time in seconds (ps precission -> 12 decimals).

    // Associated object geocentric vectors.
    Vector3D<long double> geo_pos; ///< Object geocentric interpolated positions in meters.

    /**
     * @brief Represents the InstantRange struct as a JSON-formatted string.
     * @return The JSON-formatted string representation of InstantRange.
     */
    std::string toJsonStr() const;
};

/**
 * This struct contains the computed data when applying the PredictionMode::INSTANT_VECTOR mode. The distance
 * and flight time values may include all types of corrections. If they are not included, the corresponding
 * optional parameters will not be accessible in the higher level structure (PredictorSLR::PredictionResult).
 *
 * @warning In this case, all the correction could be included.
 *
 * @see PredictionMode::INSTANT_VECTOR
 * @see PredictorSLR::PredictionResult
 */
struct LIBDPSLR_EXPORT InstantData : public InstantRange
{
    /**
     * @brief Default constructor.
     */
    InstantData() = default;

    /**
     * @brief Constructor.
     */
    InstantData(InstantRange&&);

    /**
     * @brief Default copy constructor.
     */
    InstantData(const InstantData&) = default;

    /**
     * @brief Default movement constructor.
     */
    InstantData(InstantData&&) = default;

    /**
     * @brief Default copy assingment operator.
     * @return Reference to itself.
     */
    InstantData& operator=(const InstantData&) = default;

    /**
     * @brief Default move assignment operator.
     * @return Reference to itself.
     */
    InstantData& operator=(InstantData&&) = default;

    // Associated object geocentric vectors.
    Vector3D<long double> geo_vel;   ///< Geocentric interpolated velocity in meters/second.

    // Azimuth and elevation for the instant vector.
    long double az;                       ///< Local computed azimuth in degrees (4 decimals).
    long double el;                       ///< Local computed elevation in degrees (4 decimals).

    /**
     * @brief Represents the InstantData struct as a JSON-formatted string.
     * @return The JSON-formatted string representation of InstantData.
     */
    std::string toJsonStr() const;
};

/**
 * This struct contains the computed data when applying the PredictionMode::OUTBOUND_VECTOR mode. The distance
 * and flight time values may include all types of corrections. If they are not included, the corresponding
 * optional parameters will not be accessible in the higher level structure (PredictorSLR::PredictionResult).
 *
 * @warning In this case, all the correction could be included.
 *
 * @see PredictionMode::OUTBOUND_VECTOR
 * @see PredictorSLR::PredictionResult
 */
struct LIBDPSLR_EXPORT OutboundData : public InstantData
{
    /**
     * @brief Default constructor.
     */
    OutboundData() = default;
};

/**
 * This struct contains the computed data when applying the PredictionMode::INBOUND_VECTOR mode. The distance
 * and flight time values may include all types of corrections. If they are not included, the corresponding
 * optional parameters will not be accessible in the higher level structure (PredictorSLR::PredictionResult).
 *
 * @warning In this case, all the correction could be included.
 *
 * @see PredictionMode::INBOUND_VECTOR
 * @see PredictorSLR::PredictionResult
 */
struct LIBDPSLR_EXPORT InboundData
{
    /**
     * @brief Default constructor.
     */
    InboundData() = default;

    /**
     * @brief Default copy constructor.
     */
    InboundData(const InboundData&) = default;

    /**
     * @brief Default movement constructor.
     */
    InboundData(InboundData&&) = default;

    /**
     * @brief Default copy assingment operator.
     * @return Reference to itself.
     */
    InboundData& operator=(const InboundData&) = default;

    /**
     * @brief Default move assignment operator.
     * @return Reference to itself.
     */
    InboundData& operator=(InboundData&&) = default;

    // Datetime members.
    MJDate mjd;              ///< Modified julian date in days.
    SoD sod;                 ///< Second of day in that MJD (ps precission -> 12 decimals).
    MJDateTime mjdt;         ///< Modified julian datetime (day & fraction -> 12 decimals).

    // Range (1 way) and time of flight (2 way).
    long double range_1w;          ///< One way range in meters (mm precission -> 3 decimals).
    long double tof_2w;            ///< Two way flight time in seconds (ps precission -> 12 decimals).

    /**
     * @brief Represents the InboundData struct as a JSON-formatted string.
     * @return The JSON-formatted string representation of InboundData.
     */
    std::string toJsonStr() const;
};

}} // END NAMESPACES
// =====================================================================================================================