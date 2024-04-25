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
 * @file predictor_mount_movement.h
 * @author Degoras Project Team.
 * @brief This file contains the definition of the PredictorMountMove class.
 * @copyright EUPL License
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/TrackingMount/predictors/data/mount_tracking_movement.h"
#include "LibDegorasSLR/libdegorasslr_global.h"
#include "LibDegorasSLR/Astronomical/predictors/predictor_sun_base.h"
#include "LibDegorasSLR/Timing/types/base_time_types.h"
#include "LibDegorasSLR/TrackingMount/types/mount_position.h"
#include "LibDegorasSLR/TrackingMount/utils/movement_analyzer/movement_analyzer_config.h"
#include "LibDegorasSLR/TrackingMount/utils/movement_analyzer/movement_analysis.h"
#include "LibDegorasSLR/TrackingMount/utils/movement_analyzer/movement_analyzer.h"
#include "LibDegorasSLR/TrackingMount/predictors/data/prediction_mount_movement.h"
// =====================================================================================================================

// C++ INCLUDES
// =====================================================================================================================
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace mount{
namespace predictors{
// =====================================================================================================================


/**
 * @brief The PredictorMountMove class implements a simple predictor that receives the positions of a mount movement
 * and analyzes them. After this analyze it can be used to predict new positions within the time window of the movement.
 * This positions will also avoid sun if requested.
 */
class LIBDPSLR_EXPORT PredictorMountMovement
{
public:

    // Default copy and movement constructor and operators.
    M_DEFINE_CTOR_COPY_MOVE_OP_COPY_MOVE(PredictorMountMovement)

    PredictorMountMovement(types::MountPositionV positions,
                           astro::predictors::PredictorSunPtr pred_sun,
                           const utils::MovementAnalyzerConfig& config);

    /**
     * @brief This function checks if the predictor has a valid movement and is ready for predicting.
     * @return true if there is a valid movement, false otherwise.
     */
    bool isReady() const;

    /**
     * @brief This function returns the mount tracking information for this movement.
     * @return the struct containing all the info about the mount tracking movement.
     */
    const MountTrackingMovement& getMountTrackingMovement() const;

    /**
     * @brief This function returns the object's position at a given time.
     * @param tp The time point datetime.
     * @param tracking_result, the returned TrackingResult struct.
     * @return the result of the operation. Must be checked to ensure the position is valid.

     */
    PredictionMountMovement predict(const timing::types::HRTimePointStd& tp) const;


private:

    // Private functions.

    // Helper to analyze the track.
    void analyzeTracking();

    bool checkPositions(const types::MountPositionV& positions) const;

    astro::types::AltAzPos interpPos(const timing::types::HRTimePointStd &tp) const;


    // Private members.
    MountTrackingMovement mount_track_;
    utils::MovementAnalyzer mv_analyzer_;   ///< Mount movement analyzer used.
    types::MountPositionV positions_;       ///< Original positions given at construction
};

}}} // END NAMESPACES
// =====================================================================================================================
