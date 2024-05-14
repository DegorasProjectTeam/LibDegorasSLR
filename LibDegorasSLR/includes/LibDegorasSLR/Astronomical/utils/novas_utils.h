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
 * @file novas_utils.h
 * @brief This file contains the declaration of several astronomical functions from NOVAS software.
 * @author C version: U.S. Naval Observatory - C++ revamp: Degoras Project Team
 * @copyright EUPL License
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
// =====================================================================================================================
// =====================================================================================================================

// LIBNOVASCPP INCLUDES
// =====================================================================================================================
#include <LibNovasCpp/novascpp.h>
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
#include "LibDegorasSLR/Astronomical/types/alt_az_pos.h"
#include "LibDegorasSLR/Astronomical/types/star.h"

#include "LibDegorasSLR/Geophysics/types/geodetic_point.h"
#include "LibDegorasSLR/Geophysics/types/meteo_data.h"
#include "LibDegorasSLR/Geophysics/types/surface_location.h"
#include "LibDegorasSLR/Mathematics/units/strong_units.h"
#include "LibDegorasSLR/Timing/dates/datetime_types.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace astro{
namespace novas{
// =====================================================================================================================

LIBDPSLR_EXPORT int getStarAltAzPos(const astro::types::Star& star,
                                    const geo::types::SurfaceLocation<math::units::Degrees>& loc,
                                    const timing::types::HRTimePointStd& tp,
                                    bool refraction,
                                    types::AltAzPos& pos,
                                    int leap_secs = 0,
                                    double ut1_utc_diff = 0);

LIBDPSLR_EXPORT int getStarAltAzPos(const astro::types::Star& star,
                                    const geo::types::SurfaceLocation<math::units::Degrees>& loc,
                                    const timing::dates::JDateTime& jdt,
                                    bool refraction,
                                    types::AltAzPos& pos,
                                    int leap_secs = 0,
                                    double ut1_utc_diff = 0);

LIBDPSLR_EXPORT ::novas::on_surface makeOnSurface(const geo::types::SurfaceLocation<math::units::Degrees>& loc);

LIBDPSLR_EXPORT ::novas::on_surface makeOnSurface(const geo::types::GeodeticPointDeg& geod,
                                                  const geo::types::MeteoData& meteo);

LIBDPSLR_EXPORT int makeCatEntry(const astro::types::Star& star, ::novas::cat_entry& entry);

}}} // END NAMESPACES.
// =====================================================================================================================
