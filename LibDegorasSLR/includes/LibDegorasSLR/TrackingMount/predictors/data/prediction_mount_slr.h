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
 * @file prediction_mount_slr.h
 * @author Degoras Project Team.
 * @brief
 * @copyright EUPL License
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
#include "LibDegorasSLR/Helpers/common_aliases_macros.h"
#include "LibDegorasSLR/TrackingMount/utils/movement_analyzer/mount_position_analyzed.h"
#include "LibDegorasSLR/UtilitiesSLR/predictors/data/prediction_slr.h"
// =====================================================================================================================

// LIBDEGORASSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace mount{
namespace predictors{
// =====================================================================================================================

enum class PredictionMountSLRStatus : std::uint32_t
{
    VALID_PREDICTION     = 0,  ///< The SLR mount prediction is valid, all is ok.
    OUT_OF_TRACK         = 1,  ///< The SLR mount prediction is invalid, the provided time is outside of the movement.
    SLR_PREDICTION_ERROR = 3,  ///< The SLR mount prediction is invalid, an error in the SLR predictor has occured.
};

/**
 * @brief
 *
 * @note If the status of the MountPositionAnalyzed is `OUT_OF_TRACK`, the rest of data members will be invalid.
 */
struct LIBDPSLR_EXPORT PredictionMountSLR : utils::MountPositionAnalyzed
{
    // Default constructor, copy and movement constructor and operators.
    M_DEFINE_CTOR_DEF_COPY_MOVE_OP_COPY_MOVE_DTOR(PredictionMountSLR)

    PredictionMountSLR(const utils::MountPositionAnalyzed& pos, const slr::predictors::PredictionSLR& pred_slr) :
        utils::MountPositionAnalyzed(pos),
        slr_pred(pred_slr)
    {}

    // Result members.
    slr::predictors::PredictionSLR slr_pred;  ///< SLR prediction with all the SLR prediction related data.
    PredictionMountSLRStatus pred_status;     ///< Prediction status, related with the SLR prediction and analysis.
};


/// Alias for a vector of PredictionMountSLR objects.
using PredictionMountSLRV = std::vector<PredictionMountSLR>;

}}} // END NAMESPACES
// =====================================================================================================================
