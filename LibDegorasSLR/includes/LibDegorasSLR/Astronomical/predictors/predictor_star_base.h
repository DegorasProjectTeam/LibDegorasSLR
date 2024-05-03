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
 * @file predictor_star_base.h
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
#include "LibDegorasSLR/Astronomical/types/star.h"
#include "LibDegorasSLR/Astronomical/predictors/data/prediction_star.h"
#include "LibDegorasSLR/Geophysics/types/surface_location.h"
#include "LibDegorasSLR/Mathematics/units/strong_units.h"
#include "LibDegorasSLR/Timing/dates/datetime_types.h"
#include "LibDegorasSLR/Helpers/common_aliases_macros.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace astro{
namespace predictors{
// =====================================================================================================================

/**
 * @brief The PredictorStar class provides functionality to predict the position of a star.
 *
 * This class utilizes astronomical algorithms to calculate the position of the star at a given time
 * and observer's location.
 */
class LIBDPSLR_EXPORT PredictorStarBase
{

public:

    // Copy and movement constructors and operators.
    M_DEFINE_CTOR_COPY_MOVE_OP_COPY_MOVE(PredictorStarBase)

    /**
     * @brief Constructs a PredictorStarBase object with the given observer's location.
     * @param star The parameters of the star.
     * @param loc The location of the observer.
     * @param leap_secs The leap seconds to apply.
     * @param ut1_utc_diff The difference between UT1 and UTC time systems to apply.
     */
    PredictorStarBase(const astro::types::Star& star,
                      const geo::types::SurfaceLocation<math::units::Degrees>& loc,
                      int leap_secs = 0, double ut1_utc_diff = 0);

    template <typename T, typename... Args>
    static std::shared_ptr<PredictorStarBase> factory(Args&&... args)
    {
        return std::static_pointer_cast<PredictorStarBase>(std::make_shared<T>(std::forward<Args>(args)...));
    }

    template <typename T>
    static std::shared_ptr<T> specialization(std::shared_ptr<PredictorStarBase> base)
    {
        return std::static_pointer_cast<T>(base);
    }

    /**
     * @brief Predicts the position of a star at a specific time
     *
     * @param jdt The Julian DateTime object representing the Julian date and time of the prediction.
     * @param refraction True if refraction model is to be applied, false otherwise.
     * @return The resulting PredictionStar.
     */
    virtual PredictionStar predict(const timing::dates::JDateTime& jdt, bool refraction) const = 0;

    /**
     * @brief Predicts star positions within a time range with a specified time step.
     *
     * @param jdt_start The Julian start datetime of the prediction range.
     * @param jdt_end The Julian end datetime of ñthe prediction range.
     * @param step The time step in milliseconds between predictions.
     * @param refraction True if refraction model is to be applied, false otherwise.
     * @return A vector of StarPrediction objects representing predicted star positions at each step.
     *
     * @throws std::invalid_argument If the interval is invalid.
     */
    virtual PredictionStarV predict(const timing::dates::JDateTime& jdt_start,
                                    const timing::dates::JDateTime& jdt_end,
                                    const math::units::MillisecondsU& step, bool refraction) const;

    virtual bool isReady() const = 0;

    virtual ~PredictorStarBase();

protected:

    types::Star star_;
    geo::types::SurfaceLocation<math::units::Degrees> loc_;
    int leap_secs_;
    double ut1_utc_diff_;
};

/// Alias for PredictorStarBase shared smart pointer.
using PredictorStarPtr = std::shared_ptr<PredictorStarBase>;

}}} // END NAMESPACES.
// =====================================================================================================================
