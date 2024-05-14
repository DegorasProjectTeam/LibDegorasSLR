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
#include <LibNovasCpp/novascpp.h>
// =====================================================================================================================


// LIBRARY INCLUDES
// =====================================================================================================================
#include <LibDegorasSLR/Modules/Testing>
#include <LibDegorasSLR/Modules/Astronomical>
// =====================================================================================================================

// NAMESPACES
// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

// UNIT TEST DECLARATIONS
// ---------------------------------------------------------------------------------------------------------------------
M_DECLARE_UNIT_TEST(novas_make_on_surface)
M_DECLARE_UNIT_TEST(novas_makeOnSurface)
M_DECLARE_UNIT_TEST(novas_getStarAltAzPosition)

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

    dpslr::geo::types::GeodeticPointDeg geoc(latitude, longitude, height);
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

M_DEFINE_UNIT_TEST(novas_getStarAltAzPosition)
{
    using dpslr::astro::types::Star;
    using dpslr::geo::types::MeteoData;
    using dpslr::astro::novas::getStarAltAzPos;
    using Geodetic = dpslr::geo::types::GeodeticPointDeg;
    using Geocentric = dpslr::geo::types::GeocentricPoint;
    using Angle = dpslr::math::units::Degrees;
    using Surface = dpslr::geo::types::SurfaceLocation<Angle>;
    using dpslr::timing::dates::JDateTime;
    using dpslr::astro::types::AltAzPos;

    using novas::julian_date;

    Star star;
    star.star_name = "Vega";
    star.catalog_name = "FK5";
    star.catalog_num = 699;
    star.ra = 18.615648986;
    star.dec = 38.78368896;
    star.pm_ra = 200.94;
    star.pm_dec = 287.78;
    star.parallax = 130.23;
    star.rad_vel = 20.0;

    MeteoData md;
    md.pressure = 1024.1;
    md.temperature = 25.8;

    Geodetic geod(36.465257734376407939L, -6.20530535896L, 98.2496715541929L);
    Geocentric geoc; // Don't mind

    Surface surf{md, geod, geoc};

    JDateTime jdt = julian_date(2023, 10, 18, 22) + (static_cast<double>(15)/1440.0) + (30.5 / 86400.0);
    AltAzPos pos;

    int error = getStarAltAzPos(star, surf, jdt, true, pos, 37, 0.013616);

    if (error)
    {
        M_FORCE_FAIL()
    }
    else
    {
        M_EXPECTED_EQ(static_cast<int>(pos.az), 297)
        M_EXPECTED_EQ(static_cast<int>(pos.el), 32)
    }

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

M_REGISTER_UNIT_TEST(Astronomical-Novas, NovasUtils, novas_getStarAltAzPosition)

// Run unit tests.
M_RUN_UNIT_TESTS()

// Finish the session.
M_FINISH_UNIT_TEST_SESSION()

// ---------------------------------------------------------------------------------------------------------------------

// =====================================================================================================================
