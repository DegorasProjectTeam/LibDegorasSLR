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
 * @file meteo.h
 * @author Degoras Project Team.
 * @brief This file contains the declaration of functions related with meteorological parameters.
 * @copyright EUPL License
 2305.1
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
// =====================================================================================================================
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include <vector>

#include <LibDegorasBase/Mathematics/math_constants.h>
#include "LibDegorasSLR/libdegorasslr_global.h"

using dpbase::math::kPi;
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace geo{
namespace meteo{
// =====================================================================================================================

// ENUMERATIONS
// =====================================================================================================================

/**
 * @enum WtrVapPressModel
 * @brief Represents the different models that can be used to calculate the water vapor pressure.
 * @see IERS TN 21, chap. 9. https://www.iers.org/IERS/EN/Publications/TechnicalNotes/tn21.html?nn=94912
 * @see IERS TN 32, chap. 9. https://www.iers.org/IERS/EN/Publications/TechnicalNotes/tn32.html?nn=94912
 */
enum class WtrVapPressModel
{
    ORIGINAL_MM,         ///< Original formula. IERS TN 21, chap. 9.
    GIACOMO_DAVIS        ///< Giacomo and Davis formula. IERS TN 32, chap. 9.
};

// =====================================================================================================================

// FUNCTIONS
// =====================================================================================================================

/**
 * @brief Get the water vapor pressure using diferents models.
 *
 * This function calculates the water vapor pressure at the station site using differents models. You can use the
 * original formula used in Marini and Murray model (1973) (IERS TN 21, chap. 9) or the Giacomo and Davis model
 * (IERS TN 32, chap. 9).
 *
 * @param rh   Relative humidity at the laser site (percent eg, 50%).
 * @param temp Athmospheric temperature at the laser site (Kelvin).
 * @param pres Atmospheric pressure at the laser site (millibars).
 * @param wvpm Water vapor pressure model. See WtrVapPressModel for more details.
 * @return The water vapor pressure at the laser site (millibars).
 *
 * @see IERS TN 21, chap. 9. https://www.iers.org/IERS/EN/Publications/TechnicalNotes/tn21.html?nn=94912
 * @see IERS TN 32, chap. 9. https://www.iers.org/IERS/EN/Publications/TechnicalNotes/tn32.html?nn=94912
 * @see Giacomo, P., Equation for the dertermination of the density of moist air, Metrologia, V. 18, 1982
 */
 LIBDPSLR_EXPORT long double waterVaporPressure(long double rh, long double temp, long double pres, WtrVapPressModel mode);

/**
 * @brief Calculate the refraction delay correction using the Marini-Murray model
 *
 * @param pres Atmospheric pressure at the laser site (millibars).
 * @param temp Athmospheric temperature at the laser site (Kelvin).
 * @param hum Athmospheric humidity.
 * @param alt The elevation of station.
 * @param rlam The Wavelength.
 * @param phi The latitude of observer.
 * @param hm The heigth of observer.
*/
template <typename T>
std::vector<T> refMM(const std::vector<T>& pres, const std::vector<T>& temp, const std::vector<T>& hum, const std::vector<T>& alt, const T& rlam, const T& phi, const T& hm)
{
    T flam = 0.9650 + 0.0164 / (rlam * rlam) + 0.228e-3 / std::pow(rlam,4);
    T fphih = 1.0 - 0.26e-2 * std::cos(2.0 * phi) - 0.3 * hm;

    if(pres.size() != hum.size() || hum.size() != temp.size() || temp.size() != alt.size())
    {
        return {};
    }
    std::size_t n = hum.size();
    T tzc;
    T ez;
    T rk;
    T a;
    T b;
    T sine;
    T delr;
    std::vector<T> tref(n);

    for(std::size_t i = 0; i < n; i++)
    {
        tzc = temp[i] - 273.15;
        ez = hum[i] * 6.11e-2 * std::pow(10.0,((7.5 * tzc) / (237.3 + tzc)));
        rk = 1.163 - 0.968e-2 * std::cos(2.0 * phi) - 0.104e-2 * temp[i] + 0.1435e-4 * pres[i];
        a = 0.2357e-2 * pres[i] + 0.141e-3 * ez;
        b = 1.084e-8 * pres[i] * temp[i] * rk + (4.734e-8 * 2.0 * pres[i] * pres[i]) / (temp[i] * (3.0 - 1.0 / rk));
        sine = std::sin(alt[i] * 2.0 * kPi / 360.0);
        delr = (flam / fphih) * ((a+b) / (sine + (b / (a+b+1e-5)) / (sine + 0.01)));
        tref[i] = delr * 2.0;
    }

    return tref;
}

}}} // END NAMESPACES.
// =====================================================================================================================
