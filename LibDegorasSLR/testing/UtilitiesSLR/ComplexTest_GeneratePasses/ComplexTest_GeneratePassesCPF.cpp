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

// LIBRARY INCLUDES
// =====================================================================================================================
#include <LibDegorasSLR/Initialization>
#include <LibDegorasSLR/Modules/UtilitiesSLR>
// =====================================================================================================================

// LIBDPBASE INCLUDES
// =====================================================================================================================
#include <LibDegorasBase/Modules/Timing>
// =====================================================================================================================


// ---------------------------------------------------------------------------------------------------------------------
// LibDegorasSLR types used in this example.
//
// Initialization.
using dpslr::DegorasInit;
// Time tipes and conversions.
using dpbase::timing::dates::MJDateTime;
using dpbase::timing::types::SoD;
using dpbase::timing::types::HRTimePointStd;
using dpbase::timing::dates::J2000DateTime;
using dpbase::timing::types::Iso8601Str;
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
using dpslr::geo::types::GeodeticPointDeg;
// Astronomical containers.
using dpslr::astro::types::AltAzPos;
// ILRS related.
using dpslr::ilrs::cpf::CPF;
// SLR preditor related.
using dpslr::slr::predictors::PredictorSlrBase;
using dpslr::slr::predictors::PredictorSlrCPF;
using dpslr::slr::predictors::PredictorSlrPtr;
using dpslr::slr::predictors::PredictorSlrCPFPtr;
using dpslr::slr::utils::PassCalculator;
using dpslr::slr::utils::SpaceObjectPass;

// ---------------------------------------------------------------------------------------------------------------------

