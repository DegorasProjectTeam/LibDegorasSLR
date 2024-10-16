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
 * @file predictor_sun_base.h
 * @brief
 * @author Degoras Project Team
 * @copyright EUPL License
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
// =====================================================================================================================
#include <memory>
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
#include "LibDegorasSLR/Geophysics/types/geodetic_point.h"
#include "LibDegorasSLR/Astronomical/predictors/data/prediction_sun.h"
// =====================================================================================================================

// LIBDPBASE INCLUDES
// =====================================================================================================================
#include "LibDegorasBase/Mathematics/units/strong_units.h"
#include "LibDegorasBase/Timing/dates/datetime_types.h"
#include "LibDegorasBase/Helpers/common_aliases_macros.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace astro{
namespace predictors{
// =====================================================================================================================

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

    // Default copy and movement constructor and operators.
    M_DEFINE_CTOR_COPY_MOVE_OP_COPY_MOVE(PredictorSunBase)

    /**
     * @brief Constructs a PredictorSunBase object with the given observer's geodetic coordinates.
     * @param obs_geod The geodetic coordinates of the observer.
     */
    PredictorSunBase(const geo::types::GeodeticPointDeg& obs_geod);

    /**
     * @brief Creates a shared pointer that internally is an object of type T, derived from PredictorSunBase.
     *
     * This function template dynamically creates an instance of type T. It then returns a shared pointer to the base
     * class PredictorSunBase by using a static pointer cast. This is useful for creating instances of derived classes
     * when only the base class type is known at compile time.
     *
     * @tparam T The type of the derived class to instantiate. Must be a subclass of PredictorSunBase.
     * @tparam Args The types of the arguments to forward to the constructor of T.
     * @param args The arguments to forward to the constructor of T.
     * @return A shared pointer to the newly created instance of T, cast to a shared pointer of type PredictorSunBase.
     */
    template <typename T, typename... Args>
    static std::shared_ptr<PredictorSunBase> factory(Args&&... args)
    {
        return std::static_pointer_cast<PredictorSunBase>(std::make_shared<T>(std::forward<Args>(args)...));
    }

    /**
     * @brief Casts a shared pointer of the base class PredictorSunBase to a shared pointer of a derived class T.
     *
     * This function template takes a shared pointer to an instance of the base class PredictorSunBase and performs a
     * static cast to convert it into a shared pointer of type T, where T is a derived class of PredictorSunBase. This
     * is useful for downcasting shared pointers when the type of the derived class is known.
     *
     * @tparam T The target type to which the shared pointer should be cast. Must be a subclass of PredictorSunBase.
     * @param base A shared pointer to an instance of PredictorSunBase.
     * @return A shared pointer to the base instance, cast to type T.
     */
    template <typename T>
    static std::shared_ptr<T> specialization(std::shared_ptr<PredictorSunBase> base)
    {
        return std::static_pointer_cast<T>(base);
    }

    /**
     * @brief Predicts the position of the Sun at a specific time.
     * @param j2000 The J2000DateTime object representing the J2000 date and time of the prediction.
     * @param refraction Flag indicating whether to apply atmospheric refraction correction.
     * @return The predicted PredictionSun.
     */
    virtual PredictionSun predict(const dpbase::timing::dates::J2000DateTime& j2000, bool refraction) const = 0;

    /**
     * @brief Predicts the position of the Sun within a time range with a specified time step.
     * @param j2000_start The J2000 start datetime of the prediction range.
     * @param j2000_end The J2000 end datetime of the prediction range.
     * @param step The time step in milliseconds between predictions.
     * @param refraction Flag indicating whether to apply atmospheric refraction correction.
     * @return A vector of PredictionSun objects representing predicted sun positions at each step.
     * @throws std::invalid_argument If the interval is invalid.
     */
    virtual PredictionSunV predict(const dpbase::timing::dates::J2000DateTime& j2000_start,
                                   const dpbase::timing::dates::J2000DateTime& j2000_end,
                                   const dpbase::math::units::MillisecondsU& step, bool refraction) const;

    virtual bool isReady() const = 0;

    virtual ~PredictorSunBase();

protected:

    geo::types::GeodeticPointRad obs_geo_pos_; ///< Geodetic observer point (radians and meters).
};

/// Alias for PredictorSunBase shared smart pointer.
using PredictorSunPtr = std::shared_ptr<PredictorSunBase>;

}}} // END NAMESPACES.
// =====================================================================================================================
