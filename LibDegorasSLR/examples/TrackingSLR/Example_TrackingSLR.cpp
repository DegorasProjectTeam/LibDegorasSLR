#include <LibDegorasSLR/AlgorithmsSLR/utils/tracking_slr.h>
#include <LibDegorasSLR/Timing/time_utils.h>
#include <LibDegorasSLR/Mathematics/units.h>

#include <iostream>

int main ()
{
    // Namespaces.
    using namespace dpslr;

    // Configure the CPF folder and example file.
    std::string cpf_dir("C:/0-SALARA_PROJECT/SP_DataFiles/SP_CPF/SP_CurrentCPF/");
    std::string cpf_name("41240_cpf_240128_02801.hts");


    // SFEL station geodetic coordinates.
    long double latitude = 36.46525556L, longitude = 353.79469440L, alt = 98.177L;

    // SFEL station geocentric coordinates
    long double x = 5105473.885L, y = -555110.526L, z = 3769892.958L;


    // Store the local coordinates.
    geo::common::GeocentricPoint<long double> stat_geocentric(x,y,z);
    geo::common::GeodeticPoint<long double> stat_geodetic(
                                                latitude, longitude, alt,
                                                math::units::Angle<long double>::Unit::DEGREES);

    // Open the CPF file.
    ilrs::cpf::CPF cpf(cpf_dir + cpf_name, dpslr::ilrs::cpf::CPF::OpenOptionEnum::ALL_DATA);

    // Configure the SLR predictor.
    dpslr::algoslr::utils::PredictorSLR predictor(cpf, stat_geodetic, stat_geocentric);
    predictor.setPredictionMode(dpslr::algoslr::utils::PredictorSLR::PredictionMode::INSTANT_VECTOR);

    timing::MJDate mjd_start = 60340;
    timing::SoD sod_start = 42140;
    timing::MJDate mjd_end = 60340;
    timing::SoD sod_end = 43150;
    dpslr::algoslr::utils::TrackingSLR tracking(8.L, mjd_start, sod_start, mjd_end, sod_end, std::move(predictor));
    dpslr::astro::PredictorSun<long double> sun_pred(stat_geodetic);
    std::vector<dpslr::astro::SunPosition<long double>> sun_pos;

    if (!tracking.isValid())
    {
        std::cerr << "There is no valid tracking." << std::endl;
        return -1;
    }

    if (tracking.isSunOverlapping())
    {
        std::cout << "There is sun overlapping" << std::endl;

        if (tracking.isSunAtStart())
            std::cout << "Sun overlapping at the beginning" << std::endl;
        
        if (tracking.isSunAtEnd())
            std::cout << "Sun overlapping at the end" << std::endl;
    }

    // Get new start and end
    tracking.getTrackingStart(mjd_start, sod_start);
    tracking.getTrackingEnd(mjd_end, sod_end);

    timing::MJDate mjd = mjd_start;
    timing::SoD sod = sod_start;
    long double j2000;
    std::vector<dpslr::algoslr::utils::TrackingSLR::TrackingResult> res;
    dpslr::algoslr::utils::TrackingSLR::TrackingResult pos;

    while (mjd < mjd_end || sod < sod_end)
    {

        auto error = tracking.predictTrackingPosition(mjd, sod, pos);


        if (error == dpslr::algoslr::utils::TrackingSLR::INSIDE_SUN)
        {
            // DO NOTHING
        }
        else if (error == dpslr::algoslr::utils::TrackingSLR::OUTSIDE_SUN)
        {
            res.push_back(std::move(pos));
            pos = {};
        }
        else if (error == dpslr::algoslr::utils::TrackingSLR::AVOIDING_SUN)
        {
            j2000 = dpslr::timing::mjdToJ2000Datetime(mjd, sod);
            sun_pos.push_back(sun_pred.fastPredict(j2000, false));

            res.push_back(std::move(pos));
            pos = {};

        }
        else
        {
            std::cout << "Error at getting position " << error;
            return -1;
        }

        sod += 1.L;
        if (sod > 86400.L)
        {
            sod -= 86400.L;
            mjd++;
        }
    }

    // If there was no sun overlapping, store the sun position in 10 s interval
    if (sun_pos.empty())
    {
        timing::MJDate mjd = mjd_start;
        timing::SoD sod = sod_start;

        while (mjd < mjd_end || sod < sod_end)
        {
            j2000 = dpslr::timing::mjdToJ2000Datetime(mjd, sod);
            // Only store if sun has positive elvation
            auto p = sun_pred.fastPredict(j2000, false);
            if (p.elevation > 0)
                sun_pos.push_back(std::move(p));
            sod += 10.L;
            if (sod > 86400.L)
            {
                sod -= 86400.L;
                mjd++;
            }
        }

    }


    std::ofstream file_pos("./tracking.txt", std::ios_base::out);
    for (const auto &r : res)
    {
        file_pos << r.tracking_position->az << ", " << r.tracking_position->el << std::endl;
    }
    std::ofstream file_pos_sun("./pos_sun.txt", std::ios_base::out);
    for (const auto &p : sun_pos)
    {
        file_pos_sun << p.azimuth << ", " << p.elevation << std::endl;
    }

    return 0;

}
