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
#include <LibDegorasSLR/Modules/Timing>
// =====================================================================================================================


// ---------------------------------------------------------------------------------------------------------------------
// LibDegorasSLR types used in this example.
//
// Initialization.
using dpslr::DegorasInit;
// Time tipes and conversions.
using dpslr::timing::dates::MJDateTime;
using dpslr::timing::types::SoD;
using dpslr::timing::types::HRTimePointStd;
using dpslr::timing::dates::J2000DateTime;
using dpslr::timing::types::Iso8601Str;
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
using dpslr::slr::utils::Pass;

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

    // Configure the CPF input folder.
    std::string current_dir = dpslr::helpers::files::getCurrentDir();
    std::string input_dir(current_dir+"/inputs");

    // Store the local geocentric and geodetic coordinates.
    GeocentricPoint stat_geoc(x,y,z);
    GeodeticPointDeg stat_geod(latitude, longitude, alt);


    // Get band store the example data.
    std::string cpf_path = input_dir + "/" + "39380_cpf_230309_5681.tjr";
    std::vector<Pass> passes;

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


    // Get start and end time from predictor.
    MJDateTime mjdt_start;
    MJDateTime mjdt_end;

    predictor_cpf->getTimeWindow(mjdt_start, mjdt_end);

    // Avoid predictions not in the middle. At the beginning and at the end of a CPF you cannot interpolate
    // the position because there is not enough information for Lagrange interpolation.
    mjdt_start += Seconds(960.L);
    mjdt_end -= Seconds(960.L);

	// Seek for passes in selected cpf, with a minimum of 9 degrees of elevation.
    PassCalculator pass_calculator(predictor_cpf, 9);

    auto res = pass_calculator.getPasses(mjdt_start, mjdt_end, passes);

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
        auto start_pass_tp = modifiedJulianDateTimeToTimePoint( pass.steps.front().mjd);
        auto end_pass_tp = modifiedJulianDateTimeToTimePoint(pass.steps.back().mjd);

        auto start_pass_time = std::chrono::system_clock::to_time_t(start_pass_tp);
        auto end_pass_time = std::chrono::system_clock::to_time_t(end_pass_tp);

        std::cout << "Pass number " << i
                  << ": Starts at: " << std::ctime(&start_pass_time)
                  << ". Ends at: " << std::ctime(&end_pass_time) << std::endl;

        i++;

    }

    MJDateTime mjd_test(60014, SoD(0.0L));
    std::cout << "Is MJ datetime " << mjd_test.date() << ", " << mjd_test.sod() << ", inside pass: "
              << pass_calculator.isInsidePass(mjd_test) << std::endl;
    Pass pass;

    // Find next. When the start date is outside of a pass.
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
    auto start_pass_tp = modifiedJulianDateTimeToTimePoint( pass.steps.front().mjd);
    auto end_pass_tp = modifiedJulianDateTimeToTimePoint(pass.steps.back().mjd);

    auto start_pass_time = std::chrono::system_clock::to_time_t(start_pass_tp);
    auto end_pass_time = std::chrono::system_clock::to_time_t(end_pass_tp);

    std::cout << "Pass starts at: " << std::ctime(&start_pass_time)
              << ". Ends at: " << std::ctime(&end_pass_time) << std::endl;


    // Find next pass when the start date is inside a pass.
    mjd_test = {60014, SoD(27720.0L)};

    std::cout << "Is MJ datetime " << mjd_test.date() << ", " << mjd_test.sod() << ", inside pass: "
              << pass_calculator.isInsidePass(mjd_test) << std::endl;

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
    start_pass_tp = modifiedJulianDateTimeToTimePoint( pass.steps.front().mjd);
    end_pass_tp = modifiedJulianDateTimeToTimePoint(pass.steps.back().mjd);

    start_pass_time = std::chrono::system_clock::to_time_t(start_pass_tp);
    end_pass_time = std::chrono::system_clock::to_time_t(end_pass_tp);

    std::cout << "Pass starts at: " << std::ctime(&start_pass_time)
              << ". Ends at: " << std::ctime(&end_pass_time) << std::endl;



	return 0;
}
