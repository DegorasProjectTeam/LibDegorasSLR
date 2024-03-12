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
using dpslr::timing::MJDateTime;
using dpslr::timing::SoD;
using dpslr::timing::types::HRTimePointStd;
using dpslr::timing::types::J2000DateTime;
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
// Astronomical containers.
using dpslr::astro::PredictionSun;
using dpslr::astro::AltAzPos;
// Sun predictor related.
using dpslr::astro::PredictorSunBase;
using dpslr::astro::PredictorSunFast;
using dpslr::astro::PredictorSunFixed;
using dpslr::astro::PredictorSunPtr;
// Mount predictor related.
using dpslr::mount::PredictorMountMove;
using dpslr::mount::MountPredictionMoveV;
using dpslr::mount::MountTrackingMove;
using dpslr::mount::TrackingAnalyzerConfig;
using dpslr::mount::PositionStatus;
// Helpers.
using dpslr::helpers::strings::numberToStr;
using dpslr::helpers::strings::split;
using dpslr::StringV;
// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// Auxiliar structs and classes.

struct ExampleData
{
    ExampleData(PredictorSunPtr sun_pred, const TrackingAnalyzerConfig& cfg, const std::string& alias,
                const std::string& file):
        example_alias(alias),
        file(file),
        predictor_sun(sun_pred),
        analyzer_cfg(cfg)
    {
    }

    // Specific example data.
    std::string example_alias;            // Example alias for file generation.
    std::string file;                     // Data file.
    PredictorSunPtr predictor_sun;        // Predictor Sun that will be used.
    TrackingAnalyzerConfig analyzer_cfg;  // Analyzer configuration.
};

// ---------------------------------------------------------------------------------------------------------------------

