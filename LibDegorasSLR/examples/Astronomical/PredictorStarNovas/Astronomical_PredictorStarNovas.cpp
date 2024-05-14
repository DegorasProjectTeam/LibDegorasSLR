/***********************************************************************************************************************
 *   LibDegorasSLR (Degoras Project SLR Library).                                                                      *
 *                                                                                                                     *
 *   ExamplesLibDegorasSLR related project.                                                                            *
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
#include <iostream>
#include <string>
#include <cstdlib>
// =====================================================================================================================

// LIBDEGORASSLR MODULES
// =====================================================================================================================
#include <LibDegorasSLR/Initialization>
#include <LibDegorasSLR/Modules/Helpers>
#include <LibDegorasSLR/Modules/UtilitiesSLR>
#include <LibDegorasSLR/Modules/TrackingMount>
#include <LibDegorasSLR/Modules/FormatsILRS>
#include <LibDegorasSLR/Modules/Timing>
#include <LibDegorasSLR/Modules/Astronomical>
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
// LibDegorasSLR types used in this example.
//
// Initialization.
using dpslr::DegorasInit;
// Time tipes and conversions.
using dpslr::timing::dates::JDateTime;
using dpslr::timing::types::SoD;
using dpslr::timing::types::HRTimePointStd;
using dpslr::timing::dates::J2000DateTime;
using dpslr::timing::dates::MJDate;
using dpslr::timing::iso8601DatetimeToTimePoint;
using dpslr::timing::timePointToModifiedJulianDateTime;
using dpslr::timing::modifiedJulianDateTimeToTimePoint;
// Used units.
using dpslr::math::units::Angle;
using dpslr::math::units::DegreesU;
using dpslr::math::units::Degrees;
using dpslr::math::units::Seconds;
using dpslr::math::units::MillisecondsU;
using dpslr::math::units::Meters;
// Geocentric and geodetic containers.
using dpslr::geo::types::GeocentricPoint;
using dpslr::geo::types::GeodeticPoint;
using dpslr::geo::types::SurfaceLocation;
// Astronomical containers.
using dpslr::astro::types::AltAzPos;
using dpslr::astro::predictors::PredictorStarNovas;
using dpslr::astro::predictors::PredictorStarBase;
using dpslr::astro::predictors::PredictionStarV;
using dpslr::astro::types::Star;
// Helpers.
using dpslr::helpers::strings::numberToStr;
using dpslr::helpers::strings::split;
using dpslr::StringV;
// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// Auxiliar structs and classes.

struct ExampleData
{
    ExampleData(const Star& star):
        star(star)
    {
    }

    // Specific example data.
    Star star;

};

// ---------------------------------------------------------------------------------------------------------------------

int main()
{
    // -------------------- INITIALIZATION -----------------------------------------------------------------------------

    // Initialize LibDegorasSLR.
    DegorasInit::init();

    // -------------------- EXAMPLES CONFIGURATION ---------------------------------------------------------------------

    bool plot_data = true;        // Flag for enable the data plotting using a Python3 (>3.9) helper script.

    // SFEL station geodetic position in degrees (north and east > 0) with 8 decimals (~1 mm precision).
    // Altitude in meters with 3 decimals (~1 mm precision).
    Degrees latitude = 36.46525556L;
    Degrees longitude = 353.79469440L;
    Meters alt = 98.177L;

    // SFEL station geocentric coordinates in meters with 3 decimals (~1 mm precison).
    Meters x = 5105473.885L;
    Meters y = -555110.526L;
    Meters z = 3769892.958L;

    // Configure the input folder.
    std::string current_dir = dpslr::helpers::files::getCurrentDir();
    std::string input_dir(current_dir+"/inputs");
    std::string output_dir(current_dir+"/outputs");

    // Configure the python script executable.
    std::string python_plot_analysis(current_dir+"/python_scripts/Helper_Plotting_Analysis.py");
    std::string python_plot_track(current_dir+"/python_scripts/Helper_Plotting_Track.py");
    std::string python_cmd_analysis = "python \"" + python_plot_analysis + "\" ";

    // Create the ouput directory.
    if (!dpslr::helpers::files::directoryExists(output_dir))
        dpslr::helpers::files::createDirectory(output_dir);


    // -------------------- EXAMPLES PREPARATION -----------------------------------------------------------------------

    // Store the local geocentric and geodetic coordinates.
    GeocentricPoint stat_geoc(x,y,z);
    GeodeticPoint<Degrees> stat_geod(latitude, longitude, alt);
    SurfaceLocation<Degrees> loc;

    loc.geocentric = stat_geoc;
    loc.geodetic = stat_geod;
    loc.meteo.pressure = 1024.1;
    loc.meteo.rel_humidity = 0.75;
    loc.meteo.temperature = 25.8;

    Star vega;
    vega.ra = 18.615648986;
    vega.dec = 38.78368896;
    vega.star_name = "Vega";
    vega.catalog_name = "FK5";
    vega.catalog_num = 699;
    vega.id = 699;
    vega.pm_ra = 0.01726;
    vega.pm_dec = 0.2861;
    vega.rad_vel = -13.9;
    vega.parallax = 0.123;

    Star arcturus;
    arcturus.ra = {14, 15, 39.677};
    arcturus.dec = {19, 10, 56.71};
    arcturus.star_name = "Arcturus";
    arcturus.catalog_name = "FK5";
    arcturus.catalog_num = 526;
    arcturus.id = 526;
    arcturus.pm_ra = -0.07714;
    arcturus.pm_dec = -1.9984;
    arcturus.rad_vel = -5.2;
    arcturus.parallax = 0.09;

    // Real examples vector with their configurations.
    std::vector<ExampleData> examples =
    {
        {vega}, {arcturus}
    };

    // Example selector.
    size_t example_selector = examples.size();  // Select the example to process.
    std::string input;
    while (example_selector >= examples.size() )
    {
        std::cout << "Select example to execute. (0 - " << examples.size() - 1 <<  ")" << std::endl;
        std::getline(std::cin, input);
        try
        {
            example_selector = std::stoul(input);
            if (example_selector >= examples.size())
                std::cout << "Example not found." << std::endl;
        }
        catch(...)
        {
            std::cout << "Bad input" << std::endl;
        }

    }

    // Get band store the example data.
    std::string example_alias = examples[example_selector].star.star_name;
    std::string realtime_csv_filename = example_alias + "_track_realtime.csv";

    // -------------------- PREDICTOR PREPARATION  ---------------------------------------------------------------

    // Prepare the mount slr predictor.
    auto predictor = PredictorStarBase::factory<PredictorStarNovas>(examples[example_selector].star, loc);


    // Log the pass and tracking information (illustrative example). You can read the specific
    // documentation to learn what you can do with each class and struct.
    std::stringstream border, lines, data;
    border.width(80);
    lines.width(80);
    border.fill('=');
    lines.fill('-');
    border << "\n";
    lines << "\n";

    // -------------------- NOW LET'S START CALCULATING PREDICTIONS SIMULATING REAL TIME PROCESS -----------------------

    // Store the real time track data into a CSV file (only part of the data for easy usage).
    // TODO: Store the full track data as JSON.
    // --
    // Create the file and header.
    std::ofstream file_realtime_track(output_dir + "/" + realtime_csv_filename, std::ios_base::out);
    file_realtime_track << data.str();
    file_realtime_track << "jd;sod;pass_az;pass_el;track_az;track_el;sun_az;sun_el";

    // Now, we are simulating real time prediction operations. We can now predict any position within the valid
    // tracking time window (stored in TrackingInfo struct). For the example, we will ask predictions from start to
    // end with a step of 0.1 (simulating real time operations at 10 Hz in the tracking mount).

    // Containers.
    // 31/01/2024 - 03:00
    JDateTime jd(2460340.625L);
    // 31/01/2024 - 05:00
    JDateTime jd_end(2460340.70833L);

    PredictionStarV predictions;

    while (jd < jd_end)
    {
        predictions.push_back(predictor->predict(jd, true));
        jd = jd + 5;
    }

    // Calculate predictions
    //PredictionStarV predictions = predictor->predict(jd, jd_end, 5000);

    // Iterate the real time simulated predictions.
    for(const auto& pred : predictions)
    {
        // Auxiliar container for track data.
        std::string track_az = numberToStr(pred.altaz_coord.az,9, 6);
        std::string track_el = numberToStr(pred.altaz_coord.el,9, 6);

        // Store the data.
        file_realtime_track << '\n';
        file_realtime_track << std::to_string(pred.jdt.datetime()) <<";";
        file_realtime_track << track_az <<";";
        file_realtime_track << track_el;
    }
    // Close the file.
    file_realtime_track.close();

    if(plot_data)
    {
        std::cout<<"Plotting real time simulated data using Python helpers..."<<std::endl;
        if(system(std::string(python_cmd_analysis + output_dir + "/" + realtime_csv_filename).c_str()))
            std::cout<<"Plotting failed!!"<<std::endl;
    }

    // Final wait.
    std::cout << "Example finished. Press Enter to exit..." << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');


    // All ok.
    return (0);
}

// ---------------------------------------------------------------------------------------------------------------------

// =====================================================================================================================
