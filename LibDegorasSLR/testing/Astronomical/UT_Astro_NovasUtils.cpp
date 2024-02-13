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

// C++ INCLUDES
// =====================================================================================================================
// =====================================================================================================================

// LIBNOVASCPP INCLUDES
// =====================================================================================================================
//#include <LibNovasCpp/novascpp.h>
// =====================================================================================================================


// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Geophysics/types/geodetic_point.h"
#include <LibDegorasSLR/Testing/UnitTest>
#include "LibDegorasSLR/Astronomical/novas_utils.h"

// =====================================================================================================================

// NAMESPACES
// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

// UNIT TEST DECLARATIONS
// ---------------------------------------------------------------------------------------------------------------------
M_DECLARE_UNIT_TEST(novas_make_on_surface)
M_DECLARE_UNIT_TEST(novas_makeOnSurface)

// ---------------------------------------------------------------------------------------------------------------------

// UNIT TESTS IMPLEMENTATIONS
// ---------------------------------------------------------------------------------------------------------------------

M_DEFINE_UNIT_TEST(novas_make_on_surface)
{
    // Parameters.
    const double latitude = 36.4652577343764;
    const double longitude = -6.20530535896;
    const double height = 98.2496715541929;
    const double temperature = 25.8;
    const double pressure = 1024.1;
    // Geo location.
    novas::on_surface geo_loc;
    // Make on surface.
    novas::make_on_surface(latitude,longitude,height,temperature,pressure, &geo_loc);
    // Checks.
    M_EXPECTED_EQ(latitude, geo_loc.latitude)
    M_EXPECTED_EQ(longitude, geo_loc.longitude)
    M_EXPECTED_EQ(height, geo_loc.height)
    M_EXPECTED_EQ(temperature, geo_loc.temperature)
    M_EXPECTED_EQ(pressure, geo_loc.pressure)
}

M_DEFINE_UNIT_TEST(novas_makeOnSurface)
{
    // Parameters.
    const double latitude = 36.4652577343764;
    const double longitude = -6.20530535896;
    const double height = 98.2496715541929;
    const double temperature = 25.8;
    const double pressure = 1024.1;

    dpslr::geo::types::GeodeticPoint<double> geoc(latitude, longitude, height);
    dpslr::geo::types::MeteoData meteo(temperature, pressure, 0);

    // Geo location.
    novas::on_surface geo_loc = dpslr::astro::novas::makeOnSurface(geoc, meteo);

    // Checks.
    M_EXPECTED_EQ(latitude, geo_loc.latitude)
    M_EXPECTED_EQ(longitude, geo_loc.longitude)
    M_EXPECTED_EQ(height, geo_loc.height)
    M_EXPECTED_EQ(temperature, geo_loc.temperature)
    M_EXPECTED_EQ(pressure, geo_loc.pressure)
}

// ---------------------------------------------------------------------------------------------------------------------

// UNIT TESTS EXECUTION
// ---------------------------------------------------------------------------------------------------------------------

// Start the Unit Test Session.
M_START_UNIT_TEST_SESSION("LibDegorasSLR Novas Unit Tests")

// Configuration.
M_FORCE_SHOW_RESULTS(true)

// Register the tests.
M_REGISTER_UNIT_TEST(Astronomical-Novas, LibNovasCPP, novas_make_on_surface)

M_REGISTER_UNIT_TEST(Astronomical-Novas, NovasUtils, novas_makeOnSurface)

// Run unit tests.
M_RUN_UNIT_TESTS()

// Finish the session.
M_FINISH_UNIT_TEST_SESSION()

// ---------------------------------------------------------------------------------------------------------------------

// =====================================================================================================================