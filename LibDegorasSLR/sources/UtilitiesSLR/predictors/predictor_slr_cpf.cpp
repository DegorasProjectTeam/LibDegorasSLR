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

/** ********************************************************************************************************************
 * @file predictor_cpf.cpp
 * @author Degoras Project Team.
 * @brief This file contains the implementation of the class PredictorCPF.
 * @copyright EUPL License
 2306.1
***********************************************************************************************************************/

// C++ INCLUDES
// =====================================================================================================================
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/UtilitiesSLR/predictors/predictor_slr_cpf.h"
#include "LibDegorasSLR/Astronomical/astro_constants.h"
// =====================================================================================================================

// LIBDPBASE INCLUDES
// =====================================================================================================================
#include "LibDegorasBase/Mathematics/utils/math_utils.h"
#include "LibDegorasBase/Mathematics/types/vector3d.h"
#include "LibDegorasBase/Statistics/types/statistics_types.h"
#include "LibDegorasBase/Statistics/fitting.h"
// =====================================================================================================================

// LIBDEGORASSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace slr{
namespace predictors{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using namespace dpbase::helpers::strings;
using namespace dpbase::math::units;
using namespace dpbase::math::types;
using namespace dpbase::math::units::literals;
using namespace geo::types;
using namespace dpbase::timing::types;
using namespace dpbase::timing::dates;
using namespace ilrs::cpf;
using namespace dpbase::math::units::literals;
// ---------------------------------------------------------------------------------------------------------------------

const std::array<std::string, 10> PredictorSlrCPF::PredictorErrorStr =
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

PredictorSlrCPF::PredictorSlrCPF(const std::string &cpf_path, const GeodeticPointDeg &geod,
                                 const GeocentricPoint &geoc) :
    PredictorSlrBase(geod, geoc),
    interp_funct_(InterpFunction::LAGRANGE_16)
{
    // Set the CPF ephemerids file.
    this->setCPF(cpf_path);
}

PredictorSlrCPF::PredictorSlrCPF(const GeodeticPointDeg &geod, const GeocentricPoint &geoc) :
    PredictorSlrBase(geod, geoc),
    interp_funct_(InterpFunction::LAGRANGE_16)
{}

bool PredictorSlrCPF::setCPF(const std::string &cpf_path)
{
    // Open and store the cpf.
    this->cpf_ = CPF(cpf_path, CPF::OpenOptionEnum::ALL_DATA);

    // Check if the CPF was open and has data.
    if(this->cpf_.getReadError() != CPF::ReadFileErrorEnum::NOT_ERROR || !this->cpf_.hasData())
        return false;

    // Store the eccentricity correction from the cpf.
    if(this->cpf_.getHeader().comCorrectionHeader())
    {
        this->objc_ecc_corr_ = static_cast<long double>(
            this->cpf_.getHeader().comCorrectionHeader().value().com_correction);
    }

    // Auxiliar variables.
    MJDate mjd_start = this->cpf_.getData().positionRecords().front().mjd;
    SoD sod_start = this->cpf_.getData().positionRecords().front().sod;
    Radians s_lon = this->getGeodeticLocation<Radians>().lon;
    Radians s_lat = this->getGeodeticLocation<Radians>().lat;

    // Get position records and position times for interpolation calculations.
    for (const auto& pos_record : this->cpf_.getData().positionRecords())
    {
        auto time_tag = pos_record.sod - sod_start + (pos_record.mjd - mjd_start) * 86400.L;
        pos_data_.pushBackRow(pos_record.geo_pos.store());
        pos_times_.push_back(time_tag);
    }

    // Prepare the identity matrix.
    this->rotm_topo_local_ = Matrix<Meters>::I(3);

    // Computation of rotation matrices.
    // Rotations: rot_long around longitude, rot_lat around pi/2-latitude, rot_long_pi around pi
    this->rotm_topo_local_.euclidian3DRotation(3, s_lon);
    this->rotm_topo_local_.euclidian3DRotation(2, static_cast<long double>(dpbase::math::kPi/2) - s_lat);
    this->rotm_topo_local_.euclidian3DRotation(3, static_cast<long double>(dpbase::math::kPi));

    // All ok.
    return true;
}

const CPF& PredictorSlrCPF::getCPF() const {return this->cpf_;}

bool PredictorSlrCPF::isReady() const
{
    return !this->pos_times_.empty();
}

PredictionSLR::ErrorType PredictorSlrCPF::predict(const MJDateTime& mjdt, PredictionSLR& result) const
{

 /*
 *
 * The algorithm is based on the RAZEL algorithm (the reverse of the SITE-TRACK algorithm, A. Vallado) and inspired
 * by the implementation by W. Gurtner (Astronomical Institute, University of Berne). While Hermite was previously
 * used, the algorithm now employs 9th-degree or 16th-degree Lagrange. To enhance precision, the option to include various
 * corrections in the calculations has been incorporated, following the extended ranging equation (G. Seeber).
 *
 *
 * La idea que hay detras de este algoritmo es la siguiente.
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
 * TODO Multistatic mode.
 * TODO Check CPF reference frame.
 * TODO Check CPF com applied com.
 * TODO Check CPF v2 target dynamics = 1.
 * TODO Check leap seconds flag (maybe better in CPF and set the CPF as invalid).
 * TODO TLE source mode using SGP4 propagator.
 */

    // CHECK SOME PARAMETERS.
    //------------------------------------------------------------------------------------------------------------------

    // The CPF has issues.
    if(this->cpf_.getReadError() != CPF::ReadFileErrorEnum::NOT_ERROR)
    {
        result.error = static_cast<PredictionSLR::ErrorType>(PredictorSlrCPF::PredictionError::CPF_LOAD_ERROR);
        return result.error;
    }

    // Interpolation is not possible if there are no position records
    if (this->pos_times_.empty())
    {
        result.error = static_cast<PredictionSLR::ErrorType>(PredictorSlrCPF::PredictionError::NO_POS_RECORDS);
        return result.error;
    }

    //------------------------------------------------------------------------------------------------------------------

    // CONTAINERS AND VARIABLES.
    // Remember that all units are long doubles, necessary to achieve adequate precision in calculations.
    //------------------------------------------------------------------------------------------------------------------

    // CPF interpolation result variables.
    Seconds x_instant;      // Relative interpolation time for the instant time (instant vector).
    Seconds x_bounce;       // Relative interpolation time for the bounce time (outbound vector).
    //Seconds x_arrival;      // Relative interpolation time for the arrival time (inbound vector). TODO

    // Position vectors at different times.
    Vector3D<Meters> y_instant;    // Vector with the interpolated position at instant time.
    Vector3D<Meters> y_outbound;   // Vector with the interpolated position at bounce time.
    Vector3D<Meters> y_inbound;    // Vector with the interpolated position at arrival time.

    // Rotating matrix for calculate the local topo vectors (SEZ).
    Matrix<Meters> rotatedm_topo_s_o_instant;
    Matrix<Meters> rotatedm_topo_s_o_outbound;

    // Rotation and rotated matrices for the Earth rotation.
    Matrix<Meters> rotm_earth_rotation;
    Matrix<Meters> rotatedm_earth;

    // Topocentric-horizon vectors (SEZ).
    Vector3D<Meters> topo_s_o_instant;            // Topocentric station to object vector at instant time.
    Vector3D<Meters> topo_s_o_outbound;           // Topocentric station to object vector at bounce time.
    Vector3D<Meters> topo_s_o_local_instant;      // Local topocentric station to object vector at instant time.
    Vector3D<Meters> topo_s_o_local_outbound;     // Local topocentric station to object vector at bounce time.


    // Other prediction values.
    Meters range_1w_instant;      // Range value for instant vector (meters, 1 way).
    Meters range_1w_outbound;     // Range value for the outbound vector (meters, 1 way).
    Meters range_1w_average;      // Average range value at bounce time (meters, 1 way).

    Degrees el_instant, az_instant;    // Elevation and azimuth for the instant vector (degrees).
    Degrees el_outbound, az_outbound;  // Elevation and azimuth for the outbound vector (degrees).

    // Other auxiliar variables.
    Degrees diff_az, diff_el;  // Azimuth and elevation difference between the outbound and instant vectors (degrees).
    Meters prov_range_1w;      // Provisional range variable which will be updated during calculations (meters, 1 way).
    Seconds aux_tof_1w;        // Auxiliar time of flight value (seconds, 1 way).
    Radians earth_rot_angle;   // Earth rotation angle during flight time (radians).


    //------------------------------------------------------------------------------------------------------------------

    // ALGORITHM.
    //------------------------------------------------------------------------------------------------------------------

    // ONLY INSTANT RANGE MODE -----------------------------------------------------------------------------------------

    // Default error value (no error).
    result.error = static_cast<PredictionSLR::ErrorType>(PredictionError::NOT_ERROR);

    // Generate the relative times.
    long long day_relative = mjdt.date() - this->cpf_.getData().positionRecords().front().mjd;
    x_instant = (day_relative*astro::kSecsSolDay) + mjdt.sod() - this->cpf_.getData().positionRecords().front().sod;

    // Check if the interpolation times are valid in the cpf data interval.
    if((x_instant - kTMargin)   < 0 || (x_instant + kTMargin) > this->pos_times_.back())
    {
        result.error = static_cast<PredictionSLR::ErrorType>(PredictionError::X_INTERPOLATED_OUT_OF_BOUNDS);
        return result.error;
    }

    // Call to the interpolator and get the y_instant position vector.
    this->callInterp(x_instant, y_instant, result);

    // Check the interpolator error.
    if(result.error != static_cast<PredictionSLR::ErrorType>(PredictionError::NOT_ERROR))
        return result.error;

    // Form the topocentric position vector (station to object) at instant time.
    topo_s_o_instant = y_instant - this->getGeocentricLocation().toVector3D();

    // Instant range from station to object at instant time.
    range_1w_instant = topo_s_o_instant.magnitude();

    // Update auxiliar range.
    prov_range_1w = range_1w_instant;

    // Include part of the corrections. The station correction will be permanent.
    if(this->apply_corr_ )
        prov_range_1w = this->applyCorrections(range_1w_instant, result, true);

    // Store computed data up to this moment.
    // In the storage only, include the object eccentricity correction (CoM) and the systematic errors.
    result.instant_range.mjdt = mjdt;
    result.instant_range.geo_pos = y_instant;
    result.instant_range.range_1w = prov_range_1w;
    result.instant_range.tof_2w = 2*result.instant_range.range_1w/astro::kC;

    // WARNING: At this point, <range_1w_instant> only include the system delay correction, but in the result container
    // <PredictionResult> the range include the eccentricity corrections and the systematic errors.

    // If the mode is only instant range, return here.
    if (this->getPredictionMode() == PredictionMode::ONLY_INSTANT_RANGE)
        return result.error;

    // INSTANT VECTOR MODE ---------------------------------------------------------------------------------------------

    // TODO VELOCITY VECTOR.

    // Prepare the data.
    result.instant_data.emplace(std::move(result.instant_range));

    // Topocentric instant station to object vector in local system (using the rotation matrix).
    rotatedm_topo_s_o_instant.pushBackRow(topo_s_o_instant.toVector());
    rotatedm_topo_s_o_instant *= this->rotm_topo_local_;
    topo_s_o_local_instant = Vector3D<Meters>::fromVector(rotatedm_topo_s_o_instant.getRow(0));

    // Compute azimuth for instant vector (degrees).
    el_instant = radToDegree(atanl(topo_s_o_local_instant[2] / sqrtl(
                                    dpbase::math::pow2(topo_s_o_local_instant[0]) + dpbase::math::pow2(topo_s_o_local_instant[1]))));

    // Compute elevation for instant vector (degrees).
    az_instant= radToDegree(atan2l(-topo_s_o_local_instant[1], topo_s_o_local_instant[0]));

    // Check 90 degrees elevation case (pag 263 fundamental of astrodinamic and applications A. Vallado).
    if(dpbase::math::compareFloating(el_instant, 90.0_deg) == 0)
        el_instant -= 0.0001L;

    // Check the negative azimuth case.
    if(az_instant < 0.L)
        az_instant += 360.L;

    // Include part of the corrections. The tropospheric correction will be permanent.
    if(this->apply_corr_ )
        prov_range_1w = this->applyCorrections(range_1w_instant, result, false, el_instant);

    // Store computed data up to this moment.
    result.instant_data.value().altaz_coord.az = az_instant;
    result.instant_data.value().altaz_coord.el = el_instant;
    result.instant_data.value().range_1w = prov_range_1w;
    result.instant_data.value().tof_2w = 2*result.instant_data.value().range_1w/astro::kC;

    // If the mode is only instant vector, return here.
    if (this->getPredictionMode() == PredictionMode::INSTANT_VECTOR)
        return result.error;

    // WARNING: At this point, <range_1w_instant> include the system delay correction and the tropospheric path delay
    // correction, but the <PredictionResult> container include the eccentricity corrections and the systematic errors.

    // OUTBOUND VECTOR MODE --------------------------------------------------------------------------------------------

    // TODO VELOCITY VECTOR.

    // WARNING: At this point, we have already aplied the station delay and refraction correction for the instant time.
    // So, it is not necessary to apply the corrections again during the bounce time calculation, because the new
    // positions interpolated already incorporates these corrections and the az/el changes during ToF are minimal.

    // Prepare the result storage.
    result.outbound_data.emplace();

    // Calculate the time to reach the satellite.
    aux_tof_1w = range_1w_instant/astro::kC;

    // Prepare the matrix to rotate the ECEF coordinate system (with the rotation of the Earth).
    rotatedm_earth.pushBackRow(this->getGeocentricLocation().toStdVector());

    // Rotate the coordinate system for laser pulse two-way trip (2 iteration). Remember that this is
    // due to the fact that the light becomes detached from the reference system during its trip.
    for (unsigned i=0; i<2; i++)
    {
        // Calculate the bounce time.
        x_bounce = x_instant + aux_tof_1w;

        // Interpolate geocentric position of the object at bounce time.
        this->callInterp(x_bounce, y_outbound, result);

        // Check the interpolator error.
        if(result.error != static_cast<PredictionSLR::ErrorType>(PredictionError::NOT_ERROR))
            return result.error;

        // Topocentric outbound vector
        topo_s_o_outbound = y_outbound - Vector3D<Meters>::fromVector(rotatedm_earth.getRow(0));

        // Distance from station (instant time) to object (bounce time).
        range_1w_outbound = topo_s_o_outbound.magnitude();

        // Outbound flight time (sec)
        aux_tof_1w = range_1w_outbound/astro::kC;

        // Rotate station during flight time (radians)
        earth_rot_angle = astro::kEarthRotSolDay * (aux_tof_1w/astro::kSecsSolDay);
        rotatedm_earth.euclidian3DRotation(3, earth_rot_angle);
    }

    // WARNING: At this moment, we have the station and the satellite both at the bounce time moment. This is a good
    // moment to perform (using the outbound vector) a very good aprproximation to the correct computation.

    // Topocentric outbound station to object vector in local system (using the rotation matrix).
    rotatedm_topo_s_o_outbound.pushBackRow(topo_s_o_outbound.toVector());
    rotatedm_topo_s_o_outbound *= this->rotm_topo_local_;
    topo_s_o_local_outbound = Vector3D<Meters>::fromVector(rotatedm_topo_s_o_outbound.getRow(0));

    // WARNING: The next azimuth and elevation are the laser beam pointing direction. We recommend using these
    // positions to command the tracking mount.

    // Compute azimuth for outbound vector (laser beam pointing direction, degrees).
    el_outbound = radToDegree(atanl(topo_s_o_local_outbound[2] / sqrtl(
                                dpbase::math::pow2(topo_s_o_local_outbound[0]) + dpbase::math::pow2(topo_s_o_local_outbound[1]))));

    // Compute elevation for outbound vector (degrees).
    az_outbound = radToDegree(atan2l(-topo_s_o_local_outbound[1], topo_s_o_local_outbound[0]));

    // Check 90 degrees elevation case (pag 263 fundamental of astrodinamic and applications A. Vallado).
    if(dpbase::math::compareFloating(el_outbound, Degrees(90.0L)) == 0)
        el_outbound -= 0.0001L;

    // Check the negative azimuth case.
    if(az_outbound < 0.L)
        az_outbound += 360.L;

    // Calculates the difference between the outbound and instant direction at instant time.
    diff_az = 2*(az_instant-az_outbound);
    if (diff_az < -360)
        diff_az += 720;
    if (diff_az > +360)
        diff_az -= 720;
    diff_el = 2*(el_instant-el_outbound);

    // Calculates the average distance from station to object at bounce time (good approximation).
    range_1w_average = (y_outbound-this->getGeocentricLocation().toVector3D()).magnitude();

    // WARNING: The change of the elevation and azimuth are minimal, so isn't neccesary recalculate the tropospheric
    // path delay correction. In addition, in other ILRS algorithms such as the one for NP formation and analysis,
    // we have not seen any recalculation either.

    // We need to apply the rest of the corrections (all minus tropo and system delay).
    if(this->apply_corr_)
        range_1w_average = this->applyCorrections(range_1w_average, result, false);

    // Store computed data up to this moment.
    result.diff_az = diff_az;
    result.diff_el = diff_el;
    result.outbound_data.value().altaz_coord.az = az_outbound;
    result.outbound_data.value().altaz_coord.el = el_outbound;
    result.outbound_data.value().geo_pos = y_outbound;
    result.outbound_data.value().range_1w = range_1w_average;
    result.outbound_data.value().tof_2w = 2 * result.outbound_data->range_1w/astro::kC;

    // coger del xbound relativo y pasarlo a mjd mjdt y seconds.
    result.outbound_data.value().mjdt = mjdt;

    // If the mode is only outbound vector, return here.
    if (this->getPredictionMode() == PredictionMode::OUTBOUND_VECTOR)
        return result.error;

    // Return here if inbound is selected (TODO).
    result.error = static_cast<PredictionSLR::ErrorType>(PredictionError::OTHER_ERROR);
    return result.error;

    // INBOUND VECTOR MODE ---------------------------------------------------------------------------------------------

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
            earth_rot_angle=6.300388e0*(tinb/86400.e0);
            ddreh(3,-earth_rot_angle,r3);
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

void PredictorSlrCPF::getAvailableTimeWindow(MJDateTime &start, MJDateTime &end) const
{
    // Return the real valid time window, avoiding the time lapse where the interpolation returns not in the
    // middle error removing data from the boundaries of the CPF. This is done internally by the CPF function.
    if (this->isReady())
    {
        this->cpf_.getAvailableTimeWindow(start, end);
    }
    else
    {
        start = MJDateTime();
        end = MJDateTime();
    }
}

std::string PredictorSlrCPF::getErrorMsg(PredictionSLR::ErrorType error_code) const
{
    std::string result;
    if (error_code < PredictorSlrCPF::PredictorErrorStr.size())
        result = PredictorSlrCPF::PredictorErrorStr[error_code];
    return result;
}

void PredictorSlrCPF::setInterpFunction(InterpFunction intp_funct)
{
    this->interp_funct_ = intp_funct;
}

PredictorSlrCPF::InterpFunction PredictorSlrCPF::getInterpFunction() const
{
    return this->interp_funct_;
}


void PredictorSlrCPF::callInterp(const Seconds& x, Vector3D<Meters> &y, PredictionSLR &result) const
{
    // Auxiliar error container.
    PredictorSlrCPF::PredictionError error = PredictorSlrCPF::PredictionError::UNKNOWN_INTERPOLATOR;
    result.error = static_cast<std::uint32_t>(error);

    // Lagrange related interpolators.
    if(this->interp_funct_ == PredictorSlrCPF::InterpFunction::LAGRANGE_9 ||
        this->interp_funct_ == PredictorSlrCPF::InterpFunction::LAGRANGE_16 )
    {
        unsigned deg = kPolLagDeg9;
        
        if(this->interp_funct_ == PredictorSlrCPF::InterpFunction::LAGRANGE_16)
            deg = kPolLagDeg16;

        // Result of the interpolation.
        dpbase::stats::types::LagrangeError lag_res;

        // Do the lagrange interpolation.
        lag_res = dpbase::stats::lagrangeInterpol3DVec(this->pos_times_, this->pos_data_, deg, x, y);

        // Convert the error code.
        error = PredictorSlrCPF::convertLagInterpError(lag_res);
        result.error = static_cast<std::uint32_t>(error);
    }
}

PredictorSlrCPF::PredictionError PredictorSlrCPF::convertLagInterpError(dpbase::stats::types::LagrangeError error)
{
    PredictorSlrCPF::PredictionError cpf_error = PredictionError::OTHER_ERROR;
    switch (error)
    {
    case dpbase::stats::types::LagrangeError::NOT_ERROR :
        cpf_error = PredictionError::NOT_ERROR; break;
    case dpbase::stats::types::LagrangeError::NOT_IN_THE_MIDDLE :
        cpf_error = PredictionError::INTERPOLATION_NOT_IN_THE_MIDDLE; break;
    case dpbase::stats::types::LagrangeError::X_OUT_OF_BOUNDS :
        cpf_error = PredictionError::X_INTERPOLATED_OUT_OF_BOUNDS; break;
    case dpbase::stats::types::LagrangeError::DATA_SIZE_MISMATCH :
        cpf_error = PredictionError::INTERPOLATION_DATA_SIZE_MISMATCH; break;
    }
    return cpf_error;
}

}}} // END NAMESPACES
// =====================================================================================================================
