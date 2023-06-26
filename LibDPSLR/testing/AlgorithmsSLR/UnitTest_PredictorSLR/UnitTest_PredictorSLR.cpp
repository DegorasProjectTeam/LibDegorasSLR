#include <LibDPSLR/AlgorithmsSLR/Utilities>
#include <LibDPSLR/FormatsILRS/CPFFormat>

#include <iostream>
#include <chrono>
#include <thread>

// Namespace of the LibDPSLR library.
using namespace dpslr;

// SFEL station geodetic and geocentric coordinates (degrees).
long double lat = 36.46525556L, lon = 353.79469440L, alt = 98.177L;
long double x = 5105473.885L, y = -555110.526L, z = 3769892.958L;

// Fixed CPF input data.
static const std::string kInputPath = "test_data_input/";
static const std::string kFixedTestCPF_1 = "lares_cpf_230529_14901.sgf";


int main(int argc, char**argv)
{
    // Get the path to the CPF file.
    std::string cpf_path = kInputPath + kFixedTestCPF_1;

    // Set the station location.
    dpslr::geo::frames::GeodeticPoint<long double> stat_geodetic(latitude, longitude, alt,
                                                                 dpslr::geo::meas::Angle<long double>::Unit::DEGREES);
    dpslr::geo::frames::GeocentricPoint<long double> stat_geocentric(x,y,z);



    ilrs::cpf::CPF cpf(cpf_path, CPF::OpenOptionEnum::ALL_DATA);

    // Check if CPF was opened correctly and it has positions data.
    if(cpf.getData().positionRecords().empty())
    {
        std::cout << "CPF is empty or invalid" << std::endl;
        return -1;
    }





	// Seek for passes in selected cpf, with a minimum of 9 degrees of elevation.
    dpslr::cpfutils::PassCalculator pass_calculator(cpf, stat_geodetic, stat_geocentric, 9);

    dpslr::cpfutils::CPFInterpolator interpolator{cpf, stat_geodetic, stat_geocentric};

    if (interpolator.empty())
    {
        std::cout << "Interpolator could not be initialized properly" << std::endl;
        return -1;
    }

    // These variables represent the time. One is the modified julian day and the others are the second of day and its
    // fractional part.

    // We will use date at 2023/05/29 - 12:00 p.m. for the example cpf
    long long mjd = 60093;
    unsigned int sod = 43200;
    double sod_fract = 0;
    long double sod_with_fract;
    dpslr::cpfutils::CPFInterpolator::InterpolationResult interp_result;

    sod_with_fract = sod + sod_fract;

    bool not_error = true;

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
	
	return 0;
}