int main()
{
    // -------------------- INITIALIZATION -----------------------------------------------------------------------------

    // Initialize LibDegorasSLR.
    DegorasInit::init();

    // -------------------- EXAMPLES CONFIGURATION ---------------------------------------------------------------------

    // Example selector.
    size_t example_selector = 0;  // Select the example to process (between reals and sintetics).
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

    // Generic config.
    MillisecondsU step = 500;         // Steps into which the algorithm will divide the pass for initial analysis.
    DegreesU min_el = 0;             // Minimum acceptable elevation for the mount.
    DegreesU max_el = 85;             // Maximum acceptable elevation for the mount.
    DegreesU sun_avoid_angle = 15;    // Sun avoidance angle to make Sun the security sectors.
    bool avoid_sun = true;            // Flag for enable or disable the Sun avoidance utility.

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

    // Prepare the different tracking analyzer configurations.
    TrackingAnalyzerConfig analyzer_cfg_1(step, sun_avoid_angle, min_el, max_el, avoid_sun);
    TrackingAnalyzerConfig analyzer_cfg_2(step, sun_avoid_angle, min_el, max_el, false);

    // Prepare the Sun predictors to be used. For this example, the PredictorMount class needs the shared smart
    // pointers with the Sun and SLR inherited predictors, so it is neccesary to use the factories (the smart pointers
    // can also be created manually). This is not necessary when using the Sun and SLR predictors independently, as
    // can be seen in other LibDegorasSLR examples.
    //
    // Sintetic Sun predictors.
    PredictorSunPtr pred_sun_sin_1 = PredictorSunBase::factory<PredictorSunFixed>(AltAzPos(20L,30L));
    PredictorSunPtr pred_sun_sin_2 = PredictorSunBase::factory<PredictorSunFixed>(AltAzPos(225L,70L));
    PredictorSunPtr pred_sun_sin_3 = PredictorSunBase::factory<PredictorSunFixed>(AltAzPos(90L,25L));
    //
    // Real Sun predictor.
    PredictorSunPtr pred_sun_real = PredictorSunBase::factory<PredictorSunFast>(stat_geod);

    // Real examples vector with their configurations.
    // Sun Predictor - Alias - CPF - Pass Start - Pass End
    std::vector<ExampleData> examples =
    {
        // Example 0: 25,10 - 115,68 - ccw
        {pred_sun_real, analyzer_cfg_1,
            "25_10-115_ccw", "mount_trajectory_025-010_115-068_12_ccw.csv"},

    };

    // Get band store the example data.
    std::string positions_filepath = input_dir + "/" + examples[example_selector].file;
    std::string example_alias = examples[example_selector].example_alias;
    PredictorSunPtr predictor_sun = examples[example_selector].predictor_sun;
    TrackingAnalyzerConfig analyzer_cfg = examples[example_selector].analyzer_cfg;
    std::string track_csv_filename = example_alias + "_track_analyzed.csv";
    std::string realtime_csv_filename = example_alias + "_track_realtime.csv";

    // -------------------- PREDICTOR MOUNT PREPARATION  ---------------------------------------------------------------

    std::ifstream positions_stream(positions_filepath);
    std::string line;
    StringV tokens;
    PredictorMountMove::MovePositionV positions;

    if (!positions_stream.is_open())
    {
        // Cannot open example data
        std::cerr << "Module: TrackingMount   |   Example: PredictorMountMove" << std::endl;
        std::cerr << "Error: Cannot open example input data." << std::endl;
        std::cerr << "Example finished. Press Enter to exit..." << std::endl;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return -1;
    }

    // Discard header line
    std::getline(positions_stream, line);

    while (std::getline(positions_stream, line))
    {
        dpslr::helpers::strings::split(tokens, line, ",");

        if (tokens.size() != 4)
        {
            // Example input data bad format
            std::cerr << "Module: TrackingMount   |   Example: PredictorMountMove" << std::endl;
            std::cerr << "Error: Example input data bad format." << line << std::endl;
            std::cerr << "Example finished. Press Enter to exit..." << std::endl;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return -1;
        }

        try
        {
            long long mjd = static_cast<long long>(std::stold(tokens[0]));
            long double sod = std::stold(tokens[1]);
            Degrees az = std::stold(tokens[2]);
            Degrees el = std::stold(tokens[3]);

            auto tp = modifiedJulianDateTimeToTimePoint({static_cast<MJDate>(mjd), static_cast<SoD>(sod)});

            AltAzPos pos{az, el};
            positions.push_back({tp, pos});

        }
        catch (...)
        {
            // Example input data bad format
            std::cerr << "Module: TrackingMount   |   Example: PredictorMountMove" << std::endl;
            std::cerr << "Error: Example input data bad format." << std::endl;
            std::cerr << "Example finished. Press Enter to exit..." << std::endl;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return -1;
        }


    }

    // Prepare the mount slr predictor.
    PredictorMountMove predictor_mount(positions, predictor_sun, analyzer_cfg);

    // Check if the tracking is ready.
    if (!predictor_mount.isReady())
    {
        std::cerr << "Module: TrackingMount   |   Example: PredictorMountSLR" << std::endl;
        std::cerr << "Error: The PredictorMountSLR is not ready, maybe there is no valid pass." << std::endl;
        std::cerr << "Example finished. Press Enter to exit..." << std::endl;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return -1;
    }

    // -------------------- ALL IS OK. WE WILL SEE THE ANALYZED DATA ---------------------------------------------------

    // Get the analyzed mount track with all the relevant data. You can use this data for example to print
    // a polar plot with the space object pass, the mount track and the Sun position. In the example folder,
    // you can see a Python illustrative example using the data of this struct.
    const MountTrackingMove& mount_track =  predictor_mount.getMountTrackingMove();

    // Log the pass and tracking information (illustrative example). You can read the specific
    // documentation to learn what you can do with each class and struct.
    std::stringstream border, lines, data;
    border.width(80);
    lines.width(80);
    border.fill('=');
    lines.fill('-');
    border << "\n";
    lines << "\n";
    data<<border.str();
    data<<"= Module: TrackingMount   |   Example: PredictorMountMove" << std::endl;
    data<<border.str();
    data<<"= Intputs:" << std::endl;
    data<<lines.str();
    //std::cout<<"= Pass interval: " << mount_track. << std::endl;
    data<<"= Avoid Sun:   " << (mount_track.config.sun_avoid ? "true" : "false") << std::endl;
    data<<"= Avoid angle: " << mount_track.config.sun_avoid_angle << std::endl;
    data<<"= Delta:       " << mount_track.config.time_delta << std::endl;
    data<<"= Min el:      " << mount_track.config.min_elev << std::endl;
    data<<border.str();
    data<<"= Outputs:" << std::endl;
    data<<lines.str();
    //std::cout<<"= Track interval: " << mount_track. << std::endl;
    data<<"= Trim at start: " << (mount_track.track_info.trim_at_start ? "true" : "false") << std::endl;
    data<<"= Trim at end:   " << (mount_track.track_info.trim_at_end ? "true" : "false") << std::endl;
    data<<"= Sun collision: " << (mount_track.track_info.sun_collision ? "true" : "false") << std::endl;
    data<<"= Sun at start:  " << (mount_track.track_info.sun_collision_at_start ? "true" : "false") << std::endl;
    data<<"= Sun at end:    " << (mount_track.track_info.sun_collision_at_end ? "true" : "false") << std::endl;
    data<<"= Sun deviation: " << (mount_track.track_info.sun_deviation ? "true" : "false") << std::endl;
    data<<"= El deviation: " << (mount_track.track_info.el_deviation ? "true" : "false") << std::endl;
    //TODO Etc
    data<<border.str();
    // Show the data.
    std::cout<<data.str();

    // --
    // Store the analyzed track data into a CSV file (only part of the data for easy usage).
    // TODO: Store the full track data as JSON.
    // --
    // Create the file and header.
    std::ofstream file_analyzed_track(output_dir + "/" + track_csv_filename, std::ios_base::out);
    file_analyzed_track << data.str();
    file_analyzed_track << "mjd;sod;pass_az;pass_el;track_az;track_el;sun_az;sun_el";
    //
    // Iterate the predictions. At this point, all the the real satellite position data must be valid. If the
    // predictor had failed or the data entered did not match a pass, the tracking would not be valid directly.
    for(const auto& pred : mount_track.predictions)
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
            file_analyzed_track <<'\n';
            file_analyzed_track << std::to_string(timePointToModifiedJulianDateTime(pred.tp).datetime()) <<";";
            file_analyzed_track << orig_az <<";";
            file_analyzed_track << orig_el <<";";
            file_analyzed_track << track_az <<";";
            file_analyzed_track << track_el <<";";
            file_analyzed_track << numberToStr(pred.sun_pred->altaz_coord.az, 7, 4) <<";";
            file_analyzed_track << numberToStr(pred.sun_pred->altaz_coord.el, 7, 4);
        }
    }
    //
    // Close the file.
    file_analyzed_track.close();
    // --

    if(plot_data)
    {
        std::cout<<"Plotting analyzed data using Python helpers..."<<std::endl;
        if(system(std::string(python_cmd_analysis + output_dir + "/" + track_csv_filename).c_str()))
            std::cout<<"Plotting failed!!"<<std::endl;
    }

    // -------------------- NOW LET'S START CALCULATING PREDICTIONS SIMULATING REAL TIME PROCESS -----------------------

    // TODO
    // We have some issues at this momment in the real time predictor.

    // Get the new tracking start and end date. If there is sun overlapping at start or end, the affected date
    // is changed so the tracking will start or end after/before the sun security sector.
    MJDateTime track_start = mount_track.track_info.mjdt_start;
    MJDateTime track_end = mount_track.track_info.mjdt_end;

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
    MJDateTime mjd = track_start;
    MountPredictionMoveV results;

    while (mjd < track_end)
    {
        // Store the resulting prediction
        results.push_back({});
        auto status = predictor_mount.predict(modifiedJulianDateTimeToTimePoint(mjd), results.back());

        if (status == PositionStatus::ELEVATION_CLIPPED)
        {
            //
        }
        else if (status == PositionStatus::OUTSIDE_SUN)
        {
            // In this case the position predicted is valid and it is going outside a sun security sector. This is the
            // normal case.
        }
        else if (status == PositionStatus::INSIDE_SUN)
        {
            // In this case the position predicted is valid, but it is going through a sun security sector.
            // This case is only possible if sun avoid algorithm is disabled.
            // BEWARE. If the mount points directly to this position it could be dangerous.
        }

        else if (status == PositionStatus::AVOIDING_SUN)
        {
            // In this case the position predicted is valid and it is going through an alternative way to avoid a sun
            // security sector. While the tracking returns this status, the tracking_position member in result
            // represents the position used to avoid the sun (the secure position), while prediction_result contains
            // the true position of the object (not secure).

        }
        else if (status == PositionStatus::CANT_AVOID_SUN)
        {
            // In this case the position predicted is valid and it is going through an alternative way to avoid a sun
            // security sector. While the tracking returns this status, the tracking_position member in result
            // represents the position used to avoid the sun (the secure position), while prediction_result contains
            // the true position of the object (not secure).

        }
        else if (status == PositionStatus::OUT_OF_TRACK)
        {
            // Bad situation, the prediction time requested is out of track. Stop the tracking and notify to client.
            // However, this should not happen if all is ok in the mount tracking controller software. Maybe if
            // something is wrong with the CPF or in the timing tracking mount subsystem or in the SLR station system.
            std::cerr << "Module: TrackingMount   |   Example: PredictorMountSLR" << std::endl;
            std::cerr << "Error: The requested position is in OUT_OF_TRACK state." << std::endl;
            std::cerr << "Example finished. Press Enter to exit..." << std::endl;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return -1;
        }
        else if (status == PositionStatus::PREDICTION_ERROR)
        {
            // Bad situation, stop the tracking and notify to client. However, this should not happen if all is ok in
            // the mount tracking controller software. Maybe if something is wrong with the with the CPF or in the
            // timing tracking mount subsystem or in the SLR station system.
            std::cerr << "Module: TrackingMount   |   Example: PredictorMountSLR" << std::endl;
            std::cerr << "Error: The requested position is in PREDICTION_ERROR state." << std::endl;
            std::cerr << "Example finished. Press Enter to exit..." << std::endl;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return -1;
        }

        // Advance to next position
        mjd.add(Seconds(0.1L));
    }

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

    // All ok.
    return (0);
}

// ---------------------------------------------------------------------------------------------------------------------

// =====================================================================================================================
