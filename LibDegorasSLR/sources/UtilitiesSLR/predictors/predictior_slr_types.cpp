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
 * @file cpf_interpolator.cpp
 * @author Degoras Project Team.
 * @brief This file contains the implementation of the class CPFInterpolator.
 * @copyright EUPL License
 * @version 2306.1
***********************************************************************************************************************/

// C++ INCLUDES
//======================================================================================================================
#include <sstream>
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/UtilitiesSLR/predictors/predictor_slr_types.h"
#include "LibDegorasSLR/Helpers/string_helpers.h"
// =====================================================================================================================

// LIBDEGORASSLR NAMESPACES
// =====================================================================================================================namespace dpslr{
namespace dpslr{
namespace slr{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using namespace helpers::strings;
// ---------------------------------------------------------------------------------------------------------------------

std::string InstantRange::toJsonStr() const
{
    // Result
    std::ostringstream oss;

    // Generate the data.
    oss << "{";
    oss << "\"mjdt\":" << std::to_string(this->mjdt.datetime()) << ",";
    oss << "\"range_1w\":" << numberToStr(this->range_1w, 13, 3) << ",";
    oss << "\"tof_2w\":" << numberToStr(this->tof_2w, 13, 12) << ",";
    oss << "\"geo_pos\":" << this->geo_pos.toJsonStr();
    oss << "}";

    // Return the JSON str.
    return oss.str();
}

std::string InstantData::toJsonStr() const
{
    // Result
    std::ostringstream oss;

    // Generate the data.
    oss << "{";
    oss << "\"mjdt\":" << std::to_string(this->mjdt.datetime()) << ",";
    oss << "\"range_1w\":" << numberToStr(this->range_1w, 13, 3) << ",";
    oss << "\"tof_2w\":" << numberToStr(this->tof_2w, 13, 12) << ",";
    oss << "\"geo_pos\":" << this->geo_pos.toJsonStr() << ",";
    oss << "\"geo_vel\":" << this->geo_vel.toJsonStr() << ",";
    oss << "\"az\":" << numberToStr(this->altaz_coord.az, 7, 4) << ",";
    oss << "\"el\":" << numberToStr(this->altaz_coord.el, 7, 4);
    oss << "}";

    // Return the JSON str.
    return oss.str();
}

std::string InboundData::toJsonStr() const
{
    // Result
    std::ostringstream oss;

    // Generate the data.
    oss << "{";
    oss << "\"mjdt\":" << std::to_string(this->mjdt.datetime()) << ",";
    oss << "\"range_1w\":" << numberToStr(this->range_1w, 13, 3) << ",";
    oss << "\"tof_2w\":" << numberToStr(this->tof_2w, 13, 12);
    oss << "}";

    // Return the JSON str.
    return oss.str();
}

InstantData::InstantData(InstantRange &&instant_range) : InstantRange(std::move(instant_range)){}

std::string PredictionSLR::toJsonStr() const
{
    // Result
    std::ostringstream oss;
    oss << "{";

    // InstantRange
    oss << "\"instant_range\":" << instant_range.toJsonStr() << ",";

    // InstantData.
    oss << "\"instant_data\":" << (this->instant_data.has_value() ? this->instant_data->toJsonStr() : "null") << ",";

    // OutboundData.
    oss << "\"outbound_data\":"<<(this->outbound_data.has_value() ? this->outbound_data->toJsonStr() : "null")<<",";

    // InboundData.
    oss << "\"inbound_data\":"<<(this->inbound_data.has_value() ? this->inbound_data->toJsonStr() : "null")<<",";

    // Difference between receive and transmit direction at instant time.
    oss << "\"diff_az\":";
    oss << (this->diff_az.has_value() ? numberToStr(this->diff_az.value(), 4, 4) : "null") << ",";
    oss << "\"diff_el\":";
    oss << (this->diff_el.has_value() ? numberToStr(this->diff_el.value(), 4, 4) : "null") << ",";

    // Corrections applied.
    oss << "\"objc_ecc_corr\":";
    oss <<(this->objc_ecc_corr.has_value() ? std::to_string(this->objc_ecc_corr.value()) : "null") << ",";
    oss << "\"grnd_ecc_corr\":";
    oss <<(this->grnd_ecc_corr.has_value() ? std::to_string(this->grnd_ecc_corr.value()) : "null") << ",";
    oss << "\"cali_del_corr\":";
    oss <<(this->cali_del_corr.has_value() ? std::to_string(this->cali_del_corr.value()) : "null") << ",";
    oss << "\"corr_tropo\":";
    oss <<(this->corr_tropo.has_value() ? std::to_string(this->corr_tropo.value()) : "null") << ",";
    oss << "\"syst_rnd_corr\":";
    oss <<(this->syst_rnd_corr.has_value() ? std::to_string(this->syst_rnd_corr.value()) : "null");

    // End.
    oss << "}";

    // Return the JSON str.
    return oss.str();
}

}} // END NAMESPACES
// =====================================================================================================================
