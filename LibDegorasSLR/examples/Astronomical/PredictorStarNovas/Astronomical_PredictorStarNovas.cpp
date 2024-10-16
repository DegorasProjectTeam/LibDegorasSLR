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
#include <LibDegorasSLR/Modules/UtilitiesSLR>
#include <LibDegorasSLR/Modules/TrackingMount>
#include <LibDegorasSLR/Modules/FormatsILRS>
#include <LibDegorasSLR/Modules/Astronomical>
// =====================================================================================================================

// LIBDPBASE INCLUDES
// =====================================================================================================================
#include <LibDegorasBase/Modules/Timing>
#include <LibDegorasBase/Modules/Helpers>
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
// LibDegorasSLR types used in this example.
//
// Initialization.
using dpslr::DegorasInit;
// Time tipes and conversions.
using dpbase::timing::dates::JDateTime;
using dpbase::timing::types::SoD;
using dpbase::timing::types::HRTimePointStd;
using dpbase::timing::dates::J2000DateTime;
using dpbase::timing::dates::MJDate;
using dpbase::timing::iso8601DatetimeToTimePoint;
using dpbase::timing::timePointToModifiedJulianDateTime;
using dpbase::timing::modifiedJulianDateTimeToTimePoint;
// Used units.
using dpbase::math::units::Angle;
using dpbase::math::units::DegreesU;
using dpbase::math::units::Degrees;
using dpbase::math::units::Seconds;
using dpbase::math::units::MillisecondsU;
using dpbase::math::units::Meters;
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
using dpbase::helpers::strings::numberToStr;
using dpbase::helpers::strings::split;
using dpbase::StringV;
// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// Auxiliar structs and classes.

struct ExampleData
{
    ExampleData(const Star& star, const std::string &date, const Seconds& duration):
        star(star), datetime_iso8601(date), duration_tracking(duration)
    {
    }

    // Specific example data.
    Star star;
    std::string datetime_iso8601;
    Seconds duration_tracking;

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
    Meters x = 5105473.922L;
    Meters y = -555110.640L;
    Meters z = 3769892.747L;

    // Configure the input folder.
    std::string current_dir = dpbase::helpers::files::getCurrentDir();
    std::string input_dir(current_dir+"/inputs");
    std::string output_dir(current_dir+"/outputs");

    // Configure the python script executable.
    std::string python_plot_analysis(current_dir+"/python_scripts/Helper_Plotting_Analysis.py");
    std::string python_plot_track(current_dir+"/python_scripts/Helper_Plotting_Track.py");
    std::string python_cmd_analysis = "python \"" + python_plot_analysis + "\" ";

    // Create the ouput directory.
    if (!dpbase::helpers::files::directoryExists(output_dir))
        dpbase::helpers::files::createDirectory(output_dir);


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
    vega.degoras_id = 699;
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
    arcturus.degoras_id = 526;
    arcturus.pm_ra = -0.07714;
    arcturus.pm_dec = -1.9984;
    arcturus.rad_vel = -5.2;
    arcturus.parallax = 0.09;

    Star polaris;
    polaris.ra = {2, 31, 48.704};
    polaris.dec = {89, 15, 50.72};
    polaris.star_name = "Polaris";
    polaris.catalog_name = "FK5";
    polaris.catalog_num = 907;
    polaris.degoras_id = 907;
    polaris.pm_ra = 0.19877;
    polaris.pm_dec =  -0.0152;
    polaris.rad_vel = 0.003;
    polaris.parallax = -17.4;

    Star sirius;
    sirius.ra = 6.752464;
    sirius.dec = -16.7161083;
    sirius.star_name = "Sirius";
    sirius.catalog_name = "FK5";
    sirius.catalog_num = 257;
    sirius.degoras_id = 257;
    sirius.pm_ra = -0.03847;
    sirius.pm_dec = -1.2053;
    sirius.rad_vel = -7.6;
    sirius.parallax = 0.375;


