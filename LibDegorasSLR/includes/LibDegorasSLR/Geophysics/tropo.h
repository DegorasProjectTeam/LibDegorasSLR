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
 * @file tropo.h
 * @author Degoras Project Team.
 * @brief This file contains the declaration of functions related with the troposphere.
 * @copyright EUPL License
 * @version 2307.1
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
#include "LibDegorasSLR/Geophysics/meteo.h"
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace geo{
namespace tropo{
// =====================================================================================================================

/**
 * @brief Calculates the tropospheric path delay (one way) using Marini and Murray model (1973).
 *
 * Calculate and return the tropospheric path delay (one way) produced by the troposhere for the laser beam path from a
 * ground station to a space object using the Marini and Murray model (1973). This model uses the pressure, temperature,
 * humidity, satellite elevation, station latitude, station height and laser wavelength. Commonly used in SLR.
 *
 * @param pres Atmospheric pressure (mbar).
 * @param temp Surface tempreature (Kelvin).
 * @param rh   Relative humidity (%, eg. 50%).
 * @param el   Elevation of the target (radians).
 * @param wl   Beam wavelength (micrometres).
 * @param phi  Latitude of the station (radians).
 * @param ht   Height of the station (meters above sea level).
 * @param wvpm Water vapor pressure model. See WtrVapPressModel for more details.
 * @return One way tropospheric path delay (meters).
 */
LIBDPSLR_EXPORT long double pathDelayMariniMurray(long double pres, long double temp, long double rh,
                                                  long double el, long double wl, long double phi,
                                             long double ht, meteo::WtrVapPressModel wvpm);

}}} // END NAMESPACES
// =====================================================================================================================
