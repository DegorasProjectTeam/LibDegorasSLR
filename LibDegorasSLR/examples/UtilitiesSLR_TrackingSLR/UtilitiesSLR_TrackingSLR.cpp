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
#include <LibDegorasSLR/ModuleTiming>
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
// LibDegorasSLR types used in example.
using dpslr::DegorasInit;
using dpslr::ilrs::cpf::CPF;
using dpslr::geo::types::GeocentricPoint;
using dpslr::geo::types::GeodeticPoint;
using dpslr::utils::PredictorSLR;
using dpslr::utils::PredictorMountSLR;
using dpslr::timing::MJDate;
using dpslr::timing::SoD;
using dpslr::math::units::Angle;
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
    // Initialize LibDegorasSLR.
    DegorasInit::init();

    // SFEL station geodetic and geocentric coordinates.
    long double latitude = 36.46525556L;
    long double longitude = 353.79469440L;
    long double alt = 98.177L;
    long double x = 5105473.885L;
    long double y = -555110.526L;
    long double z = 3769892.958L;

    // TrackingSLR configuration.
    unsigned step_ms = 500;
    unsigned min_el = 8;
    unsigned sun_avoid_angle = 10;
    bool avoid_sun = true;

    // Configure the CPF folder and example file.
    std::string current_dir = dpslr::helpers::files::getCurrentDir();
    std::string input_dir(current_dir+"/inputs");
    std::string output_dir(current_dir+"/outputs");

    // Examples vector with their configurations.
    std::vector<ExampleData> examples =
    {
        // Example 1: Lares with Sun at beginning.
        {"Lares_Sun_Beg", "38077_cpf_240128_02901.sgf", 60340, 56726, 60340, 57756},
        // Example 2: Jason 3 with Sun in the middle.
        {"Jason3_Sun_Mid", "41240_cpf_240128_02801.hts", 60340, 42140, 60340, 43150},
        // Example 3: Explorer 27 with Sun in the end.
        {"Explorer27_Sun_End", "1328_cpf_240128_02901.sgf", 60340, 30687, 60340, 31467}
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

    // Store the local coordinates.
    GeocentricPoint<long double> stat_geocentric(x,y,z);
    GeodeticPoint<long double> stat_geodetic(latitude, longitude, alt, Angle<long double>::Unit::DEGREES);

    // Check the input directory.
    if (!dpslr::helpers::files::directoryExists(input_dir))
    {
        std::cerr << "Input directory does not exist." << std::endl;
        return -1;
    }

    // Check the input file.
    if(!dpslr::helpers::files::fileExists(cpf_path))
    {
        std::cerr << "Input file does not exist." << std::endl;
        return -1;
    }

    // Create the ouput directory.
    if (!dpslr::helpers::files::directoryExists(output_dir))
        dpslr::helpers::files::createDirectory(output_dir);

    // Open the CPF file.
    CPF cpf(cpf_path, dpslr::ilrs::cpf::CPF::OpenOptionEnum::ALL_DATA);

    // Check that the CPF has data.
    if (!cpf.hasData())
    {
        std::cerr << "The CPF has no valid data." << std::endl;
        return -1;
    }

    // Configure the SLR predictor.
    PredictorSLR predictor(cpf, stat_geodetic, stat_geocentric);

    if (!predictor.isReady())
    {
        std::cerr << "The predictor has no data valid to predict." << std::endl;
        return -1;
    }

    // Configure the SLR tracking passing the predictor, the start and end dates and minimum elevation (optional).
    PredictorMountSLR tracking(std::move(predictor), mjd_start, sod_start, mjd_end, sod_end,
                         min_el, step_ms, avoid_sun, sun_avoid_angle);

    if (!tracking.isValid())
    {
        std::cerr << "There is no valid tracking." << std::endl;
        return -1;
    }

    // Check for sun overlapping in tracking. If there is sun overlapping at start or end, the affected date is changed
    // so the tracking will start or end after/before the sun security sector.
    if (tracking.isSunOverlapping())
    {
        std::cout << "There is sun overlapping" << std::endl;

        if (tracking.isSunAtStart())
        {
            std::cout << "Sun overlapping at the beginning" << std::endl;
            // Get the new tracking start date
            tracking.getTrackingStart(mjd_start, sod_start);
        }

        if (tracking.isSunAtEnd())
        {
            std::cout << "Sun overlapping at the end" << std::endl;
            // Get the new tracking end date.
            tracking.getTrackingEnd(mjd_end, sod_end);
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
        auto status = tracking.predict(mjd, sod, results.back());

        if (status == PredictorMountSLR::INSIDE_SUN)
        {
            // In this case the position predicted is valid, but it is going through a sun security sector.
            // This case is only possible if sun avoid algorithm is disabled.
            // BEWARE. If the mount points directly to this position it could be dangerous.
        }
        else if (status == PredictorMountSLR::OUTSIDE_SUN)
        {
            // In this case the position predicted is valid and it is going outside a sun security sector. This is the
            // normal case.
        }
        else if (status == PredictorMountSLR::AVOIDING_SUN)
        {
            // In this case the position predicted is valid and it is going through an alternative way to avoid a sun
            // security sector. While the tracking returns this status, the tracking_position member in result
            // represents the position used to avoid the sun (the secure position), while prediction_result contains
            // the true position of the object (not secure).

        }
        else
        {
            std::cout << "Error at getting position " << status;
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
        file_pos_sun << prediction.sun_pos->azimuth << "," << prediction.sun_pos->elevation << std::endl;
    }

    file_pos.close();
    file_pos_sun.close();

    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

// =====================================================================================================================
