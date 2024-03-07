/***********************************************************************************************************************
 *   LibDPSLR (Degoras Project SLR Library): A libre base library for SLR related developments.                        *                                      *
 *                                                                                                                     *
 *   Copyright (C) 2023 Degoras Project Team                                                                           *
 *                      < Ángel Vera Herrera, avera@roa.es - angeldelaveracruz@gmail.com >                             *
 *                      < Jesús Relinque Madroñal >                                                                    *
 *                                                                                                                     *
 *   This file is part of LibDPSLR.                                                                                    *
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
 * @version 2307.1
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
//======================================================================================================================
// =====================================================================================================================

// LIBNOVASCPP INCLUDES
//======================================================================================================================
#include <LibNovasCpp/novascpp.h>
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
#include "LibDegorasSLR/Astronomical/types/astro_types.h"
#include "LibDegorasSLR/Astronomical/types/star.h"
#include "LibDegorasSLR/Geophysics/types/geodetic_point.h"
#include "LibDegorasSLR/Geophysics/types/meteo_data.h"
#include "LibDegorasSLR/Geophysics/types/surface_location.h"
#include "LibDegorasSLR/Timing/types/base_time_types.h"
// =====================================================================================================================

// NAMESPACES
// =====================================================================================================================
using novas::make_on_surface;
using novas::make_cat_entry;
using novas::on_surface;
using novas::cat_entry;
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace astro{
namespace novas{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using math::units::Degrees;
// ---------------------------------------------------------------------------------------------------------------------


LIBDPSLR_EXPORT int getStarAltAzPos(const astro::types::Star &star,
                                    const geo::types::SurfaceLocation<Degrees> &loc,
                                    const timing::types::HRTimePointStd &tp,
                                    types::AltAzPos &pos,
                                    int leap_secs = 0,
                                    double ut1_utc_diff = 0);

LIBDPSLR_EXPORT int getStarAltAzPos(const astro::types::Star &star,
                                    const geo::types::SurfaceLocation<Degrees> &loc,
                                    const timing::types::JDateTime &jdt,
                                    types::AltAzPos &pos,
                                    int leap_secs = 0,
                                    double ut1_utc_diff = 0);

LIBDPSLR_EXPORT on_surface makeOnSurface(const geo::types::SurfaceLocation<Degrees> &loc);

LIBDPSLR_EXPORT on_surface makeOnSurface(const geo::types::GeodeticPoint<Degrees>& geod,
                                         const geo::types::MeteoData& meteo);

LIBDPSLR_EXPORT int makeCatEntry(const astro::types::Star &star, cat_entry &entry);

// TODO make cat entry de novas recibiendo una estructura "start" de libdegorasslr (TODO)

// Wrapper equ2hor utilizando nuestros tiempos (internamente se utilizara jd_ut1).


}}} // END NAMESPACES.
// =====================================================================================================================
