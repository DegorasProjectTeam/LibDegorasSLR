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
 * @file predictor_mount_slr.h
 * @author Degoras Project Team.
 * @brief This file contains the definition of the PredictorMountSLR class.
 * @copyright EUPL License
 * @version
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
#include "LibDegorasSLR/TrackingMount/types/tracking_types.h"
#include "LibDegorasSLR/TrackingMount/types/tracking_analyzer.h"
// =====================================================================================================================

// C++ INCLUDES
// =====================================================================================================================
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace mount{
// =====================================================================================================================

/**
 * @brief The PredictorMountSLR class implements a mount predictor for SLR trackings. This predictor calculates the
 * mount position at each timestamp within a valid SLR tracking, controlling the elevation limits and the existence of
 * sectors where the tracking passes through the sun, giving an alternative way if requested.
 *
 * @todo Checking other limits, like speed
 */
class LIBDPSLR_EXPORT PredictorMountSLR
{
public:

    /**
     * @brief PredictorMountSLR constructor.
     * @param pass_start, the modified julian datetime for pass start.
     * @param pass_end, the modified julian datetime for pass end.
     * @param pred_slr, the SLR predictor to be used for SLR object predictions.
     * @param pred_sun, the sun predictor to be used for sun position predictions.
     * @param config, the configuration parameters for the tracking analysis.
     */
    PredictorMountSLR(const timing::types::MJDateTime& pass_start, const timing::types::MJDateTime& pass_end,
                      slr::PredictorSlrPtr pred_slr, astro::PredictorSunPtr pred_sun,
                      const TrackingAnalyzerConfig& config);
    /**
     * @brief PredictorMountSLR constructor.
     * @param pass_start, the modified julian datetime for pass start.
     * @param pass_end, the modified julian datetime for pass end.
     * @param pred_slr, the SLR predictor to be used for SLR object predictions.
     * @param pred_sun, the sun predictor to be used for sun position predictions.
     * @param config, the configuration parameters for the tracking analysis.
     */
    PredictorMountSLR(const timing::types::HRTimePointStd& pass_start, const timing::types::HRTimePointStd& pass_end,
                      slr::PredictorSlrPtr pred_slr, astro::PredictorSunPtr pred_sun,
                      const TrackingAnalyzerConfig &config);

    /**
     * @brief This function checks if there is a valid SLR tracking. You should check this, before requesting positions.
     * @return true if there is a valid tracking, false otherwise.
     */
    bool isReady() const;

    /**
     * @brief This function returns the mount tracking information.
     * @return the struct containing all the info about the mount tracking.
     */
    const MountTrackingSLR& getMountTrackingSLR() const;

    /**
     * @brief This function returns the object's position at a given time.
     * @param tp_time The time point datetime.
     * @param tracking_result, the returned TrackingResult struct.
     * @return the result of the operation. Must be checked to ensure the position is valid.
     *
     * @warning Nanoseconds resolution for the prediction.
     */
    PositionStatus predict(const timing::types::HRTimePointStd& tp_time, MountPredictionSLR &tracking_result) const;

    /**
     * @brief This function returns the object's position at a given time.
     * @param mjd, the modified julian datetime.
     * @param tracking_result, the returned TrackingResult struct.
     * @return the result of the operation. Must be checked to ensure the position is valid.
     */
    PositionStatus predict(const timing::types::MJDateTime &mjd, MountPredictionSLR &tracking_result) const;

private:

    // Private functions.

    /// Helper to analyze the track.
    void analyzeTracking();


    // Private members.
    MountTrackingSLR mount_track_;                     ///< Mount track analyzed data.
    TrackingAnalyzer tr_analyzer_;                     ///< Tracking analyzer used.
};

}} // END NAMESPACES
// =====================================================================================================================
