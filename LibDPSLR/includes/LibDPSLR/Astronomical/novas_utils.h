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



}}} // END NAMESPACES.
// =====================================================================================================================
