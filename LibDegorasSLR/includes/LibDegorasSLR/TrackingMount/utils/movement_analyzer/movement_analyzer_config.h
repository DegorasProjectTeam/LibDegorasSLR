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
 * @file tracking_analyzer_types.h
 * @author Degoras Project Team
 * @brief
 * @copyright EUPL License
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
// =====================================================================================================================

// LIBDPBASE INCLUDES
// =====================================================================================================================
#include "LibDegorasBase/Mathematics/units/strong_units.h"
#include "LibDegorasBase/Helpers/common_aliases_macros.h"
// =====================================================================================================================

// LIBDEGORASSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace mount{
namespace utils{
// =====================================================================================================================

/**
 * @brief The PredictorMountSLRConfig struct contains the configuration parameters associated with a tracking. These
 * parameters will define the tracking requirements.
 *
 * @todo Max speeds analysis.
 */
struct LIBDPSLR_EXPORT MovementAnalyzerConfig
{
    // Default constructor and destructor, copy and movement constructor and operators.
    M_DEFINE_CTOR_DEF_COPY_MOVE_OP_COPY_MOVE_DTOR(MovementAnalyzerConfig)

    MovementAnalyzerConfig(const dpbase::math::units::DegreesU& sun_avoid_angle, const dpbase::math::units::DegreesU& min_elev,
                           const dpbase::math::units::DegreesU& max_elev, bool sun_avoid) :
        sun_avoid_angle(sun_avoid_angle),
        min_elev(min_elev),
        max_elev(max_elev),
        sun_avoid(sun_avoid)
    {}

    // Data members.
    dpbase::math::units::DegreesU sun_avoid_angle;  ///< Avoid angle for Sun collisions in degrees.
    dpbase::math::units::DegreesU min_elev;         ///< Configured minimum elevation (degrees).
    dpbase::math::units::DegreesU max_elev;         ///< Configured maximum elevation (degrees).
    bool sun_avoid;                         ///< Flag indicating if the track is configured for avoid the Sun.
};

}}} // END NAMESPACES
// =====================================================================================================================