    // Real examples vector with their configurations.
    std::vector<ExampleData> examples =
    {
        {vega, "2023-10-19T21:15:30.000Z", 600},
        {arcturus, "2023-10-23T08:25:30.000Z", 600},
        {sirius, "2023-10-23T08:25:30.000Z", 600}
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

    // Datetime configuration.
    std::chrono::seconds obs_secs(examples[example_selector].duration_tracking);

    dpbase::timing::types::HRTimePointStd tp_start = dpbase::timing::iso8601DatetimeToTimePoint(
        examples[example_selector].datetime_iso8601);
    dpbase::timing::types::HRTimePointStd tp_end = tp_start + obs_secs;

    JDateTime jd_start = dpbase::timing::timePointToJulianDateTime(tp_start);
    JDateTime jd_end = dpbase::timing::timePointToJulianDateTime(tp_end);

    // Get band store the example data.
    std::string example_alias = examples[example_selector].star.star_name;
    std::string realtime_csv_filename = example_alias + "_track_realtime.csv";
    std::string rt_interval_csv_filename = example_alias + "_track_rt_interval.csv";

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
    file_realtime_track << "date;jdt;pass_az;pass_el;track_az;track_el;az_h;az_min;az_sec;el_deg;el_min;el_sec";

    // Now, we are simulating real time prediction operations. We can now predict any position within the valid
    // tracking time window (stored in TrackingInfo struct). For the example, we will ask predictions from start to
    // end with a step of 0.1 (simulating real time operations at 10 Hz in the tracking mount).

    PredictionStarV predictions;

    JDateTime jd = jd_start;

    while (jd < jd_end)
    {
        predictions.push_back(predictor->predict(jd, true));
        jd = jd + 0.1L;
    }


    // Iterate the real time simulated predictions.
    for(const auto& pred : predictions)
    {
        // Auxiliar container for track data.
        std::string track_az = numberToStr(pred.altaz_coord.az,9, 6);
        std::string track_el = numberToStr(pred.altaz_coord.el,9, 6);

        int az_h, az_min;
        int el_h, el_min;
        double az_sec;
        double el_sec;

        // Get h, min, sec;
        dpslr::astro::types::degreesToDegMinSec(pred.altaz_coord.az, az_h, az_min, az_sec);
        dpslr::astro::types::degreesToDegMinSec(pred.altaz_coord.el, el_h, el_min, el_sec);

        // Get ISO
        dpbase::timing::types::HRTimePointStd tp_aux = dpbase::timing::julianDateTimeToTimePoint(pred.jdt);
        std::string iso_aux = dpbase::timing::timePointToIso8601(tp_aux);

        // Store the data.
        file_realtime_track << '\n';
        file_realtime_track << iso_aux <<";";
        file_realtime_track << std::to_string(pred.jdt.datetime()) <<";";
        file_realtime_track << track_az <<";";
        file_realtime_track << track_el <<";";
        file_realtime_track << std::to_string(az_h) <<";";
        file_realtime_track << std::to_string(az_min) <<";";
        file_realtime_track << std::to_string(az_sec) <<";";
        file_realtime_track << std::to_string(el_h) <<";";
        file_realtime_track << std::to_string(el_min) <<";";
        file_realtime_track << std::to_string(el_sec);

    }

    // Close the file.
    file_realtime_track.close();

    if(plot_data)
    {
        std::cout<<"Plotting real time simulated data using Python helpers..."<<std::endl;
        if(system(std::string(python_cmd_analysis + output_dir + "/" + realtime_csv_filename).c_str()))
            std::cout<<"Plotting failed!!"<<std::endl;
    }


    // Store the real time track data into a CSV file (only part of the data for easy usage).
    // TODO: Store the full track data as JSON.
    // --
    // Create the file and header.
    std::ofstream file_rt_interval_track(output_dir + "/" + rt_interval_csv_filename, std::ios_base::out);
    file_rt_interval_track << data.str();
    file_rt_interval_track << "date;jdt;pass_az;pass_el;track_az;track_el;az_h;az_min;az_sec;el_deg;el_min;el_sec";

    // Calculate predictions with interval method.
    predictions = predictor->predict(jd_start, jd_end, 100, true);

    // Iterate the real time simulated predictions.
    for(const auto& pred : predictions)
    {
        // Auxiliar container for track data.
        std::string track_az = numberToStr(pred.altaz_coord.az,9, 6);
        std::string track_el = numberToStr(pred.altaz_coord.el,9, 6);

        int az_h, az_min;
        int el_h, el_min;
        double az_sec;
        double el_sec;

        // Get h, min, sec;
        dpslr::astro::types::degreesToDegMinSec(pred.altaz_coord.az, az_h, az_min, az_sec);
        dpslr::astro::types::degreesToDegMinSec(pred.altaz_coord.el, el_h, el_min, el_sec);

        // Get ISO
        dpbase::timing::types::HRTimePointStd tp_aux = dpbase::timing::julianDateTimeToTimePoint(pred.jdt);
        std::string iso_aux = dpbase::timing::timePointToIso8601(tp_aux);

        // Store the data.
        file_rt_interval_track << '\n';
        file_rt_interval_track << iso_aux <<";";
        file_rt_interval_track << std::to_string(pred.jdt.datetime()) <<";";
        file_rt_interval_track << track_az <<";";
        file_rt_interval_track << track_el <<";";
        file_rt_interval_track << std::to_string(az_h) <<";";
        file_rt_interval_track << std::to_string(az_min) <<";";
        file_rt_interval_track << std::to_string(az_sec) <<";";
        file_rt_interval_track << std::to_string(el_h) <<";";
        file_rt_interval_track << std::to_string(el_min) <<";";
        file_rt_interval_track << std::to_string(el_sec);
    }

    // Close the file.
    file_rt_interval_track.close();

    if(plot_data)
    {
        std::cout<<"Plotting real time simulated data using Python helpers..."<<std::endl;
        if(system(std::string(python_cmd_analysis + output_dir + "/" + rt_interval_csv_filename).c_str()))
            std::cout<<"Plotting failed!!"<<std::endl;
    }


    // All ok.
    return (0);
}

// ---------------------------------------------------------------------------------------------------------------------

// =====================================================================================================================
