#include <LibDegorasSLR/AlgorithmsSLR/utils/tracking_slr.h>

int main ()
{
    dpslr::ilrs::cpf::CPF cpf("", dpslr::ilrs::cpf::CPF::OpenOptionEnum::ALL_DATA);
    // SFEL station geodetic coordinates
    long double latitude = 36.46525556L, longitude = 353.79469440L, alt = 98.177L;
    // SFEL station geocentric coordinates
    long double x = 5105473.885L, y = -555110.526L, z = 3769892.958L;
    dpslr::geo::common::GeocentricPoint<long double> stat_geocentric(x,y,z);
    dpslr::geo::common::GeodeticPoint<long double> stat_geodetic(latitude, longitude, alt);
    dpslr::algoslr::utils::PredictorSLR predictor(cpf, stat_geodetic, stat_geocentric);

    dpslr::algoslr::utils::TrackingSLR tracking(10.L, 0, 0., std::move(predictor));


}
