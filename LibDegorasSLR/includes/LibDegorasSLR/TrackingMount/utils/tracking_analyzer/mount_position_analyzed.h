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
#include "LibDegorasSLR/TrackingMount/types/mount_position.h"
#include "LibDegorasSLR/Helpers/common_aliases_macros.h"
// =====================================================================================================================

// LIBDEGORASSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace mount{
namespace utils{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using namespace math::units::literals;
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Enumerates the possible status codes for an analyzed tracking position.
 *
 * This enumeration defines the status of a tracking position with respect to the Sun's position and the predictor.
 * It is used to quickly identify the tracking scenario and take appropriate action based on the status. It prioritizes
 * Sun-related statuses (CANT_AVOID_SUN, INSIDE_SUN, AVOIDING_SUN) over ELEVATION_CLIPPED when both conditions are met.
 */
enum class AnalyzedPositionStatus
{
    NO_MODIF_NEEDED,      ///< No modification to the position was needed; all is okay with the original position.
    OUT_OF_TRACK,         ///< The time provided for prediction is outside of tracking.
    CANT_AVOID_SUN,       ///< Final mount position can't be calculated, since it cannot avoid sun security sector.
    INSIDE_SUN,           ///< The final mount position is in the Sun and is configured for not avoiding.
    AVOIDING_SUN,         ///< The final mount position is avoiding sun security sector.
    ELEVATION_CLIPPED,    ///< The final mount position was clipped due to maximum elevation configuration.
};

/**
 * @brief Represents the azimuth and elevation position of a tracking at a specific instant, as well as its status.
 *
 * This structure holds the calculated azimuth and elevation angles for the mount at a specific instant.
 * It also includes the differences between the real predicted position and the
 * track position. The necessity to deviate from the predicted path to avoid direct line-of-sight with the Sun or
 * other obstructions can result in these differences.
 *
 */
struct LIBDPSLR_EXPORT MountPositionAnalyzed : types::MountPosition
{
    // Default constructor, copy and movement constructor and operators.
    M_DEFINE_CTOR_COPY_MOVE_OP_COPY_MOVE_DTOR_DEF(MountPositionAnalyzed)

    MountPositionAnalyzed(const types::MountPosition& mount_pos) :
        types::MountPosition(mount_pos),
        altaz_diff(0.0_deg, 0.0_deg),
        status(AnalyzedPositionStatus::NO_MODIF_NEEDED)
    {}

    // Data members.
    astro::types::AltAzDifference altaz_diff; ///< Difference between analyzed position and the original position.
    AnalyzedPositionStatus status;            ///< The analyzed postion status situation.
};

/// Alias for tracking predictions vector.
using MountPositionAnalyzedV = std::vector<MountPositionAnalyzed>;

}}} // END NAMESPACES
// =====================================================================================================================
