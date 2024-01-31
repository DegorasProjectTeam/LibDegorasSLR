#include <LibDegorasSLR/AlgorithmsSLR/utils/tracking_slr.h>
#include <LibDegorasSLR/Timing/time_utils.h>
#include <LibDegorasSLR/Mathematics/units.h>

#include <iostream>

int main ()
{
    using dpslr::timing::common::MJDType;
    using dpslr::timing::common::MJDtType;
    using dpslr::timing::common::SoDType;

    std::string cpf_dir("C:/0-SALARA_PROJECT/SP_DataFiles/SP_CPF/SP_CurrentCPF/");
    std::string cpf_name("41240_cpf_240128_02801.hts");
    dpslr::ilrs::cpf::CPF cpf(cpf_dir + cpf_name, dpslr::ilrs::cpf::CPF::OpenOptionEnum::ALL_DATA);
    // SFEL station geodetic coordinates
    long double latitude = 36.46525556L, longitude = 353.79469440L, alt = 98.177L;
    // SFEL station geocentric coordinates
    long double x = 5105473.885L, y = -555110.526L, z = 3769892.958L;
    dpslr::geo::common::GeocentricPoint<long double> stat_geocentric(x,y,z);
    dpslr::geo::common::GeodeticPoint<long double> stat_geodetic(latitude, longitude, alt,
                                                                 dpslr::math::units::Angle<long double>::Unit::DEGREES);
    dpslr::algoslr::utils::PredictorSLR predictor(cpf, stat_geodetic, stat_geocentric);

    MJDType mjd_start_look = 60340;
    SoDType sod_start_look = 42000;
    dpslr::algoslr::utils::TrackingSLR tracking(10.L, mjd_start_look, sod_start_look, std::move(predictor));
    dpslr::astro::PredictorSun<long double> sun_pred(stat_geodetic);
    std::vector<dpslr::astro::SunPosition<long double>> sun_pos;

    if (!tracking.isValid())
    {
        std::cerr << "There is no valid tracking." << std::endl;
        return -1;
    }

    if (tracking.getSunOverlapping())
    {
        std::cout << "There is sun overlapping" << std::endl;

        if (tracking.getSunAtStart())
            std::cout << "Sun overlapping at the beginning" << std::endl;

        if (tracking.getSunAtEnd())
            std::cout << "Sun overlapping at the end" << std::endl;
    }

    MJDType mjd_start, mjd_end;
    unsigned int hour, min, sec, ns;
    SoDType sod_start, sod_end;
    tracking.getTrackingStart(mjd_start, sod_start);

    dpslr::timing::nsDayTohhmmssns(sod_start * 1e9, hour, min, sec, ns);

    std::cout << "Pass starts at " << hour << ":" << min << ":" << sec << std::endl;

    tracking.getTrackingEnd(mjd_end, sod_end);

    dpslr::timing::nsDayTohhmmssns(sod_end * 1e9, hour, min, sec, ns);

    std::cout << "Pass ends at " << hour << ":" << min << ":" << sec << std::endl;

    MJDType mjd = mjd_start;
    SoDType sod = sod_start;
    long double j2000;
    std::vector<dpslr::algoslr::utils::TrackingSLR::Position> pos;

    while (mjd < mjd_end || sod < sod_end)
    {
        pos.push_back({});
        auto error = tracking.getPosition(mjd, sod, pos.back());


        if (error == dpslr::algoslr::utils::TrackingSLR::NOT_ERROR)
        {
            // DO NOTHING
        }
        else if (error == dpslr::algoslr::utils::TrackingSLR::AVOIDING_SUN)
        {
            j2000 = dpslr::timing::mjdToJ2000Datetime(mjd, sod);
            sun_pos.push_back(sun_pred.fastPredict(j2000, false));

            auto az_diff = pos.back().az - sun_pos.back().azimuth;
            auto el_diff = pos.back().el - sun_pos.back().elevation;
            std::cout << "Tracking pos is: " << pos.back().az << ", " << pos.back().el
                      << ". Sun is at: " << sun_pos.back().azimuth << ", " << sun_pos.back().elevation
                      << ". Distance is: " << std::sqrt(az_diff * az_diff + el_diff * el_diff) << std::endl;
        }
        else
        {
            std::cout << "Error at getting position " << error;
            return -1;
        }

        sod += 0.5L;
        if (sod > 86400.L)
        {
            sod -= 86400.L;
            mjd++;
        }
    }

    // If there was no sun overlapping, store the sun position in 5 s interval
    if (sun_pos.empty())
    {
        MJDType mjd = mjd_start;
        SoDType sod = sod_start;

        while (mjd < mjd_end || sod < sod_end)
        {
            j2000 = dpslr::timing::mjdToJ2000Datetime(mjd, sod);
            sun_pos.push_back(sun_pred.fastPredict(j2000, false));
            sod += 5.L;
            if (sod > 86400.L)
            {
                sod -= 86400.L;
                mjd++;
            }
        }

    }


    std::ofstream file_pos("./tracking.txt", std::ios_base::out);
    for (const auto &p : pos)
    {
        file_pos << p.az << ", " << p.el << std::endl;
    }
    std::ofstream file_pos_sun("./pos_sun.txt", std::ios_base::out);
    for (const auto &p : sun_pos)
    {
        file_pos_sun << p.azimuth << ", " << p.elevation << std::endl;
    }

    return 0;

}
