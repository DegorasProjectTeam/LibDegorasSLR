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
 * @file predictor_slr.cpp
 * @author Degoras Project Team.
 * @brief This file contains the implementation of the class PredictorSLR.
 * @copyright EUPL License
 * @version 2306.1
***********************************************************************************************************************/

// C++ INCLUDES
//======================================================================================================================
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/UtilitiesSLR/predictors/predictor_slr_base.h"
#include "LibDegorasSLR/Mathematics/math.h"
#include "LibDegorasSLR/Astronomical/astro_constants.h"
#include "LibDegorasSLR/Geophysics/tropo.h"
// =====================================================================================================================

// LIBDEGORASSLR NAMESPACES
// =====================================================================================================================namespace dpslr{
namespace dpslr{
namespace utils{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using namespace astro;
using namespace helpers::strings;
using namespace math;
using namespace math::units;
using namespace math::types;
// ---------------------------------------------------------------------------------------------------------------------

PredictorSLR::PredictorSLR(const GeodeticPoint<long double> &geod, const GeocentricPoint &geoc) :
    tropo_model_(TroposphericModel::MARINI_MURRAY),
    objc_ecc_corr_(0.0L),
    grnd_ecc_corr_(0.0L),
    syst_rnd_corr_(0.0L),
    cali_del_corr_(0.0L),
    apply_corr_(false),
    press_(0.0L),
    temp_(0.0L),
    rel_hum_(0.0L),
    wl_(0.0L),
    wtrvap_model_(WtrVapPressModel::GIACOMO_DAVIS),
    tropo_ready_(false),
    prediction_mode_(PredictionMode::INSTANT_VECTOR),
    stat_geodetic_(geod),
    stat_geocentric_(geoc.toVector3D())
{
    // Store latitude and longitude in radians.
    this->stat_geodetic_.convert(math::units::Angle<long double>::Unit::RADIANS,
                                 math::units::Distance<long double>::Unit::METRES);
}



void PredictorSLR::setTropoModel(TroposphericModel model){this->tropo_model_ = model;}

void PredictorSLR::enableCorrections(bool enable){this->apply_corr_ = enable;}

void PredictorSLR::setObjEccentricityCorr(Meters correction){this->objc_ecc_corr_ = correction;}

void PredictorSLR::setCaliDelayCorr(Picoseconds correction){this->cali_del_corr_ =correction;}

void PredictorSLR::setSystematicCorr(Meters correction){this->syst_rnd_corr_ = correction;}

void PredictorSLR::setTropoCorrParams(long double press, long double temp, long double rh,
                                      long double wl, WtrVapPressModel wvpm)
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

const GeodeticPoint<long double>& PredictorSLR::getGeodeticLocation() const {return this->stat_geodetic_;}

const GeocentricPoint& PredictorSLR::getGeocentricLocation() const {return this->stat_geocentric_;}

void PredictorSLR::setPredictionMode(PredictionMode mode)
{
    this->prediction_mode_ = mode;
}

PredictorSLR::PredictionMode PredictorSLR::getPredictionMode() const
{
    return this->prediction_mode_;
}

bool PredictorSLR::isInsideTimeWindow(const MJDateTime& start, const MJDateTime& end) const
{
    // Auxiliar.
    MJDateTime predict_mjd_start, predict_mjd_end;

    // Get the predict time window.
    this->getTimeWindow(predict_mjd_start, predict_mjd_end);

    // Check if requested window is inside predict time window
    return start >= predict_mjd_start && end <= predict_mjd_end;
}

Meters PredictorSLR::applyCorrections(Meters& range, SLRPrediction& result, bool cali, Degrees el) const
{
    // Auxiliar provisional range.
    Meters provisional_range = range;

    // Include the half of the system delay to the range. This will be permanent for the rest of computations.
    if(math::compareFloating(this->cali_del_corr_, Picoseconds(0.0L)) && cali)
    {
        range += 0.5L*this->cali_del_corr_*kC*kPsToSec;
        result.cali_del_corr = this->cali_del_corr_;
        provisional_range = range;
    }

    // Include the object eccentricity correction.
    if(math::compareFloating(this->objc_ecc_corr_, Meters(0.0L)))
    {
        provisional_range = provisional_range - this->objc_ecc_corr_;
        result.objc_ecc_corr = this->objc_ecc_corr_;
    }

    // Include the ground eccentricity correction.
    if(math::compareFloating(this->grnd_ecc_corr_, Meters(0.0L)))
    {
        provisional_range = provisional_range + this->grnd_ecc_corr_;
        result.grnd_ecc_corr = this->grnd_ecc_corr_;
    }

    // Include the systematic and random errors.
    if(math::compareFloating(this->syst_rnd_corr_, Meters(0.0L)))
    {
        provisional_range = provisional_range + this->syst_rnd_corr_;
        result.syst_rnd_corr = this->syst_rnd_corr_;
    }

    // Compute and include the tropospheric path delay.
    if(math::compareFloating(el, Degrees(0.0L)))
    {
        if(this->tropo_model_ == PredictorSLR::TroposphericModel::MARINI_MURRAY)
        {
            // Get the elevation in radians.
            long double el_instant_rad = math::units::degToRad(el);

            // Calculate the tropospheric path delay (1 way).
            range += geo::tropo::pathDelayMariniMurray(this->press_, this->temp_, this->rel_hum_, el_instant_rad,
                                                       this->wl_, this->stat_geodetic_.lat, this->stat_geodetic_.alt,
                                                       this->wtrvap_model_);
            // Store the range.
            provisional_range = range;
        }
        else
        {
            throw std::runtime_error(
                "[LibDegorasSLR,UtilitiesSLR,PredictorSLR, predict] Tropospheric model not implemented.");
        }
    }

    // Return the new range with the corrections.
    return provisional_range;
}

PredictorSLR::~PredictorSLR()
{}

}} // END NAMESPACES
// =====================================================================================================================