int main()
{
    // -------------------- INITIALIZATION -----------------------------------------------------------------------------

    // Initialize LibDegorasSLR.
    DegorasInit::init();

    // -------------------- EXAMPLES CONFIGURATION ---------------------------------------------------------------------

    // SFEL station geodetic position in degrees (north and east > 0) with 8 decimals (~1 mm precision).
    // Altitude in meters with 3 decimals (~1 mm precision).
    Degrees latitude = 36.46525556L;
    Degrees longitude = 353.79469440L;
    Meters alt = 98.177L;

    // SFEL station geocentric coordinates in meters with 3 decimals (~1 mm precison).
    Meters x = 5105473.885L;
    Meters y = -555110.526L;
    Meters z = 3769892.958L;

    // Pass calculator configuration.
    dpbase::math::units::DegreesU min_elevation = 10;
    dpbase::math::units::MillisecondsU time_step = 10000;

    // Configure the CPF input folder.
    std::string current_dir = dpbase::helpers::files::getCurrentDir();
    std::string input_dir(current_dir+"/inputs");

    // Store the local geocentric and geodetic coordinates.
    GeocentricPoint stat_geoc(x,y,z);
    GeodeticPointDeg stat_geod(latitude, longitude, alt);

    // Get band store the example data.
    std::string cpf_path = input_dir + "/" + "39380_cpf_230309_5681.tjr";
    std::vector<SpaceObjectPass> passes;

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

    // Auxiliar variables.
    MJDateTime mjdt_start;
    MJDateTime mjdt_end;

    // Configure the predictor to instant vector for fast calculation.
    predictor_cpf->setPredictionMode(dpslr::slr::predictors::PredictorSlrBase::PredictionMode::INSTANT_VECTOR);

    // Get the available time window for the predictor.
    predictor_cpf->getAvailableTimeWindow(mjdt_start, mjdt_end);

    // Input data.
    const dpslr::ilrs::cpf::CPF& cpf = PredictorSlrBase::specialization<PredictorSlrCPF>(predictor_cpf)->getCPF();
    std::cout<<"CPF INPUT:"<<std::endl;
    std::cout<<"CPF         -> " << cpf.getStandardFilename()<<std::endl;
    std::cout<<"Target      -> " << cpf.getHeader().basicInfo1Header()->target_name<<std::endl;
    std::cout<<"Time Window -> " << mjdt_start.datetime() << " - " << mjdt_end.datetime() << std::endl;
    std::cout<<"-------------------------------------"<<std::endl;

    // Prepare the pass calculator with a minimum of 10 degrees of elevation.
    PassCalculator pass_calculator(predictor_cpf, min_elevation, time_step);

    // Calculate all the passes included in the CPF.
    auto res = pass_calculator.getPasses(mjdt_start, mjdt_end, passes);

    // Check for errors.
    if ( PassCalculator::ResultCode::NOT_ERROR != res)
    {
        std::cout << "Error at passes search. Code is: " << res << std::endl;
        return -1;
    }

    std::cout << "Number of passes found: " << passes.size() << std::endl;

    int i = 1;
    for (const auto &pass : passes)
    {
        // This should be impossible. Paranoid check.
        if (pass.steps.empty())
        {
            std::cout << "Bad pass detected" <<std::endl;
            continue;
        }
        auto start_pass_tp = modifiedJulianDateTimeToTimePoint( pass.steps.front().mjdt);
        auto end_pass_tp = modifiedJulianDateTimeToTimePoint(pass.steps.back().mjdt);

        auto start_pass_time = std::chrono::system_clock::to_time_t(start_pass_tp);
        auto end_pass_time = std::chrono::system_clock::to_time_t(end_pass_tp);

        std::cout << "Pass number " << i
                  << ": Starts at: " << std::ctime(&start_pass_time)
                  << ". Ends at: " << std::ctime(&end_pass_time) << std::endl;

        i++;

    }

    MJDateTime mjd_test;
    SpaceObjectPass pass;

    std::cout << std::endl << " ----------------------------------------------------- " << std::endl;

    std::cout << " Test: IsInsidePass " << std::endl;

    std::cout << " ----------------------------------------------------- " << std::endl;

    mjd_test = {60014, SoD(0.0L)};

    std::cout << "Is MJ datetime " << mjd_test.date() << ", " << mjd_test.sod() << ", inside pass: "
              << pass_calculator.isInsidePass(mjd_test) << std::endl;

    mjd_test = {60014, SoD(27720.0L)};

    std::cout << "Is MJ datetime " << mjd_test.date() << ", " << mjd_test.sod() << ", inside pass: "
              << pass_calculator.isInsidePass(mjd_test) << std::endl;

    std::cout << std::endl << " ----------------------------------------------------- " << std::endl;

    std::cout << " Test: GetNextPass without limits " << std::endl;

    std::cout << " ----------------------------------------------------- " << std::endl;

    // Find next. When the start date is outside of a pass.
    mjd_test = {60014, SoD(0.0L)};

    res = pass_calculator.getNextPass(mjd_test, pass);

    if ( PassCalculator::ResultCode::NOT_ERROR != res)
    {
        std::cout << "Error at next pass search. Code is: " << res << std::endl;
        return -1;
    }

    // This should be impossible. Paranoid check.
    if (pass.steps.empty())
    {
        std::cout << "Bad pass detected" <<std::endl;
        return -1;
    }
    auto start_pass_tp = modifiedJulianDateTimeToTimePoint( pass.steps.front().mjdt);
    auto end_pass_tp = modifiedJulianDateTimeToTimePoint(pass.steps.back().mjdt);

    auto start_pass_time = std::chrono::system_clock::to_time_t(start_pass_tp);
    auto end_pass_time = std::chrono::system_clock::to_time_t(end_pass_tp);

    std::cout << "Pass starts at: " << std::ctime(&start_pass_time)
              << ". Ends at: " << std::ctime(&end_pass_time) << std::endl;

    // Find next pass when the start date is inside a pass.
    mjd_test = {60014, SoD(27720.0L)};

    res = pass_calculator.getNextPass(mjd_test, pass);

    if ( PassCalculator::ResultCode::NOT_ERROR != res)
    {
        std::cout << "Error at next pass search. Code is: " << res << std::endl;
        return -1;
    }

    // This should be impossible. Paranoid check.
    if (pass.steps.empty())
    {
        std::cout << "Bad pass detected" <<std::endl;
        return -1;
    }
    start_pass_tp = modifiedJulianDateTimeToTimePoint( pass.steps.front().mjdt);
    end_pass_tp = modifiedJulianDateTimeToTimePoint(pass.steps.back().mjdt);

    start_pass_time = std::chrono::system_clock::to_time_t(start_pass_tp);
    end_pass_time = std::chrono::system_clock::to_time_t(end_pass_tp);

    std::cout << "Pass starts at: " << std::ctime(&start_pass_time)
              << ". Ends at: " << std::ctime(&end_pass_time) << std::endl;

    std::cout << std::endl << " ----------------------------------------------------- " << std::endl;

    std::cout << " Test: GetNextPass with limits " << std::endl;

    std::cout << " ----------------------------------------------------- " << std::endl;



    // Find next. When the start date is outside of a pass.
    mjd_test = {60014, SoD(0.0L)};

    std::cout << std::endl << "- Start date outside of pass, succesfully found. Pass complete" << std::endl;
    res = pass_calculator.getNextPass(mjd_test, 600, pass, 86400);

    if ( PassCalculator::ResultCode::NOT_ERROR != res)
    {
        std::cout << "Error at next pass search. Code is: " << res << std::endl;
        return -1;
    }

    // This should be impossible. Paranoid check.
    if (pass.steps.empty())
    {
        std::cout << "Bad pass detected" <<std::endl;
        return -1;
    }
    start_pass_tp = modifiedJulianDateTimeToTimePoint( pass.steps.front().mjdt);
    end_pass_tp = modifiedJulianDateTimeToTimePoint(pass.steps.back().mjdt);

    start_pass_time = std::chrono::system_clock::to_time_t(start_pass_tp);
    end_pass_time = std::chrono::system_clock::to_time_t(end_pass_tp);

    std::cout << "Pass starts at: " << std::ctime(&start_pass_time)
              << ". Ends at: " << std::ctime(&end_pass_time)
              << "Start trimmed: " << pass.start_trimmed << ". End trimmed: " << pass.end_trimmed << std::endl;

    // Find next pass when the start date is inside a pass.
    std::cout << std::endl << "- Start date inside of pass, succesfully found. Start trimmed, but end is not. "
              << std::endl;
    mjd_test = {60014, SoD(27720.0L)};

    res = pass_calculator.getNextPass(mjd_test, 600, pass);

    if ( PassCalculator::ResultCode::NOT_ERROR != res)
    {
        std::cout << "Error at next pass search. Code is: " << res << std::endl;
        return -1;
    }

    // This should be impossible. Paranoid check.
    if (pass.steps.empty())
    {
        std::cout << "Bad pass detected" <<std::endl;
        return -1;
    }
    start_pass_tp = modifiedJulianDateTimeToTimePoint( pass.steps.front().mjdt);
    end_pass_tp = modifiedJulianDateTimeToTimePoint(pass.steps.back().mjdt);

    start_pass_time = std::chrono::system_clock::to_time_t(start_pass_tp);
    end_pass_time = std::chrono::system_clock::to_time_t(end_pass_tp);

    std::cout << "Pass starts at: " << std::ctime(&start_pass_time)
              << ". Ends at: " << std::ctime(&end_pass_time)
              << "Start trimmed: " << pass.start_trimmed << ". End trimmed: " << pass.end_trimmed << std::endl;

    // Find next. When the start date is outside of a pass and search limit is reached. No pass found.
    std::cout << std::endl << "- Search limit exceeded. No pass found. " << std::endl;
    mjd_test = {60014, SoD(0.0L)};
    res = pass_calculator.getNextPass(mjd_test, 600, pass, 60);

    if ( PassCalculator::ResultCode::NO_NEXT_PASS_FOUND != res)
    {
        std::cout << "Error. No next pass should have been found. Code is: " << res << std::endl;
        return -1;
    }
    else
        std::cout << "Test passed. No pass found." << std::endl;

    // Find next pass when the start date is outside a pass, and duration limit is lower than pass duration.
    // Start is not trimmed, but end is.
    std::cout << std::endl << "- Start date outside of pass, succesfully found. Duration limit reached. " << std::endl;

    mjd_test = {60014, SoD(0.0L)};
    res = pass_calculator.getNextPass(mjd_test, 60, pass);

    if ( PassCalculator::ResultCode::NOT_ERROR != res)
    {
        std::cout << "Error at next pass search. Code is: " << res << std::endl;
        return -1;
    }

    // This should be impossible. Paranoid check.
    if (pass.steps.empty())
    {
        std::cout << "Bad pass detected" <<std::endl;
        return -1;
    }
    start_pass_tp = modifiedJulianDateTimeToTimePoint( pass.steps.front().mjdt);
    end_pass_tp = modifiedJulianDateTimeToTimePoint(pass.steps.back().mjdt);

    start_pass_time = std::chrono::system_clock::to_time_t(start_pass_tp);
    end_pass_time = std::chrono::system_clock::to_time_t(end_pass_tp);

    std::cout << "Pass starts at: " << std::ctime(&start_pass_time)
              << ". Ends at: " << std::ctime(&end_pass_time)
              << "Start trimmed: " << pass.start_trimmed << ". End trimmed: " << pass.end_trimmed << std::endl;

    // Find next pass when the start date is inside a pass. Duration limit is lower than pass duration.
    // Both start and end are trimmed.
    std::cout << std::endl << "- Start date inside of pass, succesfully found. Duration limit reached. " << std::endl
              << " Start and end trimmed. " << std::endl;

    mjd_test = {60014, SoD(27720.0L)};

    res = pass_calculator.getNextPass(mjd_test, 60, pass);

    if ( PassCalculator::ResultCode::NOT_ERROR != res)
    {
        std::cout << "Error at next pass search. Code is: " << res << std::endl;
        return -1;
    }

    // This should be impossible. Paranoid check.
    if (pass.steps.empty())
    {
        std::cout << "Bad pass detected" <<std::endl;
        return -1;
    }
    start_pass_tp = modifiedJulianDateTimeToTimePoint( pass.steps.front().mjdt);
    end_pass_tp = modifiedJulianDateTimeToTimePoint(pass.steps.back().mjdt);

    start_pass_time = std::chrono::system_clock::to_time_t(start_pass_tp);
    end_pass_time = std::chrono::system_clock::to_time_t(end_pass_tp);

    std::cout << "Pass starts at: " << std::ctime(&start_pass_time)
              << ". Ends at: " << std::ctime(&end_pass_time)
              << "Start trimmed: " << pass.start_trimmed << ". End trimmed: " << pass.end_trimmed << std::endl;

	return 0;
}
