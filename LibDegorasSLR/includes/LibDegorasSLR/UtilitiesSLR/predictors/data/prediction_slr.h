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
 * @file prediction_slr.h
 * @author Degoras Project Team
 * @brief This file contains the declaration of the struct `PredictionSLR` used by `PredictorSLR` utility.
 * @copyright EUPL License
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
#include "LibDegorasSLR/UtilitiesSLR/predictors/data/prediction_slr_data.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace slr{
namespace predictors{
// =====================================================================================================================

/**
 * @brief This container has all the data
 *
 * This container has all the data returned by the predictor. The InstantRange always will be disponible. The
 * rest of the data will be available or not depending on the selected computing mode. The azimuth and elevation
 * difference between receive and transmit direction at instant time paradpbase::math::units::Meters will only be available in
 * the PredictionMode::OUTBOUND_VECTOR and PredictionMode::INBOUND_VECTOR modes. You can check the corrections
 * applied by accessing the corresponding paradpbase::math::units::Meters.
 *
 */
struct LIBDPSLR_EXPORT PredictionSLR
{
    // Aliases.
    using OptionalPicoseconds = dpbase::Optional<dpbase::math::units::Picoseconds>;
    using OptionalMeters = dpbase::Optional<dpbase::math::units::Meters>;
    using OptionalDegrees = dpbase::Optional<dpbase::math::units::Degrees>;
    using ErrorType = std::uint32_t;

    // Default constructor and destructor, copy and movement constructor and operators.
    M_DEFINE_CTOR_DEF_COPY_MOVE_OP_COPY_MOVE_DTOR(PredictionSLR)

    // SLR result containers.
    InstantRange instant_range;           ///< Result range for the instant time, always available if NOT_ERROR.
    dpbase::Optional<InstantData> instant_data;   ///< Result data for the instant time (instant vectors).
    dpbase::Optional<OutboundData> outbound_data; ///< Result data for the bounce time (outbound vectors).
    dpbase::Optional<InboundData> inbound_data;   ///< Result data for the arrival time (inbound vectors).

    // Difference between receive and transmit direction at instant time.
    OptionalDegrees diff_az;   ///< Azimuth difference between outbound and instant vectors (4 decimals).
    OptionalDegrees diff_el;   ///< Elevation difference between outbound and instant vectors (4 decimals).

    // Corrections applied.
    OptionalPicoseconds cali_del_corr;  ///< Station calibration delay correction (Seconds, 2 way).
    OptionalMeters objc_ecc_corr;       ///< Eccentricity correction at the object (Meters, 1 way, usually CoM).
    OptionalMeters grnd_ecc_corr;       ///< Eccentricity correction at the ground (Meters, usually not used).
    OptionalMeters corr_tropo;          ///< Tropospheric path delay correction (Meters, 1 way).
    OptionalMeters syst_rnd_corr;       ///< Other systematic and random error corrections (Meters, 1 way).

    // Error code.
    ErrorType error;   ///< Error that may have occurred (zero is always NOT_ERROR).

    /**
     * @brief Represents all the prediction result as a JSON formated string.
     */
    std::string toJsonStr() const;
};

/// Alias for PredictionSLR vector.
using PredictionSLRV = std::vector<PredictionSLR>;

}}} // END NAMESPACES
// =====================================================================================================================
