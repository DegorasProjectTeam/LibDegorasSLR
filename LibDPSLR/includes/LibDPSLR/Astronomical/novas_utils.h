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
#include <cmath>
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include "LibDPSLR/libdpslr_global.h"
#include "LibDPSLR/Astronomical/common/astro_constants.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace astro{
namespace novas{
// =====================================================================================================================

/**
 * \brief Computes the Terrestrial Time (TT) or Terrestrial Dynamical Time (TDT) Julian date corresponding to a
 *        Barycentric Dynamical Time (TDB) Julian date.
 *
 * \param[in] tdb_jd TDB Julian date.
 * \param[out] tt_jd TT Julian date.
 * \param[out] secdiff Difference 'tdb_jd' - 'tt_jd' in seconds.
 *
 * \see Fairhead, L. & Bretagnon, P. (1990) Astron. & Astrophys. 229, 240.
 * \see Kaplan, G. (2005), US Naval Observatory Circular 179.
 *
 * \version V1.0/07-92/TKB (USNO/NRL Optical Interfer.) Translate Fortran.
 * \version V1.1/08-93/WTH (USNO/AA) Update to C Standards.
 * \version V1.2/06-98/JAB (USNO/AA) Adopt new model (Explanatory Supplement to the Astronomical Almanac, pp. 42-44 and p. 316.)
 * \version V1.3/11-03/JAB (USNO/AA) Changed variable names of the input Julian dates to make them more descriptive.
 * \version V1.4/01-07/JAB (USNO/AA) Adopt Fairhead & Bretagnon expression.
 *
 * \warning The expression used in this function is a truncated form of a longer and more precise series given in the
 *          first reference. The result is good to about 10 microseconds.
 */
LIBDPSLR_EXPORT void tdb2tt(long double tdb_jd, long double& tt_jd, long double& secdiff)
{
    long double t = (tdb_jd - cnst::T0) / 36525.0;

    // Expression given in USNO Circular 179, eq. 2.6.
    secdiff = 0.001657 * std::sin(628.3076 * t + 6.2401)
              + 0.000022 * std::sin(575.3385 * t + 4.2970)
              + 0.000014 * std::sin(1256.6152 * t + 6.1969)
              + 0.000005 * std::sin(606.9777 * t + 4.0212)
              + 0.000005 * std::sin(52.9691 * t + 0.4444)
              + 0.000002 * std::sin(21.3299 * t + 5.5431)
              + 0.000010 * t * std::sin(628.3076 * t + 4.2490);

    tt_jd = tdb_jd - secdiff / 86400.0;
}

/**
 * \brief Returns the value of the Earth Rotation Angle (theta) for a given UT1 Julian date.
 *
 * \param[in] jd UT1 Julian date.
 *
 * \returns The Earth Rotation Angle (theta) in degrees.
 *
 * \see IAU Resolution B1.8, adopted at the 2000 IAU General Assembly, Manchester, UK.
 * \see Kaplan, G. (2005), US Naval Observatory Circular 179.
 *
 * \version V1.0/09-03/JAB (USNO/AA)
 *
 * \note This algorithm is equivalent to the canonical theta = 0.7790572732640 + 1.00273781191135448 * t, where t is the
 *       time in days from J2000 (t = jd - T0), but it avoids many two-PI 'wraps' that decrease precision (adopted from
 *       SOFA Fortran routine iau_era00; see also expression at top of page 35 of IERS Conventions (1996)).
 */
LIBDPSLR_EXPORT long double era(long double jd)
{
    long double theta, thet1, thet2, thet3;

    thet1 = 0.7790572732640 + 0.00273781191135448 * (jd - cnst::T0);
    thet2 = 0.00273781191135448 * std::fmod(jd, 1.0);
    thet3 = std::fmod(jd, 1.0);

    theta = std::fmod(thet1 + thet2 + thet3, 1.0) * 360.0;

    if (theta < 0.0)
        theta += 360.0;

    return theta;
}

/**
 * \brief Computes quantities related to the orientation of the Earth's rotation axis at a given TDB Julian date.
 *
 * \param[in] jd_tdb TDB Julian Date.
 * \param[out] mobl Mean obliquity of the ecliptic in degrees at 'jd_tdb'.
 * \param[out] tobl True obliquity of the ecliptic in degrees at 'jd_tdb'.
 * \param[out] ee Equation of the equinoxes in seconds of time at 'jd_tdb'.
 * \param[out] dpsi Nutation in longitude in arcseconds at 'jd_tdb'.
 * \param[out] deps Nutation in obliquity in arcseconds at 'jd_tdb'.
 *
 * \version V1.0/08-93/WTH (USNO/AA) Translate Fortran.
 * \version V1.1/06-97/JAB (USNO/AA) Incorporate IAU (1994) and IERS (1996) adjustment to the "equation of the equinoxes".
 * \version V1.2/10-97/JAB (USNO/AA) Implement function that computes arguments of the nutation series.
 * \version V1.3/07-98/JAB (USNO/AA) Use global variables 'PSI_COR' and 'EPS_COR' to apply celestial pole offsets for high-precision applications.
 * \version V2.0/10-03/JAB (USNO/AA) Update function for IAU 2000 resolutions.
 * \version V2.1/12-04/JAB (USNO/AA) Add 'mode' argument.
 * \version V2.2/01-06/WKP (USNO/AA) Changed 'mode' to 'accuracy'.
 * \version VDPSLR-1 (ROA) Fixed the accuracy mode to reduced. Removed the PSI_COR and EPS_COR variables usage. Removed
 *          all the logic related to the full accuracy mode.
 *
 * \warning Values of the celestial pole offsets not will be used.
 * \warning The reimplemented method always use the NOVAS reduced accuracy mode (0.1 milliarcsecond preccision).
 * \warning The reimplemented method always computes the nutation angles (arcseconds).
 */
/*
void e_tilt(long double jd_tdb, double& mobl, double& tobl, double& ee, double& dpsi, double& deps)
{
    static double jd_last = 0.0;
    static double dp, de, c_terms;
    double t, d_psi, d_eps, mean_ob, true_ob, eq_eq;

    // Compute time in Julian centuries from epoch J2000.0.
    t = (jd_tdb - cnst::T0) / 36525.0;

    nutation_angles(t, 1, &dp, &de); // Use reduced accuracy (1) always.

    // Obtain complementary terms for the equation of the equinoxes in arcseconds.
    c_terms = ee_ct(jd_tdb, 0.0, 1) / cnst::ASEC2RAD; // Use reduced accuracy (1) always.


    // Apply observed celestial pole offsets.
    d_psi = dp + cnst::PSI_COR;
    d_eps = de + cnst::EPS_COR;

    // Compute mean obliquity of the ecliptic in arcseconds.
    mean_ob = mean_obliq(jd_tdb);

    // Compute true obliquity of the ecliptic in arcseconds.
    true_ob = mean_ob + d_eps;

    // Convert obliquity values to degrees.
    mean_ob /= 3600.0;
    true_ob /= 3600.0;

    // Compute equation of the equinoxes in seconds of time.
    eq_eq = d_psi * std::cos(mean_ob * cnst::DEG2RAD) + c_terms;
    eq_eq /= 15.0;

    // Set output values.
    mobl = mean_ob;
    tobl = true_ob;
    ee = eq_eq;
    dpsi = d_psi;
    deps = d_eps;
}
*/


}}} // END NAMESPACES.
// =====================================================================================================================