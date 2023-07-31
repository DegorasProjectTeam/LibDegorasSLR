#include <LibDPSLR/AlgorithmsSLR/Utilities>
#include <LibDPSLR/Geo/common/geo_types.h>
#include <LibDPSLR/FormatsILRS/CPFFormat>
#include <LibDPSLR/Mathematics/units.h>

#include <iostream>
#include <chrono>
#include <thread>

#define TEST_INPUT_PATH _TEST_INPUT_PATH

// Namespace of the LibDPSLR library.
using namespace dpslr::algoslr::utils;
using namespace dpslr::geo::common;
using namespace dpslr::math::units;

// SFEL station geodetic and geocentric coordinates (degrees).
static long double lat = 36.46525556L, lon = 353.79469440L, alt = 98.177L;
static long double x = 5105473.885L, y = -555110.526L, z = 3769892.958L;

// Fixed CPF input data.
static std::string kInputPath = TEST_INPUT_PATH + std::string("/test_data_input/");
static std::string kFixedTestCPF_1 = "lares_cpf_230529_14901.sgf";


int main(int, char**)
{
    // Get the path to the CPF file.
    std::string cpf_path = kInputPath + kFixedTestCPF_1;

    // Set the station location.
    GeodeticPoint<long double> stat_geodetic(lat, lon, alt, Angle<long double>::Unit::DEGREES);
    GeocentricPoint<long double> stat_geocentric(x,y,z);

    // Prepare the CPF data.
    CPF cpf(cpf_path, CPF::OpenOptionEnum::ALL_DATA);

    // Check if CPF was opened correctly and it has positions data.
    if(cpf.getData().positionRecords().empty())
    {
        std::cout << "CPF is empty or invalid! Path is: " << cpf_path << std::endl;
        return -1;
    }

    // Instantiate the predictor.
    PredictorSLR predictor(stat_geodetic, stat_geocentric);

    // Configure the predictor.
    predictor.setCPF(cpf);
    predictor.setPredictionMode(PredictorSLR::PredictionMode::OUTBOUND_VECTOR);
    predictor.setInterpolFunction(PredictorSLR::InterpolFunction::LAGRANGE_16);
    predictor.enableCorrections(false);

    // Result containers for the predictor.
    PredictorSLR::PredictionResult result;
    PredictorSLR::PredictionError error;

    // These variables represent the time. One is the modified julian day and the others are the second of day and its
    // fractional part.

    // We will use date at 2023/05/29 - 12:00 p.m. for the example cpf
    long long mjd = 60093;
    unsigned int sod = 43200;
    double sod_fract = 0;
    long double sod_with_fract;

    sod_with_fract = sod + sod_fract;

    error = predictor.predict(mjd, sod_with_fract, result);

    if(error == PredictorSLR::PredictionError::NO_ERROR)
    {
        std::cout<<"Done"<<std::endl;
        std::cout<<result.toJsonStr()<<std::endl;
    }
    else
    {
        std::cout<<"ERROR"<<std::endl;
        std::cout<<static_cast<int>(error)<<std::endl;
        std::cout<<PredictorSLR::PredictorErrorStr[static_cast<std::size_t>(error)]<<std::endl;
    }

    /*
    while (not_error )
    {
        auto error = interpolator.interpolate(mjd, sod_with_fract, interp_result);

        if (error != dpslr::cpfutils::CPFInterpolator::NOT_ERROR &&
            error != dpslr::cpfutils::CPFInterpolator::ErrorEnum::INTERPOLATION_NOT_IN_THE_MIDDLE)
            not_error = false;

        else
        {
            std::cout << "For object with Norad: " << cpf.getHeader().basicInfo2Header()->norad
                      << ", at mjd " << mjd << std::setprecision(14) <<  ", sec: " << sod_with_fract <<  ", azimuth (deg) is: "
                      << interp_result.azimuth_out << ", elevation (deg) is: " << interp_result.elevation_out << std::endl;
        }

        sod_with_fract += 1.L;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    */

	return 0;
}
