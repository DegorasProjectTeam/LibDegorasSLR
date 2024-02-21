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
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include <LibDegorasSLR/Initialization>
#include <LibDegorasSLR/ModuleHelpers>
#include <LibDegorasSLR/ModuleUtilitiesSLR>
#include <LibDegorasSLR/ModuleFormatsILRS>
#include <LibDegorasSLR/ModuleTiming>
#include <LibDegorasSLR/ModuleMathematics>
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
// LibDegorasSLR types used in example.
using dpslr::DegorasInit;
using dpslr::ilrs::cpf::CPF;
using dpslr::geo::types::GeocentricPointLD;
using dpslr::geo::types::GeodeticPointLD;
using dpslr::utils::PredictorSLR;
using dpslr::utils::PredictorMountSLR;
using dpslr::timing::MJDate;
using dpslr::timing::SoD;
using dpslr::math::units::Angle;
using dpslr::helpers::strings::numberToStr;
using dpslr::math::units::DegreesU;
using dpslr::math::units::Degrees;
using dpslr::math::units::MillisecondsU;
using dpslr::math::units::Meters;
// ---------------------------------------------------------------------------------------------------------------------

// Auxiliar structs.

// ---------------------------------------------------------------------------------------------------------------------

struct ExampleData
{
    std::string example_alias;
    std::string cpf_name;
    MJDate mjd_start;
    SoD sod_start;
    MJDate mjd_end;
    SoD sod_end;
};

// ---------------------------------------------------------------------------------------------------------------------

