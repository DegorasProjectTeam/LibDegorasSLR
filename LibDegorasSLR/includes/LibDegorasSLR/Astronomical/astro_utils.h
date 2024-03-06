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
 * @file astro_utils.h
 * @brief This file contains the declaration of several astronomical usefull functions.
 * @author Degoras Project Team
 * @copyright EUPL License
 * @version 2306.1
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
//======================================================================================================================

// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
#include "LibDegorasSLR/Mathematics/units/unit_conversions.h"
#include <cmath>
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace astro{
// =====================================================================================================================

//
//======================================================================================================================





/**
 * @brief Convert azimuth and elevation coordinates to right ascension and declination.
 *
 * @param[in] az The azimuth angle in radians.
 * @param[in] el The elevation angle in radians.
 * @param[in] lmst The local mean sidereal time in hours (range: [0, 24)).
 * @param[in] lat The latitude in radians.
 * @param[out] ra The calculated right ascension in the same units as `lmst`.
 * @param[out] dec The calculated declination in radians.
 */
//LIBDPSLR_EXPORT void azElToRaDec(long double az, long double el, long double lmst, long double lat, long double &ra, long double &dec);



//void raDecToAzEl(long double ra, long double dec, long double lmst, long double lat, long double &az, long double &el);



//======================================================================================================================

}} // END NAMESPACES.
// =====================================================================================================================
