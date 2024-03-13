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
using dpslr::timing::types::MJDateTime;
using dpslr::timing::types::SoD;
using dpslr::timing::types::HRTimePointStd;
using dpslr::timing::types::J2000DateTime;
using dpslr::timing::types::MJDate;
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
using dpslr::astro::AltAzPos;
using dpslr::astro::PredictorStarNovas;
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

    Star vega;
    vega.ra = 18.615648986;
    vega.dec = 38.78368896;
    vega.star_name = "Vega";
    vega.catalog_name = "FK5";
    vega.catalog_num = 699;
    vega.id = 699;
    vega.pm_ra = 200.94;
    vega.pm_dec = 287.78;
    vega.rad_vel = 130.23;
    vega.parallax = 20.0;

    // Real examples vector with their configurations.
    std::vector<ExampleData> examples =
    {
        // Vega
        {vega}

    };

    // Example selector.
    size_t example_selector = examples.size();  // Select the example to process (between reals and sintetics).
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
    PredictorStarNovas predictor_star(examples[example_selector].star, loc);

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
    file_realtime_track << "mjd;sod;pass_az;pass_el;track_az;track_el;sun_az;sun_el";

    // Now, we are simulating real time prediction operations. We can now predict any position within the valid
    // tracking time window (stored in TrackingInfo struct). For the example, we will ask predictions from start to
    // end with a step of 0.1 (simulating real time operations at 10 Hz in the tracking mount).

    // Containers.
    /*
    JDateTime jd = track_start;


    // Iterate the real time simulated predictions.
    for(const auto& pred : results)
    {
        // Auxiliar container for track data.
        std::string track_az;
        std::string track_el;
        std::string orig_az;
        std::string orig_el;
        //
        // At this point, you only must check if the prediction is outside track. This is becaouse, for example,
        // the beginning of the real satellite pass may coincide with the Sun sector, so at those points there
        // would be no data from the mount's track, only the real pass.
        if(pred.status != PositionStatus::OUT_OF_TRACK)
        {
            track_az = numberToStr(pred.mount_pos->altaz_coord.az,7, 4);
            track_el = numberToStr(pred.mount_pos->altaz_coord.el,7, 4);
            orig_az = numberToStr(pred.mount_pos->altaz_coord.az - pred.mount_pos->diff_az, 7, 4);
            orig_el = numberToStr(pred.mount_pos->altaz_coord.el - pred.mount_pos->diff_el, 7, 4);

            // Store the data.
            file_realtime_track <<'\n';
            file_realtime_track << std::to_string(timePointToModifiedJulianDateTime(pred.tp).datetime()) <<";";
            file_realtime_track << orig_az <<";";
            file_realtime_track << orig_el <<";";
            file_realtime_track << track_az <<";";
            file_realtime_track << track_el <<";";
            file_realtime_track << numberToStr(pred.sun_pred->altaz_coord.az, 7, 4) <<";";
            file_realtime_track << numberToStr(pred.sun_pred->altaz_coord.el, 7, 4);
        }
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
*/
    // All ok.
    return (0);
}

// ---------------------------------------------------------------------------------------------------------------------

// =====================================================================================================================
