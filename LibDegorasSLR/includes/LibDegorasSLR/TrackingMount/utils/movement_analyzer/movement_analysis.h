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
 * @file tracking_analysis.h
 * @author Degoras Project Team
 * @brief
 * @copyright EUPL License
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Helpers/common_aliases_macros.h"
#include "LibDegorasSLR/Timing/dates/datetime_types.h"
#include "LibDegorasSLR/Astronomical/types/alt_az_pos.h"
#include "LibDegorasSLR/Astronomical/types/local_sun_position.h"
#include "LibDegorasSLR/TrackingMount/utils/movement_analyzer/mount_position_analyzed.h"
#include "LibDegorasSLR/TrackingMount/utils/movement_analyzer/sun_collision_sector.h"
// =====================================================================================================================

// LIBDEGORASSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace mount{
namespace utils{
// =====================================================================================================================

/**
 * @brief The TrackingInfo struct contains the information obtained from the tracking analysis.
 */
struct LIBDPSLR_EXPORT MovementAnalysis
{
    // Copy and movement constructor and operators, and default destructor.
    M_DEFINE_CTOR_COPY_MOVE_OP_COPY_MOVE_DTOR_DEF(MovementAnalysis)

    MovementAnalysis() :
        empty_movement(false),
        sun_deviation(false),
        sun_collision(false),
        sun_collision_high_el(false),
        sun_collision_at_middle(false),
        sun_collision_at_start(false),
        sun_collision_at_end(false),
        trim_at_start(false),
        trim_at_end(false),
        el_deviation(false)
    {}

    // Time data.
    timing::dates::MJDateTime mjdt_start;     ///< Movement start Modified Julian Datetime.
    timing::dates::MJDateTime mjdt_end;       ///< Movement end Modified Julian Datetime.

    // Position data.
    astro::types::AltAzPos start_coord;       ///< Movement start altazimuth coordinates.
    astro::types::AltAzPos end_coord;         ///< Movement end altazimuth  coordinates.
    math::units::Degrees max_el;              ///< Movement maximum elevation in degrees.
    // TODO Track Max speed.

    // Data containers.
    SunCollisionSectorV sun_sectors;                ///< Data for sun collision sectors.
    types::MountPositionV original_positions;       ///< Original absolute mount positions.
    MountPositionAnalyzedV analyzed_positions;      ///< Analyzed absolute final mount positions.
    astro::types::LocalSunPositionV sun_positions;  ///< Sun altazimuth positions.

    // Iterators with the start and end of the movement.
    MountPositionAnalyzedV::iterator start_mov_it; ///< Const iterator to the real valid start of the movement.
    MountPositionAnalyzedV::iterator end_mov_it;   ///< Const iterator to the real valid end of the movement.

    // Validation flag.
    bool empty_movement;          ///< Flag inficating if the movement is empty (due to analysis checks).

    // Movement alterations.
    bool sun_deviation;           ///< Flag indicating if the movement was deviated from pass due to Sun.
    bool sun_collision;           ///< Flag indicating if the movement has a collision with the Sun.
    bool sun_collision_high_el;   ///< Flag indicating if the movement has a collision with the Sun at high elevation.
    bool sun_collision_at_middle; ///< Flag indicating if the movement has a collision at middle with the Sun.
    bool sun_collision_at_start;  ///< Flag indicating if the movement has a collision at start with the Sun.
    bool sun_collision_at_end;    ///< Flag indicating if the movement has a collision at end with the Sun.
    bool sun_collision_high;      ///< Flag indicating if the movement has a collision with a high sun sector.
    bool trim_at_start;           ///< Flag indicating if the movement was trimmed due to elevation or Sun at start.
    bool trim_at_end;             ///< Flag indicating if the movement was trimmed due to elevation or Sun at end.
    bool el_deviation;            ///< Flag indicating if the movement was deviated from pass due to max elevation.
};

}}} // END NAMESPACES
// =====================================================================================================================
