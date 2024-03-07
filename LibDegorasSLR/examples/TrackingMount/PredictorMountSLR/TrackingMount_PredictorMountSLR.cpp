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
using dpslr::timing::types::Iso8601Str;
using dpslr::timing::iso8601DatetimeToTimePoint;
using dpslr::timing::timePointToModifiedJulianDateTime;
// Used units.
using dpslr::math::units::Angle;
using dpslr::math::units::DegreesU;
using dpslr::math::units::Degrees;
using dpslr::math::units::MillisecondsU;
using dpslr::math::units::Meters;
// Geocentric and geodetic containers.
using dpslr::geo::types::GeocentricPoint;
using dpslr::geo::types::GeodeticPoint;
// Astronomical containers.
using dpslr::astro::SunPrediction;
using dpslr::astro::AltAzPos;
// ILRS related.
using dpslr::ilrs::cpf::CPF;
// Sun predictor related.
using dpslr::astro::PredictorSunBase;
using dpslr::astro::PredictorSunFast;
using dpslr::astro::PredictorSunFixed;
using dpslr::astro::PredictorSunPtr;
// SLR preditor related.
using dpslr::utils::PredictorSlrBase;
using dpslr::utils::PredictorSlrCPF;
using dpslr::utils::PredictorSlrPtr;
using dpslr::utils::PredictorSlrCPFPtr;
// Mount predictor related.
using dpslr::mount::PredictorMountSLR;
// Helpers.
using dpslr::helpers::strings::numberToStr;
using dpslr::mount::PositionStatus;
using dpslr::mount::MountTrackingSLR;
using dpslr::mount::TrackingAnalyzerConfig;
// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// Auxiliar structs and classes.

struct ExampleData
{
    ExampleData(PredictorSunPtr sun_pred, const std::string& alias, const std::string& cpf,
                const Iso8601Str& start, const Iso8601Str& end):
        example_alias(alias),
        cpf_name(cpf),
        predictor_sun(sun_pred)
    {
        this->mjdt_start = timePointToModifiedJulianDateTime(iso8601DatetimeToTimePoint(start));
        this->mjdt_end = timePointToModifiedJulianDateTime(iso8601DatetimeToTimePoint(end));
    }

    // Specific example data.
    std::string example_alias;      // Example alias for file generation.
    std::string cpf_name;           // CPF ephemeris namefile.
    MJDateTime mjdt_start;          // Space object pass fragment start Modified Julian Datetime.
    MJDateTime mjdt_end;            // Space object pass fragment end Modified Julian Datetime.
    PredictorSunPtr predictor_sun;  // Predictor Sun that will be used.
};

// ---------------------------------------------------------------------------------------------------------------------

