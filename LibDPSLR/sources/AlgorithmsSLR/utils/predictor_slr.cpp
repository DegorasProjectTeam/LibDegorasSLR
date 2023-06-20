/***********************************************************************************************************************
 *   LibDPSLR (Degoras Project SLR Library): A libre base library for SLR related developments.                        *                                      *
 *                                                                                                                     *
 *   Copyright (C) 2023 Degoras Project Team                                                                           *
 *                      < Ángel Vera Herrera, avera@roa.es - angeldelaveracruz@gmail.com >                             *
 *                      < Jesús Relinque Madroñal >                                                                    *
 *                                                                                                                     *
 *   This file is part of LibDPSLR.                                                                                    *
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

/** ********************************************************************************************************************
 * @file cpf_interpolator.cpp
 * @author Degoras Project Team.
 * @brief This file contains the implementation of the class CPFInterpolator.
 * @copyright EUPL License
 * @version 2306.1
***********************************************************************************************************************/

// C++ INCLUDES
//======================================================================================================================
#include <sstream>
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include <LibDPSLR/AlgorithmsSLR/utils/predictor_slr.h>
#include <LibDPSLR/Timing/common/time_types.h>
#include <LibDPSLR/Mathematics/units.h>
#include <LibDPSLR/Mathematics/math.h>
#include <LibDPSLR/Mathematics/containers/vector3d.h>
#include <LibDPSLR/Mathematics/common/operators.h>
#include <LibDPSLR/Statistics/fitting.h>
#include <LibDPSLR/Statistics/common/statistics_types.h>
#include <LibDPSLR/Astronomical/common/astro_types.h>
#include <LibDPSLR/Geo/tropo.h>
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================namespace dpslr{
namespace dpslr{
namespace algoslr{
namespace utils{
// =====================================================================================================================

// =====================================================================================================================
using ilrs::cpf::CPF;
using namespace astro::common;
using namespace math::common;
using namespace math::units;
using namespace math;
using namespace stats;
// =====================================================================================================================

const std::array<std::string, 10> PredictorSLR::PredictorErrorStr =
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

PredictorSLR::PredictorSLR(const CPF &cpf, const GeodeticPoint<long double> &geod,
                           const GeocentricPoint<long double> &geoc) :
    interpol_function_(InterpolFunction::LAGRANGE_9),
    tropo_model_(TroposphericModel::MARINI_MURRAY),
    prediction_mode_(PredictionMode::OUTBOUND_VECTOR),
    objc_ecc_corr_(0.0),
    grnd_ecc_corr_(0.0),
    cali_del_corr_(0.0),
    syst_rnd_corr_(0.0),
    apply_corr_(true),
    press_(0.0),
    temp_(0.0),
    rel_hum_(0.0),
    wl_(0.0),
    wtrvap_model_(WtrVapPressModel::GIACOMO_DAVIS),
    tropo_ready_(false),
    stat_geodetic_(geod),
    stat_geocentric_(geoc.toVector3D()),
    cpf_(cpf)
    {

    // Check if the cpf is empty.
    if (!this->cpf_.hasData())
        return;

    // Store latitude and longitude in radians.
    this->stat_geodetic_.convert(math::units::Angle<long double>::Unit::RADIANS,
                                math::units::Distance<long double>::Unit::METRES);

    // Store the eccentricity correction from the cpf.
    if(this->cpf_.getHeader().comCorrectionHeader())
        this->objc_ecc_corr_ = this->cpf_.getHeader().comCorrectionHeader().value().com_correction;

    // Auxiliar variables.
    long long mjd_start = this->cpf_.getData().positionRecords().front().mjd;
    long double sod_start = this->cpf_.getData().positionRecords().front().sod;
    long double s_lon = this->stat_geodetic_.lon;
    long double s_lat = this->stat_geodetic_.lat;

    // Rotation matrices.
    math::Matrix<long double> rot_long, rot_lat, rot_long_pi;

    // Get position records and position times for interpolation calculations.
    for (const auto& pos_record : cpf.getData().positionRecords())
    {
        auto time_tag = pos_record.sod - sod_start + (pos_record.mjd - mjd_start) * 86400.L;
        pos_data_.push_back_row(pos_record.position.store());
        pos_times_.push_back(time_tag);
    }

    // Prepare the identity matrix.
    this->rotm_topo_local_ = math::Matrix<long double>::I(3);

    // Computation of rotation matrices.
    // Rotations: rot_long around longitude, rot_lat around pi/2-latitude, rot_long_pi around pi
    math::euclid3DRotMat(3, s_lon, rot_long);
    math::euclid3DRotMat(2, static_cast<long double>(pi/2) - s_lat, rot_lat);
    math::euclid3DRotMat(3, static_cast<long double>(pi), rot_long_pi);
    this->rotm_topo_local_ *= rot_long * rot_lat * rot_long_pi;
}

const CPF& PredictorSLR::getCPF() const {return this->cpf_;}

void PredictorSLR::setPredictionMode(PredictionMode mode){this->prediction_mode_ = mode;}

void PredictorSLR::setTropoModel(TroposphericModel model){this->tropo_model_ = model;}

void PredictorSLR::setInterpolFunction(InterpolFunction func){this->interpol_function_ = func;}

void PredictorSLR::enableCorrections(bool enable){this->apply_corr_ = enable;}

void PredictorSLR::setObjEccentricityCorr(long double correction){this->objc_ecc_corr_ = correction;}

void PredictorSLR::setCaliDelayCorr(long double correction){this->cali_del_corr_ =correction;}

void PredictorSLR::setSystematicCorr(long double correction){this->syst_rnd_corr_ = correction;}

void PredictorSLR::setTropoCorrParams(double press, double temp, double rh, double wl, WtrVapPressModel wvpm)
{
    this->press_ = press;
    this->temp_ = temp;
    this->rel_hum_ = rh;
    this->wl_ = wl;
    this->wtrvap_model_ = wvpm;
    this->tropo_ready_ = true;
}

void PredictorSLR::unsetTropoCorrParams()
{
    this->press_ = 0;
    this->temp_ = 0;
    this->rel_hum_ = 0;
    this->wl_ = 0;
    this->tropo_ready_ = false;
}

const GeodeticPoint<long double>& PredictorSLR::getGeodeticLocation() const{return this->stat_geodetic_;}

GeocentricPoint<long double> PredictorSLR::getGeocentricLocation() const
{
    return GeocentricPoint<long double>(this->stat_geocentric_.store());
}

bool PredictorSLR::isReady() const {return !this->pos_times_.empty(); }



PredictorSLR::PredictionError PredictorSLR::predict(unsigned mjd, long double sod_instant,
                                                    PredictionResult& result) const
{

/*
 *
 * The algorithm is based on the RAZEL algorithm (the reverse of the SITE-TRACK algorithm, A. Vallado) and inspired
 * by the implementation by W. Gurtner (Astronomical Institute, University of Berne). While Hermite was previously
 * used, the algorithm now employs 9th-degree Lagrange. To enhance precision, the option to include various
 * corrections in the calculations has been incorporated, following the extended ranging equation (G. Seeber).
 *
 *
 * LA idea que hay detras de este algoritmo es la siguiente.
 *
 * Hay varias formas de realziar el calculo.
 *
 * Esta algoritmia considera la siguiente asociación de instantes de tiempo y vectores.
 *
 * Instant time -> instant vector
 * Bounce time -> outbound vector.
 * Arrival time -> inbound vector.
 *
 * El primero de ellos, un método rápido, solo tiene en cuenta el tiempo de salida, (el instant time). Este método
 * solo calcularía el instant vector en el instante de tiempo proporcionado como entrada. A partir del instant
 * vector se calcularía el instant range. De esta manera, el tiempo de vuelo sería:
 *
 * FLIGHT TIME = 2 * INSTANT RANGE (instant time)
 *
 * El uso de este método está solo aconsejado cuando la alta precisión no es importante o para sistemas de tiempo real
 * que necesitan realizar un cálculo a priori de manera rápida.
 *
 * El segundo de
 * ellos tiene en cuenta el tiempo del rebote (bounce time), de manera que calcula un promedio de tiempo de vuelo
 *  ofreciendo un resultado bastante preciso.
 *
 * Since the laser pulses are emitted from a rotating Earth, this rotation affects the path and travel time of the laser light, because the entire ECEF geocentric reference system will rotate during the journey of the laser pulse.
because the entire ECEF geocentric reference system will rotate during the journey of the laser pulse.
porque todo el sistema de referencia geocéntrica ECEF rotará durante el viaje del pulso láser.
because the entire ECEF geocentric reference system will rotate during the laser pulse's journey.
porque todo el sistema de referencia geocéntrica ECEF rotará durante el viaje del pulso láser.. In this scenario, it is necessary to consider the rotation of the Earth to accurately calculate the bounce time and the outbound vector.
 *
 * Por otra parte, la elevación y azimuth calculadas para el outbound vector are the ciorresponding with the laser
 * beam pointing direction. Therefore, this calculation method is the most suitable for when you want to get the elevation
 * and azimuth positions to command the tracking mount.
 *
 *
 * El último método tiene en cuenta el tiempo de regreso del rayo (ibound time). De esta manera, determina la
 * como ha rotado la tierra desde que el rayo salió para tener en cuenta la distancia real. Es el método más
 * preciso, pero requiere que lo analicemos más en detalle.
 *
 * El tiempo de interpolacion introducido lo llamaremos outbound.
 *
 *
 * En el modo rápido, el predictor no aplicará la corrección por refracción, ya que necesita de la latitud y longitud
 * para ello.
 *
 *  Por ello, es importante recordar que este modo
 * solo debe de ser usado cuando la alta precisión no es importante.
 *
 * Los resultados de distancias que ofrece este predictor aplica la eccentricity correction at the satellite, if it
 * is available in the CPF. Also, can be defined with the function
 *
 Recordemos que the extended
 * ranging equation is:
 *
 * d = 0.5*c*At + Ad0 + Ads + Adb + Adr + n
 *
 * At  -> Measured flight time of the laser pulse between start and stop signal.
 * Ad0 -> Eccentricity correction on the ground. Could be positive or negative.
 * Ads -> Eccentricity correction at the object (usually the CoM for spherical). Must be applied in a negative manner.
 * Adb -> Signal delay in the ground system (station calibration). For 1w, must be applied half of the delay.
 * Adr -> Refraction correction (usually Marini and Murray formulation is used).
 * n   -> Remaining systematic and random observation errors. Could be positive or negative.
 *
 * Por tanto, esta clase (CPFPredictor) lo que determina (relativo a distancias), es una predicción del parámetro
 * At. El resto de parámetros de la ecuación anterior deben de ser tenidos en cuenta de manera externa cuando sea necesario.
 *
 * Referencias:
 *
 * Satellite Geodesy, Gunter Seeber 2nd Edition, capitulo 8.
 * Fundamentals of Astrodynamics and Applications, David. A. Vallado. 4th Edition. Capitulo 4.
 *
 * TODO Eccentricity correction on the ground.
 * TODO Multistatic mode.
 * TODO Check CPF reference frame.
 * TODO Check CPF com applied com.
 * TODO Check CPF v2 target dynamics = 1.
 * TODO Check leap seconds flaf.
 * TODO TLE source mode using SGP4 propagator.
 */

    // CHECK SOME PARAMETERS.
    //------------------------------------------------------------------------------------------------------------------

    // Interpolation is not possible if there are no position records
    if (this->pos_times_.empty())
    {
        result.error = PredictionError::NO_POS_RECORDS;
        return PredictionError::NO_POS_RECORDS;
    }

    //------------------------------------------------------------------------------------------------------------------

    // CONTAINERS AND VARIABLES.
    //------------------------------------------------------------------------------------------------------------------

    // CPF interpolation result variables.
    long double x_instant;                  // Relative interpolation time for the instant time (instant vector).
    long double x_bounce;                   // Relative interpolation time for the bounce time (outbound vector).
    long double x_arrival;                  // Relative interpolation time for the arrival time (inbound vector).

    // Position vectors at different times.
    Vector3D<long double> y_instant;    // Vector with the interpolated position at instant time.
    Vector3D<long double> y_outbound;   // Vector with the interpolated position at bounce time.
    Vector3D<long double> y_inbound;    // Vector with the interpolated position at arrival time.

    // Rotating matrix for calculate the local topo vectors (SEZ).
    Matrix<long double> rotatedm_topo_s_o_instant;
    Matrix<long double> rotatedm_topo_s_o_outbound;

    // Rotation and rotated matrices for the Earth rotation.
    Matrix<long double> rotm_earth_rotation ;
    Matrix<long double> rotatedm_earth;

    // Topocentric-horizon vectors (SEZ).
    Vector3D<long double> topo_s_o_instant;            // Topocentric station to object vector at instant time.
    Vector3D<long double> topo_s_o_outbound;           // Topocentric station to object vector at bounce time.
    Vector3D<long double> topo_s_o_local_instant;      // Local topocentric station to object vector at instant time.
    Vector3D<long double> topo_s_o_local_outbound;     // Local topocentric station to object vector at bounce time.


    // Other prediction values.
    long double range_1w_instant;        // Range value in meters for instant vector.
    long double range_1w_outbound;       // Range value in meters for outbound vector.

    long double el_instant, az_instant;    // Elevation and azimuth for the instant vector (degrees).
    long double el_outbound, az_outbound;  // Elevation and azimuth for the outbound vector (degrees).

    // Other auxiliar variables.
    PredictionError error;      // Auxiliar error variable.
    double diff_az, diff_el;    // Azimuth and elevation difference between the outbound and instant vectors
    long double aux_range;      // Auxiliar range variable.

    // TODO rename some variables and add comments.

    long double  tof_1w, dsidt;


    //------------------------------------------------------------------------------------------------------------------

    // ALGORITHM.
    //------------------------------------------------------------------------------------------------------------------

    // ONLY INSTANT RANGE MODE -----------------------------------------------------------------------------------------

    // Default error value (no error).
    result.error = PredictionError::NO_ERROR;

    // Generate the relative times.
    int day_relative = mjd - this->cpf_.getData().positionRecords().front().mjd;
    x_instant = (day_relative*kSecsSolDay) + sod_instant - this->cpf_.getData().positionRecords().front().sod;

    // Check if the interpolation times are valid in the cpf data interval.
    if((x_instant - kTMargin)   < 0 || (x_instant + kTMargin) > this->pos_times_.back())
    {
        result.error = PredictionError::X_INTERPOLATED_OUT_OF_BOUNDS;
        return PredictionError::X_INTERPOLATED_OUT_OF_BOUNDS;
    }

    // Call to the interpolator and get the y_instant position vector.
    error = this->callToInterpol(x_instant, y_instant, result);

    // Check the interpolator error.
    if(error != PredictionError::NO_ERROR)
        return error;

    // Form the topocentric position vector (station to object) at instant time.
    topo_s_o_instant = y_instant - this->stat_geocentric_;

    // Instant range from station to object at instant time.
    range_1w_instant = topo_s_o_instant.magnitude();

    // Update auxiliar range.
    aux_range = range_1w_instant;

    // Include part of the corrections. The station correction will be permanent.
    if(this->apply_corr_ )
        aux_range = this->applyCorrections(range_1w_instant, result, true);

    // Store computed data up to this moment.
    // In the storage only, include the object eccentricity correction (CoM) and the systematic errors.
    result.instant_range.mjd = mjd;
    result.instant_range.mjdt = mjd + sod_instant/kSecsSolDay;
    result.instant_range.sod = sod_instant;
    result.instant_range.geo_pos = y_instant;
    result.instant_range.range_1w = aux_range;
    result.instant_range.tof_2w = 2*result.instant_range.range_1w/c;

    // WARNING: At this point, <range_1w_instant> only include the system delay correction, but the <PredictionResult>
    // result container include the eccentricity corrections and the systematic errors.

    // If the mode is only instant range, return here.
    if (this->prediction_mode_ == PredictionMode::ONLY_INSTANT_RANGE)
        return PredictionError::NO_ERROR;

    // INSTANT VECTOR MODE ---------------------------------------------------------------------------------------------

    // TODO VELOCITY VECTOR.

    // Prepare the data.
    result.instant_data.emplace(result.instant_range);

    // Topocentric instant station to object vector in local system (using the rotation matrix).
    rotatedm_topo_s_o_instant.push_back_row(topo_s_o_instant.toVector());
    rotatedm_topo_s_o_instant *= this->rotm_topo_local_;
    topo_s_o_local_instant = Vector3D<long double>::fromVector(rotatedm_topo_s_o_instant.getRow(0));

    // Compute azimuth for instant vector (degrees).
    el_instant = units::radToDegree(atanl(topo_s_o_local_instant[2] / sqrtl(
                                    math::pow2(topo_s_o_local_instant[0]) + math::pow2(topo_s_o_local_instant[1]))));

    // Compute elevation for instant vector (degrees).
    az_instant=units::radToDegree(atan2l(-topo_s_o_local_instant[1], topo_s_o_local_instant[0]));

    // Check 90 degrees elevation case (pag 263 fundamental of astrodinamic and applications A. Vallado).
    if(dpslr::math::compareFloating(el_instant, 90.0L) == 0)
        el_instant -= 0.0001L;

    // Check the negative azimuth case.
    if(az_instant < 0.L)
        az_instant += 360.L;

    // Include part of the corrections. The tropospheric correction will be permanent.
    if(this->apply_corr_ )
        aux_range = this->applyCorrections(range_1w_instant, result, false, el_instant);

    // Store computed data up to this moment.
    result.instant_data.value().az = az_instant;
    result.instant_data.value().el = el_instant;
    result.instant_data.value().range_1w = aux_range;
    result.instant_data.value().tof_2w = 2*result.instant_data.value().range_1w/c;

    // If the mode is only instant vector, return here.
    if (this->prediction_mode_ == PredictionMode::INSTANT_VECTOR)
        return PredictionError::NO_ERROR;

    // WARNING: At this point, <range_1w_instant> include the system delay correction and the tropospheric path delay
    // correction, but the <PredictionResult> container include the eccentricity corrections and the systematic errors.

    // OUTBOUND VECTOR MODE --------------------------------------------------------------------------------------------

    // At this point, we have already aplied the CoM, station delay and refraction correction for the instant time. So,
    // it is not necessary to apply the corrections again during the bounce time calculation, because the new positions
    // interpolated already incorporates these corrections.

    // Prepare the result storage.
    result.outbound_data.emplace();

    // Calculate the time to reach the satellite.
    tof_1w = range_1w_instant/c;

    // Prepare the matrix to rotate the ECEF coordinate system (with the rotation of the Earth).
    rotatedm_earth.push_back_row(this->stat_geocentric_.toVector());

    // Rotate the coordinate system for laser pulse two-way trip (2 iteration).
    for (unsigned i=0; i<2; i++)
    {
        // Calculate the bounce time.
        x_bounce = x_instant + tof_1w;

        // Interpolate geocentric position of the object at bounce time.
        error = this->callToInterpolator(x_bounce, y_outbound, result);

        // Check the interpolator error.
        if(error != PredictionError::NO_ERROR)
            return error;

        // Topocentric outbound vector
        topo_s_o_outbound = y_outbound - Vector3D<long double>::fromVector(rotatedm_earth.getRow(0));

        // Distance from station (instant time) to object (bounce time).
        range_1w_outbound = topo_s_o_outbound.magnitude();

        // Outbound flight time (sec)
        tof_1w = range_1w_outbound/c;

        // Rotate station during flight time (radians)
        dsidt= kEarthRotSolDay * (tof_1w/kSecsSolDay);
        dpslr::math::euclid3DRotMat(3,dsidt,rotm_earth_rotation);
        rotatedm_earth *= rotm_earth_rotation;
    }

    // WARNING: At this moment, we have the station and the satellite both at the bounce time moment. This is a good
    // moment to perform (using the outbound vector) a very good aprproximation to the correct computation.

    // Topocentric outbound station to object vector in local system (using the rotation matrix).
    rotatedm_topo_s_o_outbound.push_back_row(topo_s_o_outbound.toVector());
    rotatedm_topo_s_o_outbound *= this->rotm_topo_local_;
    topo_s_o_local_outbound = Vector3D<long double>::fromVector(rotatedm_topo_s_o_instant.getRow(0));

    // WARNING: The next azimuth and elevation are the laser beam, pointing direction.

    // Compute azimuth for outbound vector (laser beam pointing direction, degrees).
    el_outbound = units::radToDegree(atanl(topo_s_o_local_outbound[2] / sqrtl(
                                math::pow2(topo_s_o_local_outbound[0]) + math::pow2(topo_s_o_local_outbound[1]))));

    // Compute elevation for outbound vector (degrees).
    az_outbound = units::radToDegree(atan2l(-topo_s_o_local_outbound[1], topo_s_o_local_outbound[0]));

    // Check 90 degrees elevation case (pag 263 fundamental of astrodinamic and applications A. Vallado).
    if(dpslr::math::compareFloating(el_outbound, 90.0L) == 0)
        el_outbound -= 0.0001L;

    // Check the negative azimuth case.
    if(az_outbound < 0.L)
        az_outbound += 360.L;

    // WARNING: The change of the elevation and azimuth are minimal, so isn't neccesary recalculate the tropospheric
    // path delay correction.

    // Calculates the difference between receive and transmit direction at instant time.
    diff_az = 2*(az_instant-az_outbound);
    if (diff_az < -360)
        diff_az += 720;
    if (diff_az > +360)
        diff_az -= 720;
    diff_el = 2*(el_instant-el_outbound);

    // Store computed data up to this moment.
    result.diff_az = diff_az;
    result.diff_el = diff_el;
    result.outbound_data->az = az_outbound;
    result.outbound_data->el = el_outbound;
    result.outbound_data->geo_pos = y_outbound;
    // coger del xbound relativo y pasarlo a mjd mjdt y seconds.
    result.outbound_data.value().mjd = mjd;

    // One-way range
    result.outbound_data->range_1w = std::sqrt(
        y_outbound[0]*y_outbound[0] +
        y_outbound[1]*y_outbound[1] +
        y_outbound[2]*y_outbound[2]
        );

    // Radial center of mass correction.
    if(this->com_offset)
        result.outbound_data->range_1w -= *this->com_offset;

    // Round trip flight time (sec)
    result.outbound_data->tof_2w = 2 * result.outbound_data->range_1w/dpslr::astro::common::c;


    // Average distance from station to object (both at bounce time for good approximation).





        result.mjd = mjd;
        result.instant_data.mjdt = mjd + sod_instant/kSecsSolDay;
        result.instant_data.sod = sod_instant;
        result.instant_data.geo_pos = y_instant;


        // Calculate topocentric
        x_instant = y_interp - geo_stat_xyz;





    if(mode == InterpolationMode::AVERAGE_DISTANCE)
    {
        result.error = CPFPredictor::NO_ERROR;
        return CPFPredictor::NO_ERROR;
    }

    result.error = CPFPredictor::NO_ERROR;
    return CPFPredictor::NO_ERROR;

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




PredictorSLR::PredictionError PredictorSLR::interpolate(long double mjt, InterpolationResult &interp_res,
                                                        InterpolationMode mode, InterpolationFunction function) const
{
    // Time conversions.
    long double mjd;
    long double second = std::modf(mjt , &mjd) * 86400.0L;

    // Call with the datetime value splitted in the day and second in that day.
    return this->predict(static_cast<int>(mjd), second, interp_res, mode, function);
}

long double PredictorSLR::applyCorrections(long double &range, PredictionResult &result, bool cali, double el) const
{
    // Auxiliar variable.
    long double aux_range = range;

    // Include the half of the system delay to the range. This will be permanent for the rest of computations.
    if(this->cali_del_corr_ != 0 && cali)
    {
        range += 0.5*this->cali_del_corr_*c*kPsToSec;
        result.cali_del_corr = this->cali_del_corr_;
        aux_range = range;
    }

    // Include the object eccentricity correction.
    if(this->objc_ecc_corr_ != 0)
    {
        aux_range = aux_range - this->objc_ecc_corr_;
        result.objc_ecc_corr = this->objc_ecc_corr_;
    }

    // Include the ground eccentricity correction.
    if(this->objc_ecc_corr_ != 0)
    {
        aux_range = aux_range + this->grnd_ecc_corr_;
        result.grnd_ecc_corr = this->grnd_ecc_corr_;
    }

    // Include the systematic and random errors.
    if(this->syst_rnd_corr_ != 0)
    {
        aux_range = aux_range + this->syst_rnd_corr_;
        result.syst_rnd_corr = this->syst_rnd_corr_;
    }

    // Compute and include the tropospheric path delay.
    if(el != 0)
    {
        // Get the elevation in radians.
        double el_instant_rad = math::units::degToRad(el);

        // Calculate the tropospheric path delay (1 way).
        range += geo::tropo::pathDelayMariniMurray(this->press_, this->temp_, this->rel_hum_, el_instant_rad,
                                                   this->wl_, this->stat_geodetic_.lat, this->stat_geodetic_.alt,
                                                   this->wtrvap_model_);
        // Store the range.
        aux_range = range;
    }

    // Return the new range with the corrections and the original range with the calibration correction applied.
    return aux_range;
}


PredictorSLR::PredictionError PredictorSLR::callToInterpol(long double x, Vector3D<long double> &y, PredictionResult &result) const
{
    if(this->interpol_function_ == PredictorSLR::InterpolFunction::LAGRANGE_9)
    {
        // Result of the interpolation.
        stats::common::LagrangeError lag_res;

        // Do the 9th degree interpolation.
        lag_res = stats::lagrangeInterpol3DVec(this->pos_times_, this->pos_data_, kPolLagDeg, x, y);

        // Return if error.
        if (stats::common::LagrangeError::NOT_ERROR != lag_res)
        {
            PredictionError err = this->convertLagInterpError(lag_res);
            result.error = err;
            return err;
        }
    }
    else
    {
        // Interpolator not implemented.
        result.error = PredictorSLR::PredictionError::UNKNOWN_INTERPOLATOR;
        return PredictorSLR::PredictionError::UNKNOWN_INTERPOLATOR;
    }
}

PredictorSLR::PredictionError PredictorSLR::convertLagInterpError(stats::common::LagrangeError error) const
{
    PredictorSLR::PredictionError cpf_error;
    switch (error)
    {
    case stats::common::LagrangeError::NOT_ERROR :
        cpf_error = PredictionError::NO_ERROR; break;
    case stats::common::LagrangeError::NOT_IN_THE_MIDDLE :
        cpf_error = PredictionError::INTERPOLATION_NOT_IN_THE_MIDDLE; break;
    case stats::common::LagrangeError::X_OUT_OF_BOUNDS :
        cpf_error = PredictionError::X_INTERPOLATED_OUT_OF_BOUNDS; break;
    case stats::common::LagrangeError::DATA_SIZE_MISMATCH :
        cpf_error = PredictionError::INTERPOLATION_DATA_SIZE_MISMATCH; break;
    }
    return cpf_error;
}

PredictorSLR::InstantData::InstantData(const InstantRange& instant_range) : InstantRange(instant_range),
    mjd(0),
    sod(0.), mjdt(0.), az(0.), el(0.)
{}


}}} // END NAMESPACES
// =====================================================================================================================
