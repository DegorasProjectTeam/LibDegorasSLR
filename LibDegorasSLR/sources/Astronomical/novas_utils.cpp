
// C++ INCLUDES
//======================================================================================================================
// =====================================================================================================================

// LIBNOVASCPP INCLUDES
//======================================================================================================================
#include <LibNovasCpp/novascpp.h>
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Astronomical/novas_utils.h"
#include "LibDegorasSLR/Timing/time_utils.h"
#include "LibDegorasSLR/Timing/time_constants.h"
// =====================================================================================================================

// NAMESPACES
// =====================================================================================================================
using novas::topo_star;
using novas::equ2hor;
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace astro{
namespace novas{
// =====================================================================================================================



on_surface makeOnSurface(const geo::types::SurfaceLocation<double> &loc)
{
    return makeOnSurface(loc.geodetic, loc.meteo);
}

on_surface makeOnSurface(const geo::types::GeodeticPoint<double> &geod, const geo::types::MeteoData &meteo)
{
    // TODO CHECK GEDOTIC UNITS AND METEO

    on_surface geo_loc;
    make_on_surface(geod.lat, geod.lon, geod.alt, meteo.temperature, meteo.pressure, &geo_loc);
    return geo_loc;
}

LIBDPSLR_EXPORT int getStarAltAzPosition(const astro::types::Star &star,
                                         const geo::types::SurfaceLocation<double> &loc,
                                         const timing::types::JDateTime &jdt,
                                         types::AltAzPosition &pos,
                                         int leap_secs,
                                         double ut1_utc_diff)
{
    int error;
    auto surface = makeOnSurface(loc);
    cat_entry entry;
    error = makeCatEntry(star, entry);

    // If there was an error at catalog entry creation, return
    if (error)
    {
        return error;
    }

    // Calculate timestamps
    double leap_secs_d = static_cast<double>(leap_secs);
    double jd_utc = jdt;
    double jd_tt = jd_utc + (leap_secs_d + 32.184) / timing::kSecsPerDay;  // TT = UTC + incrementAT + 32.184
    double jd_ut1 = jd_utc + ut1_utc_diff / timing::kSecsPerDay;
    double delta_t = 32.184 + leap_secs_d - ut1_utc_diff;                               // TT - UT1 in seconds.

    // Variable declarations
    double ra_topo;                                                                   // Topocentric RA
    double dec_topo;                                                                  // Topocentric dec
    double zd;                                                                        // Zenith angle of star
    double el;                                                                        // Elevation angle of star
    double az;                                                                        // Azimuth angle of star
    double ra_topo_ref;                                                               // Topocentric RA with refraction
    double dec_topo_ref;                                                              // Topocentric Dec with refraction
    // TODO: configurable accuracy mode and poles?
    const int accuracy = 1;                                                           // Accuracy configuration.
    const double x_pole = 0;                                                          // X pole
    const double y_pole = 0;                                                          // Y pole

    // Get topocentric place of star
    if (!(error = topo_star(jd_tt, delta_t, &entry, &surface, 1, &ra_topo, &dec_topo)))
    {
        // If there was no error on previous function, transform the coordinates to az and zenith
        equ2hor(jd_ut1, delta_t, 1, x_pole, y_pole, &surface, ra_topo, dec_topo, 2,
                &zd, &az, &ra_topo_ref, &dec_topo_ref);
        el = 90. - zd;
        pos = types::AltAzPosition(az, el);
    }

    return error;
}


int getStarAltAzPosition(const types::Star &star,
                         const geo::types::SurfaceLocation<double> &loc,
                         const timing::types::HRTimePointStd &tp,
                         types::AltAzPosition &pos,
                         int leap_secs,
                         double ut1_utc_diff)
{
    timing::types::JDate jd;
    timing::types::DayFraction df;
    timing::timePointToJulianDate(tp, jd, df);

    return getStarAltAzPosition(star, loc, jd + df, pos, leap_secs, ut1_utc_diff);
}

int makeCatEntry(const types::Star &star, cat_entry &entry)
{
    std::vector<char>star_name(star.star_name.begin(), star.star_name.end());
    star_name.push_back('\0');
    std::vector<char>cat_name(star.catalog_name.begin(), star.catalog_name.end());
    cat_name.push_back('\0');
    return make_cat_entry(star_name.data(), cat_name.data(), star.catalog_num, star.ra,
                          star.dec, star.pm_ra, star.pm_dec, star.parallax, star.rad_vel,
                          &entry);
}


}}} // END NAMESPACES.
// =====================================================================================================================
