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
 * @file predictor_sun.h
 * @brief
 * @author Degoras Project Team.
 * @copyright EUPL License
 * @version
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
//======================================================================================================================
#include <memory>
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
#include "LibDegorasSLR/Geophysics/types/geocentric_point.h"
#include "LibDegorasSLR/Geophysics/types/geodetic_point.h"
#include "LibDegorasSLR/Timing/types/base_time_types.h"
#include "LibDegorasSLR/Astronomical/types/astro_types.h"
#include "LibDegorasSLR/Mathematics/units/strong_units.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace astro{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using timing::types::J2000DateTime;
using timing::types::J2000DateTimeV;
using timing::types::MJDate;
using timing::types::SoD;
using timing::types::MJDateTime;
using timing::types::Seconds;
using geo::types::GeodeticPoint;
using geo::types::GeocentricPoint;
using math::units::MillisecondsU;
using math::units::Degrees;
using math::units::Radians;
using astro::types::AltAzPos;
// ---------------------------------------------------------------------------------------------------------------------

struct LIBDPSLR_EXPORT SunPrediction
{
    // Default constructor.
    SunPrediction() = default;

    SunPrediction(const J2000DateTime& j2000, const AltAzPos& altaz_coord, const GeocentricPoint& geo_pos) :
        j2dt(j2000), altaz_coord(altaz_coord), geo_pos(geo_pos)
    {}

    // Containers.
    J2000DateTime j2dt;        ///< J2000 datetime used to generate the Sun prediction data.
    AltAzPos altaz_coord;      ///< Sun predicted altazimuth coordinates referenced to an observer (degrees).
    GeocentricPoint geo_pos;   ///< Sun predicted geocentric position in meters.

    // TODO Calculate also position vectors, neccesary to check non visible moments in space object passes.

};

/// Alias for a vector of SunPrediction.
using SunPredictionV = std::vector<SunPrediction>;


/**
 * @brief The PredictorSun class provides functionality to predict the position of the Sun.
 *
 * This class utilizes astronomical algorithms to calculate the azimuth and elevation of the Sun at a given time
 * and observer's geodetic coordinates.
 *
 * @warning At this moment, only the function fastPredict (0.01 degree accuracy) is implemented.
 */
class LIBDPSLR_EXPORT PredictorSunBase
{

public:

    /**
     * @brief Constructs a PredictorSunBase object with the given observer's geodetic coordinates.
     * @param obs_geod The geodetic coordinates of the observer.
     */
    PredictorSunBase(const GeodeticPoint<Degrees>& obs_geod);

    PredictorSunBase(const PredictorSunBase&) = default;
    PredictorSunBase(PredictorSunBase&&) = default;
    PredictorSunBase& operator =(const PredictorSunBase&) = default;
    PredictorSunBase& operator =(PredictorSunBase&&) = default;

    template <typename T, typename... Args>
    static std::shared_ptr<PredictorSunBase> factory(Args&&... args)
    {
        return std::static_pointer_cast<PredictorSunBase>(std::make_shared<T>(std::forward<Args>(args)...));
    }

    virtual ~PredictorSunBase() = default;

    /**
     * @brief Predicts the position of the Sun at a specific time.
     *
     * @param j2000 The J2000DateTime object representing the J2000 date and time of the prediction.
     * @param refraction Flag indicating whether to apply atmospheric refraction correction.
     * @return The predicted SunPrediction.
     */
    virtual SunPrediction predict(const J2000DateTime& j2000, bool refraction) const = 0;

    /**
     * @brief Predicts Sun positions within a time range with a specified time step.
     *
     * @param j2000_start The J2000 start datetime of the prediction range.
     * @param j2000_end The J2000 end datetime of the prediction range.
     * @param step The time step in milliseconds between predictions.
     * @param refraction Flag indicating whether to apply atmospheric refraction correction.
     * @return A vector of SunPrediction objects representing predicted sun positions at each step.
     *
     * @throws std::invalid_argument If the interval is invalid.
     */
    virtual SunPredictionV predict(const J2000DateTime& j2000_start, const J2000DateTime& j2000_end,
                                   const MillisecondsU& step, bool refraction) const
    {
        // Container and auxiliar.
        J2000DateTimeV interp_times;
        Seconds step_sec = static_cast<long double>(step) * math::units::kMsToSec;

        // Check for valid time interval.
        if(!(j2000_start <= j2000_end))
            throw std::invalid_argument("[LibDegorasSLR,Astronomical,PredictorSun::predict] Invalid interval.");

        // Calculates all the interpolation times.
        interp_times = J2000DateTime::linspaceStep(j2000_start, j2000_end, step_sec);

        // Results container.
        SunPredictionV results(interp_times.size());

        // Parallel calculation.
        #pragma omp parallel for
        for(size_t i = 0; i<interp_times.size(); i++)
        {
            results[i] = this->predict(interp_times[i], refraction);
        }

        // Return the container.
        return results;
    }

protected:

    GeodeticPoint<Radians> obs_geo_pos_;  ///< Geodetic observer position (radians and meters).
};

/// Alias for PredictorSun unique smart pointer.
using PredictorSunPtr = std::shared_ptr<PredictorSunBase>;

}} // END NAMESPACES.
// =====================================================================================================================
