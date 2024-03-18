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
 * @file tracking_analyzer.h
 * @author Degoras Project Team.
 * @brief This file contains the definition of the TrackingAnalyzer class.
 * @copyright EUPL License
 * @version
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
#include "LibDegorasSLR/Mathematics/units/strong_units.h"
#include "LibDegorasSLR/Astronomical/predictors/data/prediction_sun.h"
#include "LibDegorasSLR/TrackingMount/utils/tracking_analyzer/tracking_analysis.h"
#include "LibDegorasSLR/TrackingMount/types/tracking_mount_types.h"
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace mount{
namespace utils{
// =====================================================================================================================

// CONSTANTS
// ---------------------------------------------------------------------------------------------------------------------
constexpr math::units::Degrees kAvoidAngleOffset = 0.5L;   ///< Offset to apply to avoid the Sun sector.
// ---------------------------------------------------------------------------------------------------------------------



// If a not tracking movement (absolute, relative, continuous) starts at sun, the mount must go to error state.
// If it ends at sun, the movment is not valid.

// TODO LIST:

// Analisis de los límites físicos de velocidad.


/**
 * @brief
 *
 * Can be used to analyze a tracking or absolute mount movement.
 *
 * This class uses a PredictorSLR to look for a SLR tracking within the given time window, i.e., the object is
 * always above the minimum elevation.
 *
 * This class also offers a sun avoidance algorithm. This algorithm changes the tracking trajectory whenever it passes
 * through a circular sector around the sun (the sun security sector). There are other checkings performed to the
 * tracking by this class. Before using the tracking you should check whether the tracking is valid or not.
 *
 *@todo Max speed analysis.
 */
class LIBDPSLR_EXPORT TrackingAnalyzer
{
public:

    TrackingAnalyzer(const TrackingAnalyzerConfig& config);

    TrackingAnalysis analyzeTracking(const types::MountPositionV& mount_positions,
                                     const astro::types::SunPositionV &sun_positions) const;

    MountPositionAnalyzed analyzePosition(const TrackingAnalysis& analysis,
                                          const types::MountPosition& mount_pos,
                                          const astro::types::SunPosition& sun_pos) const;

private:

    /// Helper to check the tracking start.
    bool analyzeTrackingStart(TrackingAnalysis& analysis) const;

    /// Helper to check the tracking end.
    bool analyzeTrackingEnd(TrackingAnalysis& analysis) const;

    /// Helper to check the tracking at the middle.
    bool analyzeTrackingMiddle(TrackingAnalysis& analysis) const;

    /// Helper to check if the predicted position is inside a sun sector.
    bool insideSunSector(const astro::types::AltAzPos& pass_pos, const astro::types::AltAzPos& sun_pos) const;

    /// Helper to set the rotation direction of a sun sector.
    bool setSunSectorRotationDirection(SunCollisionSector &sector,
                                       MountPositionAnalyzedV::const_iterator sun_start,
                                       MountPositionAnalyzedV::const_iterator sun_end);

    /// Helper to check positions whithin a sun sector to see if it is possible to avoid sun
    void checkSunSectorPositions(const SunCollisionSector &sector,
                                 MountPositionAnalyzedV::iterator sun_start,
                                 MountPositionAnalyzedV::iterator sun_end);

    /// Helper to calculate the avoidance trajectory.
    long double calcSunAvoidTrajectory(const timing::types::MJDateTime& mjdt, const SunCollisionSector& sector,
                                       const astro::types::AltAzPos& sun_pos) const;

    void calcSunAvoidPos(MountPositionAnalyzed& pred, const SunCollisionSector &sector) const;

    // Private data members.
    TrackingAnalyzerConfig config_; ///< Contains the tracking analyzer user configuration.
};

}}} // END NAMESPACES
// =====================================================================================================================
