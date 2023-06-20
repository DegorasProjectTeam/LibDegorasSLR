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
 * @version 2305.1
***********************************************************************************************************************/

// C++ INCLUDES
//======================================================================================================================
#include <cmath>
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include <LibDPSLR/Geo/tropo.h>
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace geo{
namespace tropo{
// =====================================================================================================================

double pathDelayMariniMurray(double pres, double temp, double rh, double el, double wl, double phi,
                             double ht, meteo::WtrVapPressModel wvpm)
{
    // Calculate the water vapor pressure.
    double e0 = meteo::waterVaporPressure(rh, temp, pres, wvpm);
    // Calculate A, B and K.
    double a = 0.2357e-2 * pres + 0.141e-3 * e0;
    double k = 1.163 - 0.968e-2 * std::cos(2.0 * phi) - 0.104e-2 * temp + 0.1435e-4 * pres;
    double b = 1.084e-8 * pres * temp * k + 4.734e-8 * (2.0 * std::pow(pres,2))/(temp * (3.0 - 1.0/k));
    // Calculate the laser frequency parameter and the laser site function value.
    double flam = 0.9650 + 0.0164 * std::pow(wl, -2) + 0.228e-3 * std::pow(wl, -4);
    double fphih = 1.0 - 0.26e-2 * std::cos(2.0 * phi) - 0.31e-6 * ht;
    // Calculate the range correction.
    double sine = std::sin(el);
    double ab = a + b;
    double ar = (flam / fphih) * (ab / (sine + (b / ab) / (sine + 0.01)));
    // Return one way the tropospheric path delay (meters).
    return ar;
}

}}} // END NAMESPACES.
// =====================================================================================================================