int main()
{
    // -------------------- INITIALIZATION -----------------------------------------------------------------------------

    // Initialize LibDegorasSLR.
    DegorasInit::init();

    // -------------------- EXAMPLE CONFIGURATION ----------------------------------------------------------------------

    // SFEL station geodetic coordinates.
    Degrees latitude = 36.46525556L;
    Degrees longitude = 353.79469440L;  // Always > 0.
    Meters alt = 98.177L;

    // SFEL station geocentric coordinates (meters).
    Meters x = 5105473.885L;
    Meters y = -555110.526L;
    Meters z = 3769892.958L;

    // TrackingSLR configuration.
    MillisecondsU step_ms = 100;
    DegreesU min_el = 15;
    DegreesU max_el = 80;
    DegreesU sun_avoid_angle = 15;
    bool avoid_sun = true;

    // Configure the CPF folder and example file.
    std::string current_dir = dpslr::helpers::files::getCurrentDir();
    std::string input_dir(current_dir+"/inputs");
    std::string output_dir(current_dir+"/outputs");

    // Examples vector with their configurations.
    std::vector<ExampleData> examples =
    {
        // Example 0: Lares with Sun at beginning.
        {"Lares_SunBeg", "38077_cpf_240128_02901.sgf", 60340, 56726.0L, 60340, 57756.0L},
        // Example 1: Jason 3 with Sun in the middle.
        {"Jason3_SunMid", "41240_cpf_240128_02801.hts", 60340, 42140.0L, 60340, 43150.0L},
        // Example 2: Explorer 27 with Sun in the end.
        {"Explorer27_SunEnd", "1328_cpf_240128_02901.sgf", 60340, 30687.0L, 60340, 31467.0L}
    };

    // Example selector.
    size_t example_selector = 2;

    // Store the example data.
    std::string cpf_path = input_dir + "/" + examples[example_selector].cpf_name;
    MJDate mjd_start = examples[example_selector].mjd_start;
    SoD sod_start = examples[example_selector].sod_start;
    MJDate mjd_end = examples[example_selector].mjd_end;
    SoD sod_end = examples[example_selector].sod_end;
    std::string example_alias = examples[example_selector].example_alias;

    // Check the input file.
    if(!dpslr::helpers::files::fileExists(cpf_path))
    {
        std::cerr << "Input file does not exist." << std::endl;
        return -1;
    }

    // Create the ouput directory.
    if (!dpslr::helpers::files::directoryExists(output_dir))
        dpslr::helpers::files::createDirectory(output_dir);

    // -------------------- UTILITIES INSTANTIATION  -------------------------------------------------------------------

    // Store the local geocentric and geodetic coordinates.
    GeocentricPointLD stat_geocentric(x,y,z);
    GeodeticPointLD stat_geodetic(latitude, longitude, alt, Angle<long double>::Unit::DEGREES);

    // Open the CPF file (all data).
    CPF cpf(cpf_path, dpslr::ilrs::cpf::CPF::OpenOptionEnum::ALL_DATA);

    // Check that the CPF has data.
    if (!cpf.hasData())
    {
        std::cerr << "The CPF has no valid data." << std::endl;
        return -1;
    }

    // Configure the SLR predictor_slr.
    PredictorSLR predictor_slr(cpf, stat_geodetic, stat_geocentric);

    // Check if the predictor is ready.
    if (!predictor_slr.isReady())
    {
        std::cerr << "The predictor has no valid data to do predictions." << std::endl;
        return -1;
    }

    // Configure the SLR predictor_mount. The class will process the pass automatically and will
    // generate a preview mount track in the steps indicated by step_ms.
    PredictorMountSLR predictor_mount(std::move(predictor_slr), mjd_start, sod_start, mjd_end, sod_end,
                                      step_ms, min_el, max_el , sun_avoid_angle, avoid_sun);

    // Check if the tracking is valid.
    if (!predictor_mount.isValid())
    {
        std::cerr << "There is no valid tracking." << std::endl;
        return -1;
    }

    // -------------------- ALL IS OK. WE WILL SEE THE ANALYZED DATA ---------------------------------------------------

    // Get the new tracking start and end date. If there is sun overlapping at start or end, the affected date
    // is changed so the tracking will start or end after/before the sun security sector.
    predictor_mount.getTrackingStart(mjd_start, sod_start);
    predictor_mount.getTrackingEnd(mjd_end, sod_end);

    // Get the analyzed mount track with all the relevant data. You can use this data for example to print
    // a polar plot with the space object pass, the mount track and the Sun position. In the example folder,
    // you can see a Python illustrative example using the data of this struct.
    const PredictorMountSLR::MountTrackSLR& mount_track =  predictor_mount.getMountTrack();

    // Log the pass and tracking information (illustrative example). You can read the specific
    // documentation to learn what you can do with each class and struct.
    std::stringstream border;
    std::stringstream lines;
    std::stringstream data;
    border.fill('=');
    lines.fill('-');
    border.width(80);
    lines.width(80);
    border << "\n";
    lines << "\n";
    data<<border.str();
    data<<"= Module: UtilititesSLR   |   Example: TrackingSLR" << std::endl;
    data<<border.str();
    data<<"= Intputs:" << std::endl;
    data<<lines.str();
    data<<"= File:        " << mount_track.cpf.getSourceFilename() << std::endl;
    data<<"= Object:      " << mount_track.cpf.getHeader().basicInfo1Header()->target_name << std::endl;
    //std::cout<<"= Pass interval: " << mount_track. << std::endl;
    data<<"= Avoid Sun:   " << (mount_track.cfg_sun_avoid ? "true" : "false") << std::endl;
    data<<"= Avoid angle: " << mount_track.cfg_sun_avoid_angle << std::endl;
    data<<"= Delta:       " << mount_track.cfg_time_delta << std::endl;
    data<<"= Min el:      " << mount_track.cfg_min_elev << std::endl;
    data<<border.str();
    data<<"= Outputs:" << std::endl;
    data<<lines.str();
    //std::cout<<"= Track interval: " << mount_track. << std::endl;
    data<<"= Trim at start: " << (mount_track.trim_at_start ? "true" : "false") << std::endl;
    data<<"= Trim at end:   " << (mount_track.trim_at_end ? "true" : "false") << std::endl;
    data<<"= Sun collision: " << (mount_track.sun_collision ? "true" : "false") << std::endl;
    data<<"= Sun at start:  " << (mount_track.sun_collision_at_start ? "true" : "false") << std::endl;
    data<<"= Sun at end:    " << (mount_track.sun_collision_at_end ? "true" : "false") << std::endl;
    data<<"= Sun deviation: " << (mount_track.sun_deviation ? "true" : "false") << std::endl;
    data<<"= El deviation: " << (mount_track.el_deviation ? "true" : "false") << std::endl;
    //TODO Etc
    data<<border.str();
    // Show the data.
    std::cout<<data.str();

    // --
    // Store the analyzed track data into a CSV file (only part of the data for easy usage).
    // --
    // Create the file and header.
    std::ofstream file_analyzed_track(output_dir + "/" + example_alias + "_track.csv", std::ios_base::out);
    file_analyzed_track << data.str();
    file_analyzed_track << "mjd;sod;pass_az;pass_el;track_az;track_el;sun_az;sun_el";
    //
    // Iterate the predictions. At this point, all the the real satellite position data must be valid. If the
    // predictor had failed or the data entered did not match a pass, the tracking would not be valid directly.
    for(const auto& pred : mount_track.predictions)
    {
        // Auxiliar container for track data.
        std::string track_az = "";
        std::string track_el = "";
        //
        // At this point, you only must check if the prediction is outside track. This is becaouse, for example,
        // the beginning of the real satellite pass may coincide with the Sun sector, so at those points there
        // would be no data from the mount's track, only the real pass.
        if(pred.status != PredictorMountSLR::PositionStatus::OUT_OF_TRACK)
        {
            track_az = numberToStr(pred.tracking_position->az,7, 4);
            track_el = numberToStr(pred.tracking_position->el,7, 4);
        }
        //
        // Store the data.
        file_analyzed_track <<'\n';
        file_analyzed_track << std::to_string(pred.mjd) <<";" << std::to_string(pred.sod) <<";";
        file_analyzed_track << numberToStr(pred.prediction_result->instant_data->az,7, 4) <<";";
        file_analyzed_track << numberToStr(pred.prediction_result->instant_data->el,7, 4) <<";";
        file_analyzed_track << track_az <<";";
        file_analyzed_track << track_el <<";";
        file_analyzed_track << numberToStr(pred.sun_position->az,7, 4) <<";";
        file_analyzed_track << numberToStr(pred.sun_position->el,7, 4);
    }
    //
    // Close the file.
    file_analyzed_track.close();
    // --

    // -------------------- NOW LET'S START CALCULATING PREDICTIONS ----------------------------------------------------



    if (predictor_mount.isSunOverlapping())
    {
        std::cout << "Sun overlapping." << std::endl;

        if (predictor_mount.isSunAtStart())
        {
            std::cout << "Sun overlapping at the beginning." << std::endl;
            // Get the new tracking start date
            predictor_mount.getTrackingStart(mjd_start, sod_start);
        }

        if (predictor_mount.isSunAtEnd())
        {
            std::cout << "Sun overlapping at the end." << std::endl;
            // Get the new tracking end date.
            predictor_mount.getTrackingEnd(mjd_end, sod_end);
        }
    }



    // Real time.

    // Now, we have the tracking configured, so we can ask the tracking to predict any position within the valid
    // tracking time window (determined by tracking start and tracking end). For the example, we will ask
    // predictions from start to end with a step of 0.5 s.
    MJDate mjd = mjd_start;
    SoD sod = sod_start;
    PredictorMountSLR::MountSLRPredictions results;

    while (mjd < mjd_end || sod < sod_end)
    {

        // Store the resulting prediction
        results.push_back({});
        auto status = predictor_mount.predict(mjd, sod, results.back());

        if (status == PredictorMountSLR::PositionStatus::INSIDE_SUN)
        {
            // In this case the position predicted is valid, but it is going through a sun security sector.
            // This case is only possible if sun avoid algorithm is disabled.
            // BEWARE. If the mount points directly to this position it could be dangerous.
        }
        else if (status == PredictorMountSLR::PositionStatus::OUTSIDE_SUN)
        {
            // In this case the position predicted is valid and it is going outside a sun security sector. This is the
            // normal case.
        }
        else if (status == PredictorMountSLR::PositionStatus::AVOIDING_SUN)
        {
            // In this case the position predicted is valid and it is going through an alternative way to avoid a sun
            // security sector. While the tracking returns this status, the tracking_position member in result
            // represents the position used to avoid the sun (the secure position), while prediction_result contains
            // the true position of the object (not secure).

        }
        else
        {
            //std::cout << "Error at getting position " << status;
            return -1;
        }

        // Advance to next position
        sod += 0.5L;
        if (sod > 86400.L)
        {
            sod -= 86400.L;
            mjd++;
        }
    }

    // We will store the positions in a file. This could be used for graphical representation.
    // We will also store the sun position at each tracking position.
    std::ofstream file_pos(output_dir + "/" + example_alias + "_tracking.dat", std::ios_base::out);
    std::ofstream file_pos_sun(output_dir + "/" + example_alias + "_sun.dat", std::ios_base::out);

    for (const auto &prediction : results)
    {
        file_pos << prediction.tracking_position->az << "," << prediction.tracking_position->el << std::endl;
        file_pos_sun << prediction.sun_position->az << "," << prediction.sun_position->el << std::endl;
    }

    file_pos.close();
    file_pos_sun.close();

    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

// =====================================================================================================================
