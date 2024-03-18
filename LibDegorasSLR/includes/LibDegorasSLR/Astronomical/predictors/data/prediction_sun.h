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
 * @file prediction_sun.h
 * @brief
 * @author Degoras Project Team
 * @copyright EUPL License
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
//======================================================================================================================
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
#include "LibDegorasSLR/Geophysics/types/geocentric_point.h"
#include "LibDegorasSLR/Astronomical/types/astro_types.h"
#include "LibDegorasSLR/Timing/types/datetime_types.h"
#include "LibDegorasSLR/Helpers/common_aliases_macros.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace astro{
namespace predictors{
// =====================================================================================================================

struct LIBDPSLR_EXPORT PredictionSun : types::SunPosition
{
    // Default constructor and destructor, copy and movement constructor and operators.
    M_DEFINE_CTOR_DEF_COPY_MOVE_OP_COPY_MOVE_DTOR_DEF(PredictionSun)

    PredictionSun(const timing::types::J2000DateTime& j2000,
                  const astro::types::AltAzPos& altaz_coord,
                  const geo::types::GeocentricPoint& geo_pos) :
        types::SunPosition(j2000, altaz_coord),
        geo_pos(geo_pos)
    {}

    // Containers.
    geo::types::GeocentricPoint geo_pos; ///< Sun predicted geocentric position (meters).

    // TODO Calculate the geocentric position, neccesary to check non visible moments in space object passes.
};

/// Alias for a vector of PredictionSun.
using PredictionSunV = std::vector<PredictionSun>;

}}} // END NAMESPACES.
// =====================================================================================================================
