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
 * @file meteo.cpp
 * @author Degoras Project Team.
 * @brief This file contains the implementation of functions related with meteorological parameters.
 * @copyright EUPL License
 * @version 2305.1
***********************************************************************************************************************/

// C++ INCLUDES
//======================================================================================================================
#include <cmath>
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include <LibDegorasSLR/Geophysics/meteo.h>
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace geo{
namespace meteo{
// =====================================================================================================================

double waterVaporPressure(double rh, double temp, double pres, WtrVapPressModel mode)
{
    // Water vapor pressure variable.
    double e0 = 0;

    // Calculate the water vapor pressute using the original Marini and Murray formula.
    if(mode == WtrVapPressModel::ORIGINAL_MM)
        e0 = rh * 6.11e-2 * std::pow(10.0, ((7.5 * (temp - 273.15)) / (237.3 + (temp - 273.15))));

    // Calculate the water vapor pressure using Giacomo and Davis formula.
    if(mode == WtrVapPressModel::GIACOMO_DAVIS)
    {
        // Calculate the saturation vapor pressure in mbar (Giacomo 1982 and Davis 1992).
        double es = 0.01 * std::exp(
                    1.2378847e-5*std::pow(temp,2) - 1.9121316e-2*temp + 33.93711047 - 6.3431645e3*std::pow(temp,-1));
        // Calculate the enhancement factor (Giacomo 1982)
        double fw = 1.00062 + 3.14e-6 * pres + 5.6e-7 * std::pow(temp - 273.15, 2);
        // Finally, calculate the water vapor pressure from a relative humidity measurement (%).
        e0 = rh * 0.01 * fw * es;
    }

    // Return the water vapor pressure.
    return e0;
}

}}} // END NAMESPACES.
// =====================================================================================================================
