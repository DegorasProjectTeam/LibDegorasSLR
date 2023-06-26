

#include <iostream>
#include <chrono>

int main(int argc, char**argv)
{
    /*
    std::string cpf_path;;

    if (argc < 2)
    {
        std::string exec_path(dpslr::utils::getCurrentDir());
        cpf_path = exec_path + "/test_data/39380_cpf_230309_5681.tjr";
    }
    else
        cpf_path = argv[1];


    CPF cpf(cpf_path, CPF::OpenOptionEnum::ALL_DATA);

    // Check if CPF was opened correctly and it has positions data.
    if(cpf.getData().positionRecords().empty())
    {
        std::cout << "CPF is empty or invalid" << std::endl;
        return -1;
    }

	// SFEL station geodetic coordinates
    long double latitude = 36.46525556L, longitude = 353.79469440L, alt = 98.177L;
	// SFEL station geocentric coordinates
    long double x = 5105473.885L, y = -555110.526L, z = 3769892.958L;

    // Set the station location.
    dpslr::geo::frames::GeodeticPoint<long double> stat_geodetic(latitude, longitude, alt,
                                                                 dpslr::geo::meas::Angle<long double>::Unit::DEGREES);
    dpslr::geo::frames::GeocentricPoint<long double> stat_geocentric(x,y,z);

	// Seek for passes in selected cpf, with a minimum of 9 degrees of elevation.
    dpslr::cpfutils::PassCalculator pass_calculator(cpf, stat_geodetic, stat_geocentric, 9);

    // The getPasses function needs a time interval, given by two pairs mjd-sod (day in modified julian date and
    // fraction of day in seconds). First pair is interval start and the other the interval end.
    // For this example, we use the first and the last time from the CPF.
    // It is also possible to use function timePointToModifiedJulianDate in utils.h. This function gives you mjd,
    // second of day and fraction of second given a TimePoint (it is an alias for std::chrono::time_point)
    const auto& first_pos = cpf.getData().positionRecords().front();
    const auto& last_pos = cpf.getData().positionRecords().back();

    std::vector<dpslr::cpfutils::Pass> passes;
    auto res = pass_calculator.getPasses(first_pos.mjd, first_pos.sod, last_pos.mjd, last_pos.sod, passes);

    if (res != dpslr::cpfutils::PassCalculator::ResultCodes::NOT_ERROR)
    {
        std::cout << "Error at pass search. Code is: " << res << std::endl;
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
        auto start_pass_tp = dpslr::utils::modifiedJulianDatetimeToTimePoint(
                    pass.steps.front().mjd + pass.steps.front().fract_day / 86400.L);
        auto end_pass_tp = dpslr::utils::modifiedJulianDatetimeToTimePoint(
                    pass.steps.back().mjd + pass.steps.back().fract_day / 86400.L);

        auto start_pass_time = std::chrono::system_clock::to_time_t(start_pass_tp);
        auto end_pass_time = std::chrono::system_clock::to_time_t(end_pass_tp);

        std::cout << "Pass number " << i
                  << ": Starts at: " << std::ctime(&start_pass_time)
                  << ". Ends at: " << std::ctime(&end_pass_time) << std::endl;

        i++;

    }
    */
	return 0;
}
