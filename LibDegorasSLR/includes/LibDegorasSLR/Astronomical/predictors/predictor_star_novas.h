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
 * @file predictor_star_novas.h
 * @brief
 * @author Degoras Project Team
 * @copyright EUPL License
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
// =====================================================================================================================
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
#include "LibDegorasSLR/Astronomical/predictors/predictor_star_base.h"
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
class LIBDPSLR_EXPORT PredictorStarNovas : public PredictorStarBase
{

public:

    PredictorStarNovas(const astro::types::Star& star, const geo::types::SurfaceLocation<dpbase::math::units::Degrees>& loc,
                       int leap_secs = 0, double ut1_utc_diff = 0);

    /**
     * @brief Predicts the position of a star at a specific time
     *
     * @param jdt The Julian DateTime object representing the Julian date and time of the prediction.
     * @return The resulting PredictionStar.
     */
    PredictionStar predict(const dpbase::timing::dates::JDateTime& jdt, bool refraction) const override;

    /**
     * @brief Predicts star positions within a time range with a specified time step.
     *
     * @warning This function is does not perform computations in parallel, since Novas is not thread safe.
     *
     * @param jdt_start   The Julian start datetime of the prediction range.
     * @param jdt_end     The Julian end datetime of ñthe prediction range.
     * @param step        The time step in milliseconds between predictions.
     * @param refraction  True if refraction model is to be applied, false otherwise.
     * @return A vector of StarPrediction objects representing predicted star positions at each step.
     *
     * @throws std::invalid_argument If the interval is invalid.
     */
    PredictionStarV predict(const dpbase::timing::dates::JDateTime& jdt_start,
                            const dpbase::timing::dates::JDateTime& jdt_end,
                            const dpbase::math::units::MillisecondsU& step, bool refraction) const override;

    virtual bool isReady() const override {return true;}

};

}}} // END NAMESPACES.
// =====================================================================================================================
