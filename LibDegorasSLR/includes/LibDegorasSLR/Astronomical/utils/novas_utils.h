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
#include "LibDegorasSLR/Astronomical/types/eo_parameters.h"
#include "LibDegorasSLR/Geophysics/types/geodetic_point.h"
#include "LibDegorasSLR/Geophysics/types/meteo_data.h"
#include "LibDegorasSLR/Geophysics/types/surface_location.h"
// =====================================================================================================================

// LIBDPBASE INCLUDES
// =====================================================================================================================
#include "LibDegorasBase/Mathematics/units/strong_units.h"
#include "LibDegorasBase/Timing/dates/datetime_types.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace astro{
namespace novas{
// =====================================================================================================================
/**
 * @brief Calculate a star altazimuth position for given parameters.
 * @param star - The star parameters.
 * @param loc - The location used for calculations.
 * @param eo_params - The Earth Orientation Parameters.
 * @param tp - The time for the calculated position.
 * @param leap_secs - The leap seconds.
 * @param refraction - True if refraction model is to be applied. False otherwise.
 * @param pos - The calculated altazimuth position.
 * @return The novas error code. 0 is successful.
 */
LIBDPSLR_EXPORT int getStarAltAzPos(const astro::types::Star& star,
                                    const geo::types::SurfaceLocation<dpbase::math::units::Degrees>& loc,
                                    const astro::types::EOParameters &eo_params,
                                    const dpbase::timing::types::HRTimePointStd& tp,
                                    int leap_secs,
                                    bool refraction,
                                    types::AltAzPos& pos);

/**
 * @brief Calculate a star altazimuth position for given parameters.
 * @param star - The star parameters.
 * @param loc - The location used for calculations.
 * @param eo_params - The Earth Orientation Parameters.
 * @param tp - The time for the calculated position.
 * @param leap_secs - The leap seconds.
 * @param refraction - True if refraction model is to be applied. False otherwise.
 * @param pos - The calculated altazimuth position.
 * @return The novas error code. 0 is successful.
 */
LIBDPSLR_EXPORT int getStarAltAzPos(const astro::types::Star& star,
                                    const geo::types::SurfaceLocation<dpbase::math::units::Degrees>& loc,
                                    const astro::types::EOParameters &eo_params,
                                    const dpbase::timing::dates::JDateTime& jdt,
                                    int leap_secs,
                                    bool refraction,
                                    types::AltAzPos& pos);

/**
 * @brief Make a novas on_surface object from a SurfaceLocation.
 * @param loc
 * @return The created on_surface object.
 */
LIBDPSLR_EXPORT ::novas::on_surface makeOnSurface(const geo::types::SurfaceLocation<dpbase::math::units::Degrees>& loc);

/**
 * @brief Make a novas on_surface object from a geodetic point and meteo data.
 * @param geod - The geodetic position of the location
 * @param meteo - The meteo data of the location.
 * @return The created on_surface object.
 */
LIBDPSLR_EXPORT ::novas::on_surface makeOnSurface(const geo::types::GeodeticPointDeg& geod,
                                                  const geo::types::MeteoData& meteo);

/**
 * @brief Make a novas cat_entry from a Star object.
 * @param star - The star with the parameters.
 * @param entry - The c reated cat_entry object.
 * @return The novas error code.
 */
LIBDPSLR_EXPORT int makeCatEntry(const astro::types::Star& star, ::novas::cat_entry& entry);

}}} // END NAMESPACES.
// =====================================================================================================================
