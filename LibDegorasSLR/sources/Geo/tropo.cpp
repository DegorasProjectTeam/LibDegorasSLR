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
 * @file tropo.cpp
 * @author Degoras Project Team.
 * @brief This file contains the implementation of functions related with the troposphere.
 * @copyright EUPL License
 * @version 2307.1
***********************************************************************************************************************/

// C++ INCLUDES
//======================================================================================================================
#include <cmath>
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Geophysics/tropo.h"
#include "LibDegorasSLR/Mathematics/units/unit_conversions.h"
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace geo{
namespace tropo{
// =====================================================================================================================

long double pathDelayMariniMurray(long double pres, long double temp, long double rh, long double el, long double wl,
                                  long double phi, long double ht, meteo::WtrVapPressModel wvpm)
{
    // Calculate the water vapor pressure.
    long double e0 = meteo::waterVaporPressure(rh, temp, pres, wvpm);
    // Calculate A, B and K.
    long double a = 0.2357e-2L * pres + 0.141e-3L * e0;
    long double k = 1.163L - 0.968e-2L * std::cos(2.0L * phi) - 0.104e-2L * temp + 0.1435e-4L * pres;
    long double b = 1.084e-8L * pres * temp * k + 4.734e-8L * (2.0L * std::pow(pres,2))/(temp * (3.0L - 1.0L/k));
    // Calculate the laser frequency parameter and the laser site function value.
    long double flam = 0.9650L + 0.0164L * std::pow(wl, -2) + 0.228e-3L * std::pow(wl, -4);
    long double fphih = 1.0L - 0.26e-2L * std::cos(2.0L * phi) - 0.31e-6L * ht;
    // Calculate the range correction.
    long double sine = std::sin(el);
    long double ab = a + b;
    long double ar = (flam / fphih) * (ab / (sine + (b / ab) / (sine + 0.01L)));
    // Return one way the tropospheric path delay (meters).
    return ar;
}

}}} // END NAMESPACES.
// =====================================================================================================================
