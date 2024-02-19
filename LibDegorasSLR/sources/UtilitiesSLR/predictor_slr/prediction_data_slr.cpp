/***********************************************************************************************************************
 *   LibDPSLR (Degoras Project SLR Library): A libre base library for SLR related developments.                        *                                      *
 *                                                                                                                     *
 *   Copyright (C) 2023 Degoras Project Team                                                                           *
 *                      < Ángel Vera Herrera, avera@roa.es - angeldelaveracruz@gmail.com >                             *
 *                      < Jesús Relinque Madroñal >                                                                    *
 *                                                                                                                     *
 *   This file is part of LibDPSLR.                                                                                    *
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
#include <LibDegorasSLR/UtilitiesSLR/predictor_slr/prediction_data_slr.h>
#include <LibDegorasSLR/Helpers/string_helpers.h>
// =====================================================================================================================

// LIBDEGORASSLR NAMESPACES
// =====================================================================================================================namespace dpslr{
namespace dpslr{
namespace utils{
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
    oss << "\"mjd\":" << this->mjd << ",";
    oss << "\"sod\":" << this->sod << ",";
    oss << "\"mjdt\":" << std::to_string(this->mjdt) << ",";
    oss << "\"range_1w\":" << numberToStr(this->range_1w, 13, 3) << ",";
    oss << "\"tof_2w\":" << numberToStr(this->tof_2w, 13, 12) << ",";
    oss << "\"geo_pos\":" << this->geo_pos.toJson();
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
    oss << "\"mjd\":" << this->mjd << ",";
    oss << "\"sod\":" << this->sod << ",";
    oss << "\"mjdt\":" << std::to_string(this->mjdt) << ",";
    oss << "\"range_1w\":" << numberToStr(this->range_1w, 13, 3) << ",";
    oss << "\"tof_2w\":" << numberToStr(this->tof_2w, 13, 12) << ",";
    oss << "\"geo_pos\":" << this->geo_pos.toJson() << ",";
    oss << "\"geo_vel\":" << this->geo_vel.toJson() << ",";
    oss << "\"az\":" << numberToStr(this->az, 7, 4) << ",";
    oss << "\"el\":" << numberToStr(this->el, 7, 4);
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
    oss << "\"mjd\":" << this->mjd << ",";
    oss << "\"sod\":" << this->sod << ",";
    oss << "\"mjdt\":" << std::to_string(this->mjdt) << ",";
    oss << "\"range_1w\":" << numberToStr(this->range_1w, 13, 3) << ",";
    oss << "\"tof_2w\":" << numberToStr(this->tof_2w, 13, 12);
    oss << "}";

    // Return the JSON str.
    return oss.str();
}

InstantData::InstantData(InstantRange &&instant_range) : InstantRange(std::move(instant_range)){}

}} // END NAMESPACES
// =====================================================================================================================
