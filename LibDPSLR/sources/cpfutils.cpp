/***********************************************************************************************************************
 * Copyright 2023 Degoras Project Team
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they will be approved by the
 * European Commission - subsequent versions of the EUPL (the "Licence");
 *
 * You may not use this work except in compliance with the Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the Licence is distributed on
 * an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the Licence for the
 * specific language governing permissions and limitations under the Licence.
 **********************************************************************************************************************/

#include "LibDPSLR/cpfutils.h"
#include "LibDPSLR/math_operators.h"

namespace dpslr {
namespace cpfutils {

const std::array<std::string, 10> CPFInterpolator::ErrorEnumStrings =
{
    "No error",
    "CPF not found",
    "CPF header load failed",
    "CPF data load failed",
    "Interpolation not in the middle",
    "Interpolation out of bounds",
    "Interpolation data size mismatch",
    "Unknown interpolator",
    "No position records",
    "Other error"
};

CPFInterpolator::CPFInterpolator(const CPF &cpf,
                                 const dpslr::geo::frames::GeodeticPoint<long double> &stat_geodetic,
                                 const dpslr::geo::frames::GeocentricPoint<long double> &stat_geocentric) :
    stat_geodetic(stat_geodetic),
    stat_geocentric(stat_geocentric)
{
    // TODO: improve error handling
    if (cpf.empty() || cpf.getData().positionRecords().empty())
        return;

    this->mjd_orig = cpf.getData().positionRecords().front().mjd;
    this->sod_orig = cpf.getData().positionRecords().front().sod;
    this->mjd_end = cpf.getData().positionRecords().back().mjd;
    this->sod_end = cpf.getData().positionRecords().back().sod;

    // Get position records and position times for interpolation calculations
    for (const auto& pos_record : cpf.getData().positionRecords())
    {
        auto time_tag = pos_record.sod - this->sod_orig + (pos_record.mjd - this->mjd_orig) * 86400.L;
        position_data.push_back_row(pos_record.geocentric_pos);
        position_times.push_back(time_tag);
    }

    // Rotation matrices.
    dpslr::math::Matrix<long double> rot_long, rot_lat, rot_long_pi;

    // Store latitude and longitude in radians.
    this->stat_geodetic.convert(dpslr::geo::meas::Angle<long double>::Unit::RADIANS,
                                dpslr::geo::meas::Distance<long double>::Unit::METRES);
    long double station_lon = this->stat_geodetic.lon;
    long double station_lat = this->stat_geodetic.lat;

    // Identity matrix.
    this->rotation_matrix = dpslr::math::Matrix<long double>::I(3);

    // Computation of rotation matrices
    // rot_long around longitude, rot_lat around pi/2-latitude, rot_long_pi around pi
    dpslr::math::euclid3DRotMat(3, station_lon, rot_long);
    dpslr::math::euclid3DRotMat(2, static_cast<long double>(dpslr::math::pi/2) - station_lat, rot_lat);
    dpslr::math::euclid3DRotMat(3, static_cast<long double>(dpslr::math::pi), rot_long_pi);
    this->rotation_matrix *= rot_long * rot_lat * rot_long_pi;

    // Get CoM offset correction, if any.
    if (cpf.getHeader().basicInfo2Header() && cpf.getHeader().basicInfo2Header()->com_applied &&
            cpf.getHeader().coMCorrectionHeader())
        this->com_offset = cpf.getHeader().coMCorrectionHeader()->com_correction;
}

CPFInterpolator::InterpolationError CPFInterpolator::interpolate(long double mjt, InterpolationResult &interp_res,
                                                        InterpolationMode mode, InterpolationFunction function) const
{
    // Time conversions.
    long double mjd;
    long double second = std::modf(mjt , &mjd) * 86400.0L;

    // Call with the datetime value splitted in the day and second in that day.
    return this->interpolate(static_cast<int>(mjd), second, interp_res, mode, function);
}

void CPFInterpolator::getStationLocation(
        dpslr::geo::frames::GeodeticPoint<long double> &geodetic,
        dpslr::geo::frames::GeocentricPoint<long double> &geocentric) const
{
    geodetic = this->stat_geodetic;
    geocentric = this->stat_geocentric;
}

bool CPFInterpolator::empty() const
{
    return position_times.empty();
}

bool CPFInterpolator::ready() const
{
    return !position_times.empty();
}

void CPFInterpolator::getAvailableTimeInterval(int &mjd_start, long double &fract_start,
                                               int &mjd_end, long double &fract_end) const
{
    if (empty())
    {
        mjd_start = 0;
        fract_start = 0.L;
        mjd_end = 0;
        fract_end = 0.L;
    }
    else
    {
        mjd_start = this->mjd_orig;
        fract_start = this->sod_orig;
        mjd_end = this->mjd_end;
        fract_end = this->sod_end;
    }
}

CPFInterpolator::InterpolationError CPFInterpolator::convertInterpError(math::LagrangeResult error) const
{
    CPFInterpolator::InterpolationError cpf_error;
    switch (error)
    {
    case dpslr::math::LagrangeResult::NOT_ERROR :
        cpf_error = CPFInterpolator::NOT_ERROR; break;
    case dpslr::math::LagrangeResult::NOT_IN_THE_MIDDLE :
        cpf_error = CPFInterpolator::INTERPOLATION_NOT_IN_THE_MIDDLE; break;
    case dpslr::math::LagrangeResult::X_OUT_OF_BOUNDS :
        cpf_error = CPFInterpolator::X_INTERPOLATED_OUT_OF_BOUNDS; break;
    case dpslr::math::LagrangeResult::DATA_SIZE_MISMATCH :
        cpf_error = CPFInterpolator::INTERPOLATION_DATA_SIZE_MISMATCH; break;
    }
    return cpf_error;
}

CPFInterpolator::InterpolationError CPFInterpolator::interpolate(int mjd, long double second,
                                                        InterpolationResult &interp_res,
                                                        InterpolationMode mode, InterpolationFunction function) const
{
    using namespace dpslr::math_operators;

    // Interpolation is not possible if there are no position records
    if (this->position_times.empty())
        return CPFInterpolator::NO_POS_RECORDS;


    // Variables and containers.
    long double x_interp;
    std::vector<long double> y_interp;
    long double dist_to_object, elevation, azimuth, diff_azim, diff_elev;
    long double azi_out, elev_out, time_out, dsidt, tb = 0.0L, distout = 0.0L;
    std::vector<long double> topocentric_position, topocentric_outbound;
    std::vector<long double> stat_xyz(this->stat_geocentric.store<std::vector<long double>>());
    dpslr::math::Matrix<long double> station_rotated, station_rotation, topocentric_local_pos, topocentric_out_local;
    dpslr::math::LagrangeResult interp_error = dpslr::math::LagrangeResult::NOT_ERROR;

    // Generate the relative time.
    int day_relative = mjd - this->mjd_orig;
    x_interp = (day_relative*86400) + second - this->sod_orig;

    // Check if the relative time is negative.
    if(x_interp < 0 || x_interp > this->position_times.back())
    {
        interp_res.error = CPFInterpolator::X_INTERPOLATED_OUT_OF_BOUNDS;
        return CPFInterpolator::X_INTERPOLATED_OUT_OF_BOUNDS;
    }

    // Store the interpolation datetime data.
    interp_res.mjd = mjd;
    interp_res.mjdt = mjd + second/86400.L;
    interp_res.sec_of_day = second;

    if(function == CPFInterpolator::LAGRANGE_9)
    {
        interp_error = dpslr::math::lagrangeInterp(this->position_times, this->position_data, 9, x_interp, y_interp);
    }
    else
    {
        interp_res.error = CPFInterpolator::UNKNOWN_INTERPOLATOR;
        return CPFInterpolator::UNKNOWN_INTERPOLATOR;
    }

    // Return if errors.
    if (dpslr::math::LagrangeResult::NOT_ERROR != interp_error)
    {
        interp_res.error = this->convertInterpError(interp_error);
        return this->convertInterpError(interp_error);
    }

    // Topocentric vector station/object both at transmit time
    topocentric_position = y_interp - stat_xyz;

    // Instant distance from station to object at transmit time
    dist_to_object=sqrtl(topocentric_position[0]*topocentric_position[0] +
                        topocentric_position[1]*topocentric_position[1] +
                        topocentric_position[2]*topocentric_position[2]);

    // Topocentric vector in local system
    topocentric_local_pos.push_back_row(topocentric_position);
    topocentric_local_pos *= rotation_matrix;

    // Azimuth and elevation (degrees)
    elevation=atanl(topocentric_local_pos[0][2]/sqrtl(topocentric_local_pos[0][0]*topocentric_local_pos[0][0]+
            topocentric_local_pos[0][1]*topocentric_local_pos[0][1]))*180/dpslr::math::pi;
    // TODO: Check 90 degrees elevation case (pag 263 fundamental of astrodinamic and applications (Vallado).
    // Fix, but never should be reached.
    if(dpslr::math::compareFloating(elevation, 90.0L) == 1)
        elevation+=0.01L;

    azimuth=atan2l(-topocentric_local_pos[0][1],topocentric_local_pos[0][0])*180/dpslr::math::pi;
    if(azimuth < 0.L)
        azimuth+=360.L;

    // Instant vector only mode (not used in range gate generator)
    if (mode == InterpolationMode::INSTANT_VECTOR)
    {
        // One-way range
        interp_res.range = dist_to_object;
        if(this->com_offset)
            interp_res.range -= *this->com_offset;
        // Round trip flight time (sec)
        interp_res.tof_2w = 2*interp_res.range/dpslr::math::c;
        // Direction
        interp_res.azimuth = azimuth;
        interp_res.elevation = elevation;
        // Difference is 0, since no point is behind.
        interp_res.diff_azimuth = 0;
        interp_res.diff_elevation = 0;
        // Store geocentric interpolated position
        std::copy(y_interp.begin(), y_interp.end(), interp_res.geocentric.begin());
        // Return.
        interp_res.error = CPFInterpolator::NOT_ERROR;
        return CPFInterpolator::NOT_ERROR;
    }

    // Store geocentric for calculate station rotated.
    station_rotated.push_back_row(stat_xyz);
    // Calculate the time out.
    time_out = dist_to_object/dpslr::math::c;

    // Iteration for laser pulse two-way trip (2 iteration).
    for (unsigned int i=0; i<2; i++)
    {
        //  Bounce time.
        tb = x_interp + time_out;

        // Interpolate geocentric position of the object for bounce time tb
        interp_error =  dpslr::math::lagrangeInterp(position_times, position_data, 9, tb, y_interp);

        if ( dpslr::math::LagrangeResult::NOT_ERROR != interp_error )
            return this->convertInterpError(interp_error);

        // Topocentric outbound vector
        topocentric_outbound = y_interp - station_rotated[0];

        //  Distance from station (tt) to object (tb)
        distout=sqrtl(topocentric_outbound[0] * topocentric_outbound[0] +
                     topocentric_outbound[1] * topocentric_outbound[1] +
                     topocentric_outbound[2] * topocentric_outbound[2] );

        // Outbound flight time (sec)
        time_out= distout/dpslr::math::c;

        // Rotate station during flight time (radians) */
        dsidt= 6.300388L * (time_out/86400.0L);
        dpslr::math::euclid3DRotMat(3,dsidt,station_rotation);
        station_rotated *= station_rotation;
    }

    // Topocentric outbound vector in local system
    topocentric_out_local.push_back_row(topocentric_outbound);
    topocentric_out_local *= this->rotation_matrix;

    // Outbound azimuth and elevation (laser beam pointing direction)
    elev_out=atanl(topocentric_out_local[0][2]/sqrtl(topocentric_out_local[0][0]*topocentric_out_local[0][0] +
            topocentric_out_local[0][1]*topocentric_out_local[0][1]))*180/dpslr::math::pi;
    azi_out=atan2l(-topocentric_out_local[0][1],topocentric_out_local[0][0])*180/dpslr::math::pi;
    if(azi_out < 0.L)
        azi_out+=360;

    //DIFFERENCE BETWEEN RECEIVE AND TRANSMIT DIRECTION AT TRANSMIT TIME
    diff_azim = 2*(azimuth-azi_out);
    if (diff_azim < -360)
        diff_azim += 720;
    if (diff_azim > +360)
        diff_azim -= 720;
    diff_elev= 2*(elevation-elev_out);

    // Average distance from station to object (both at bounce time).
    if(mode == InterpolationMode::AVERAGE_DISTANCE)
    {
        // Store geocentric interpolated position
        std::copy(y_interp.begin(), y_interp.end(), interp_res.geocentric.begin());

        // Calculate topocentric
        y_interp = y_interp - stat_xyz;

        // One-way range
        interp_res.range = sqrtl(y_interp[0] * y_interp[0] +
                                 y_interp[1] * y_interp[1] +
                                 y_interp[2] * y_interp[2]);

        // Radial center of mass correction.
        if(this->com_offset)
            interp_res.range -= *this->com_offset;

        // Round trip flight time (sec)
        interp_res.tof_2w = 2 * interp_res.range/dpslr::math::c;

        // Direction
        interp_res.azimuth = azi_out;
        interp_res.elevation = elev_out;

        // Differences.
        interp_res.diff_azimuth = diff_azim;
        interp_res.diff_elevation = diff_elev;

        interp_res.error = CPFInterpolator::NOT_ERROR;
        return CPFInterpolator::NOT_ERROR;
    }

    interp_res.error = CPFInterpolator::NOT_ERROR;
    return CPFInterpolator::NOT_ERROR;

    /* TODO: Mode Inbound vector
    // Inbound vector: station at receiving time
    if (mode == 2)
    {
        tinb= time_out;
        //  Iteration
        for (i=0; i<2; i++)
        {
            //  Receive time
            if (iunits == 0)
                tr=tb+tinb;
            else
                tr=tb+tinb/86400.e0;
            //  Rotate station during inbound flight time (other direction)
            dsidt=6.300388e0*(tinb/86400.e0);
            ddreh(3,-dsidt,r3);
            dmlmav(stageo,r3,starot);
            //  Topocentric inbound vector in geocentric system
            topinb[0]=objbnc[0]-starot[0];
            topinb[1]=objbnc[1]-starot[1];
            topinb[2]=objbnc[2]-starot[2];
            //  Distance from object to station
            distinb=sqrt(topinb[0]*topinb[0]+topinb[1]*topinb[1]+topinb[2]*topinb[2]);
            //  Inbound flight time (sec)
            tinb= distinb/c;
        }
        // Total distance from station to object to station (correct computation)
        distot=distout+distinb;
        *fltime=(distot- 2.e0*cofm_corr)/c;
        *range =distot/2.e0- cofm_corr;
    } */

}

PassCalculator::PassCalculator(const CPF &cpf, const dpslr::geo::frames::GeodeticPoint<long double> &stat_geodetic,
                               const dpslr::geo::frames::GeocentricPoint<long double> &stat_geocentric,
                               unsigned int min_elev, long double interval) :
    min_elev_(min_elev),
    interval_(interval),
    interpolator_{cpf, stat_geodetic, stat_geocentric}
{

}

PassCalculator::PassCalculator(const std::string &cpf_path,
                               const dpslr::geo::frames::GeodeticPoint<long double> &stat_geodetic,
                               const dpslr::geo::frames::GeocentricPoint<long double> &stat_geocentric,
                               unsigned int min_elev, long double interval) :
    min_elev_(min_elev),
    interval_(interval),
    interpolator_{CPF(cpf_path, CPF::OpenOptionEnum::ALL_DATA), stat_geodetic, stat_geocentric}
{

}

void PassCalculator::setMinElev(unsigned int min_elev)
{
    this->min_elev_ = min_elev;
}

unsigned int PassCalculator::minElev() const
{
    return this->min_elev_;
}

void PassCalculator::setInterval(long double interval)
{
    this->interval_ = interval;
}

long double PassCalculator::interval() const
{
    return this->interval_;
}

PassCalculator::ResultCodes PassCalculator::getPasses(int mjd_start, long double fract_day_start,
                                                      int mjd_end, long double fract_day_end,
                                                      std::vector<Pass> &passes) const
{
    passes.clear();

    if (this->interpolator_.empty())
        return ResultCodes::CPF_NOT_VALID;

    int mjd_start_cpf, mjd_end_cpf;
    long double fract_start_cpf, fract_end_cpf;

    this->interpolator_.getAvailableTimeInterval(mjd_start_cpf, fract_start_cpf, mjd_end_cpf, fract_end_cpf);

    if ( mjd_start < mjd_start_cpf || mjd_end > mjd_end_cpf ||
        (mjd_start_cpf == mjd_start && fract_day_start < fract_start_cpf) ||
        (mjd_end_cpf == mjd_end && fract_day_end > fract_end_cpf) )
        return ResultCodes::INTERVAL_OUTSIDE_OF_CPF;

    int mjd = mjd_start;
    long double fract_day = fract_day_start;
    CPFInterpolator::InterpolationResult interp_result;
    bool pass_started = false;
    Pass current_pass;
    current_pass.interval = this->interval_;
    current_pass.min_elev = this->min_elev_;
    Pass::Step current_step;

    // TODO improve algorithm
    /*
    std::vector<std::pair<int, long double>> interp_times;

    while(mjd < mjd_end || fract_day <= fract_day_end )
    {
        interp_times.push_back({mjd, fract_day});
        fract_day += this->interval_;
        if (fract_day >= 86400.L)
        {
            mjd++;
            fract_day -= 86400.L;
        }
    }

    CPFInterpolator::InterpolationVector interp_results(interp_times.size());

    // Configure OMP.
    omp_set_num_threads(omp_get_max_threads());

    #pragma omp parallel for
    for(size_t i = 0; i<interp_times.size(); i++)
    {
        CPFInterpolator::InterpolationResult res;
        this->interpolator_.interpolate(interp_times[i].first, interp_times[i].second, res);
        interp_results[i] = res;
    }

    for(const auto interp_result : interp_results)
    {
        if (interp_result.elevation >= this->min_elev_)
        {
            if (!pass_started)
            {
                pass_started = true;
                current_step.azim_rate = 0;
                current_step.elev_rate = 0;
            }
            else
            {
                current_step.azim_rate = (interp_result.azimuth - current_pass.steps.back().azim) / this->interval_;
                current_step.elev_rate = (interp_result.elevation - current_pass.steps.back().elev) / this->interval_;
            }
            current_step.mjd = mjd;
            current_step.fract_day = fract_day;
            current_step.azim = interp_result.azimuth;
            current_step.elev = interp_result.elevation;
            current_pass.steps.push_back(std::move(current_step));
            current_step = {};
        }
        else if(pass_started)
        {
            pass_started = false;
            passes.push_back(std::move(current_pass));
            current_pass = {};
        }

        fract_day += this->interval_;

        if (fract_day >= 86400.L)
        {
            mjd++;
            fract_day -= 86400.L;
        }
    }*/

    while (mjd < mjd_end || fract_day <= fract_day_end )
    {
        auto error = this->interpolator_.interpolate(mjd, fract_day, interp_result);
        
        if (error != CPFInterpolator::InterpolationError::NOT_ERROR && error !=
                                                                           CPFInterpolator::InterpolationError::INTERPOLATION_NOT_IN_THE_MIDDLE)
            return PassCalculator::ResultCodes::OTHER_ERROR;
        
        if (interp_result.elevation >= this->min_elev_)
        {
            if (!pass_started)
            {
                pass_started = true;
                current_step.azim_rate = 0;
                current_step.elev_rate = 0;
            }
            else
            {
                current_step.azim_rate = (interp_result.azimuth - current_pass.steps.back().azim) / this->interval_;
                current_step.elev_rate = (interp_result.elevation - current_pass.steps.back().elev) / this->interval_;
            }
            current_step.mjd = mjd;
            current_step.fract_day = fract_day;
            current_step.azim = interp_result.azimuth;
            current_step.elev = interp_result.elevation;
            current_pass.steps.push_back(std::move(current_step));
            current_step = {};

        }
        else if(pass_started)
        {
            pass_started = false;
            passes.push_back(std::move(current_pass));
            current_pass = {};
        }

        fract_day += this->interval_;

        if (fract_day >= 86400.L)
        {
            mjd++;
            fract_day -= 86400.L;
        }
    }

    if (pass_started)
    {
        passes.push_back(current_pass);
    }

    return PassCalculator::ResultCodes::NOT_ERROR;
}

std::string CPFInterpolator::InterpolationResult::toJson() const
{
    std::ostringstream oss;
    oss << "{"
        << "\"mjd\":" << mjd << ","
        << "\"sec_of_day\":" << std::setprecision(12) << std::fixed << sec_of_day << ","
        << "\"mjdt\":" << std::setprecision(12) << std::fixed << mjdt << ","
        << "\"range\":" << std::setprecision(3) << std::fixed << range << ","
        << "\"tof_2w\":" << std::setprecision(12) << std::fixed << tof_2w << ","
        << "\"azimuth\":" << std::setprecision(4) << std::fixed << azimuth << ","
        << "\"elevation\":" << std::setprecision(4) << std::fixed << elevation << ","
        << "\"diff_azimuth\":" << std::setprecision(4) << std::fixed << diff_azimuth << ","
        << "\"diff_elevation\":" << std::setprecision(4) << std::fixed << diff_elevation << ","
        << "\"geocentric\":[" << std::setprecision(6) << std::fixed << geocentric[0] << ","
        << std::setprecision(6) << std::fixed << geocentric[1] << ","
        << std::setprecision(6) << std::fixed << geocentric[2] << "],"
        << "\"error\":" << error
        << "}";
    return oss.str();
}

}
}