int main()
{
    // -------------------- INITIALIZATION -----------------------------------------------------------------------------

    // Initialize LibDegorasSLR.
    DegorasInit::init();

    // -------------------- EXAMPLES CONFIGURATION ---------------------------------------------------------------------

    // Example selector.
    size_t example_selector = 4;  // Select the example to process (between reals and sintetics).
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

    // TrackingSLR configuration.
    MillisecondsU step = 500;         // Steps into which the algorithm will divide the pass for initial analysis.
    DegreesU min_el = 15;             // Minimum acceptable elevation for the mount.
    DegreesU max_el = 85;             // Maximum acceptable elevation for the mount.
    DegreesU sun_avoid_angle = 15;    // Sun avoidance angle to make Sun the security sectors.
    bool avoid_sun = true;            // Flag for enable or disable the Sun avoidance utility.

    // Configure the CPF input folder.
    std::string current_dir = dpslr::helpers::files::getCurrentDir();
    std::string input_dir(current_dir+"/inputs");
    std::string output_dir(current_dir+"/outputs");

    // Configure the python script executable.
    std::string python_plot_analysis(current_dir+"/python_scripts/Helper_Plotting_Analysis.py");
    std::string python_plot_track(current_dir+"/python_scripts/Helper_Plotting_Track.py");
    std::string python_cmd_analysis = "python \"" + python_plot_analysis + "\" ";
    std::string python_cmd_track = "python \"" + python_plot_track + "\" ";

    // Create the ouput directory.
    if (!dpslr::helpers::files::directoryExists(output_dir))
        dpslr::helpers::files::createDirectory(output_dir);

    // -------------------- EXAMPLES PREPARATION -----------------------------------------------------------------------

    // Store the local geocentric and geodetic coordinates.
    GeocentricPoint stat_geoc(x,y,z);
    GeodeticPoint<Degrees> stat_geod(latitude, longitude, alt);

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
        // Example 0: Lares | Sun at beginning | SW-N_CW
        {pred_sun_real, "Lares_SunBeg", "38077_cpf_240128_02901.sgf",
            "2024-01-31T15:45:25Z", "2024-01-31T16:02:35Z"},

        // Example 1: Jason 3 | Sun at middle | NW-SE-CCW
        {pred_sun_real, "Jason3_SunMid", "41240_cpf_240128_02801.hts",
            "2024-01-31T11:42:20Z", "2024-01-31T11:59:10Z"},

        // Example 2: Explorer 27 | Sun at end | El deviation | WW-ESE-CCW
        {pred_sun_real, "Explorer27_SunEnd", "1328_cpf_240128_02901.sgf",
            "2024-01-31T08:31:27Z", "2024-01-31T08:44:27Z"},

        // Example 3: Jason 3 | No Sun | N-E-CW
        {pred_sun_real, "Jason3_NoSun", "41240_cpf_240128_02801.hts",
            "2024-01-31T09:47:30Z", "2024-01-31T10:01:00Z"},

        // Example 4: Jason 3 | Sun at middle | N-E-CW
        {pred_sun_sin_1, "Jason3_SunMid_Sintetic_1", "41240_cpf_240128_02801.hts",
            "2024-01-31T09:47:30Z", "2024-01-31T10:01:00Z"},

        // Example 5: Jason 3 | Sun at middle in high el | NW-SE-CCW
        {pred_sun_sin_2, "Jason3_SunMid_Sintetic_1", "41240_cpf_240128_02801.hts",
            "2024-01-31T11:42:20Z", "2024-01-31T11:59:10Z"},

        // Example 6: Jason 3 | Sun at beginning | NE-E-CW
        {pred_sun_sin_1, "Jason3_SunMid_Sintetic_2", "41240_cpf_240128_02801.hts",
            "2024-01-31T09:51:00Z", "2024-01-31T10:01:00Z"},

        // Example 7: Jason 3 | Sun at end | N-ENE-CW
        {pred_sun_sin_3, "Jason3_SunMid_Sintetic_3", "41240_cpf_240128_02801.hts",
            "2024-01-31T09:47:30Z", "2024-01-31T09:59:00Z"}
    };

    // Get band store the example data.
    std::string cpf_path = input_dir + "/" + examples[example_selector].cpf_name;
    MJDateTime mjd_pass_start = examples[example_selector].mjdt_start;
    MJDateTime mjd_pass_end = examples[example_selector].mjdt_end;
    std::string example_alias = examples[example_selector].example_alias;
    PredictorSunPtr predictor_sun = examples[example_selector].predictor_sun;
    std::string track_csv_filename = example_alias + "_track.csv";

    // -------------------- PREDICTOR MOUNT PREPARATION  ---------------------------------------------------------------

    // Prepare the SLR predictor to be used. For this example, the PredictorMount class needs the shared smart
    // pointers with the Sun and SLR inherited predictors, so it is neccesary to use the factories (the smart pointers
    // can also be created manually). This is not necessary when using the Sun and SLR predictors independently, as
    // can be seen in other LibDegorasSLR examples.
    PredictorSlrPtr predictor_cpf = PredictorSlrBase::factory<PredictorSlrCPF>(cpf_path, stat_geod, stat_geoc);

    // Check if the predictor is ready.
    if (!predictor_cpf->isReady())
    {
        std::cerr << "Module: TrackingMount   |   Example: PredictorMountSLR" << std::endl;
        std::cerr << "Error: The PredictorSlrCPF is not ready, check CPF inputs." << std::endl;
        std::cerr << "Example finished. Press Enter to exit..." << std::endl;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return -1;
    }

    // Configure the SLR predictor_mount. The class will process the pass automatically and will
    // generate a preview mount track in the steps indicated by step_ms.
    // Store the configuration data.
    TrackingConfig config;
    config.mjdt_start = mjd_pass_start;
    config.mjdt_end = mjd_pass_end;
    config.min_elev = min_el;
    config.max_elev = max_el;
    config.time_delta = step;
    config.sun_avoid_angle = sun_avoid_angle;
    config.sun_avoid = avoid_sun;

    PredictorMountSLR predictor_mount(predictor_cpf, predictor_sun, config);

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

    // Get the specializated SLR predictor to get useful information.
    PredictorSlrCPFPtr pred_cpf_recover = PredictorSlrBase::specialization<PredictorSlrCPF>(predictor_cpf);

    // Get the new tracking start and end date. If there is sun overlapping at start or end, the affected date
    // is changed so the tracking will start or end after/before the sun security sector.

    // Get the analyzed mount track with all the relevant data. You can use this data for example to print
    // a polar plot with the space object pass, the mount track and the Sun position. In the example folder,
    // you can see a Python illustrative example using the data of this struct.
    const MountTrackingSLR& mount_track =  predictor_mount.getMountTrackingSLR();

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
    data<<"= Module: TrackingMount   |   Example: PredictorMountSLR" << std::endl;
    data<<border.str();
    data<<"= Intputs:" << std::endl;
    data<<lines.str();
    data<<"= File:        " << pred_cpf_recover->getCPF().getSourceFilename() << std::endl;
    data<<"= Object:      " << pred_cpf_recover->getCPF().getHeader().basicInfo1Header()->target_name << std::endl;
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
        std::string track_az = "";
        std::string track_el = "";
        //
        // At this point, you only must check if the prediction is outside track. This is becaouse, for example,
        // the beginning of the real satellite pass may coincide with the Sun sector, so at those points there
        // would be no data from the mount's track, only the real pass.
        if(pred.status != PositionStatus::OUT_OF_TRACK)
        {
            track_az = numberToStr(pred.mount_pos->altaz_coord.az,7, 4);
            track_el = numberToStr(pred.mount_pos->altaz_coord.el,7, 4);

            // Store the data.
            file_analyzed_track <<'\n';
            file_analyzed_track << std::to_string(pred.mjdt.datetime()) <<";";
            file_analyzed_track << numberToStr(pred.slr_pred->instant_data->altaz_coord.az, 7, 4) <<";";
            file_analyzed_track << numberToStr(pred.slr_pred->instant_data->altaz_coord.el, 7, 4) <<";";
            file_analyzed_track << track_az <<";";
            file_analyzed_track << track_el <<";";
            file_analyzed_track << numberToStr(pred.sun_pred->altaz_coord.az, 7, 4) <<";";
            file_analyzed_track << numberToStr(pred.sun_pred->altaz_coord.el, 7, 4);
        }
        //

    }
    //
    // Close the file.
    file_analyzed_track.close();
    // --

    if(plot_data)
    {
        std::cout<<"Plotting analyzed data using Python helpers..."<<std::endl;
        python_cmd_analysis += (output_dir + "/" + track_csv_filename);
        if(system(python_cmd_analysis.c_str()))
            std::cout<<"Plotting failed!!"<<std::endl;
    }

    // -------------------- NOW LET'S START CALCULATING PREDICTIONS SIMULATING REAL TIME PROCESS -----------------------

    // TODO
    // We have some issues at this momment in the real time predictor.

    /*

    // Real time.

    // Now, we have the tracking configured, so we can ask the tracking to predict any position within the valid
    // tracking time window (determined by tracking start and tracking end). For the example, we will ask
    // predictions from start to end with a step of 0.5 s.
    MJDateTime mjd = mjd_start;
    PredictorMountSLR::MountSLRPredictions results;

    while (mjd < mjd_end)
    {
        // Store the resulting prediction
        results.push_back({});
        auto status = predictor_mount.predict(mjd, results.back());

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
        mjd.add(0.5L);
    }

    */

    // Final wait.
    std::cout << "Example finished. Press Enter to exit..." << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // All ok.
    return (0);
}

// ---------------------------------------------------------------------------------------------------------------------

// =====================================================================================================================
