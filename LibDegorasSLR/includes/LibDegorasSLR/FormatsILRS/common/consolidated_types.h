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
 * @file consolidated_types.h
 * @author Degoras Project Team.
 * @brief This file contains several types that are common in the ILRS standard consolidated formats.
 * @copyright EUPL License
 * @version 2305.1
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
//======================================================================================================================
#include <array>
#include <vector>
#include <map>
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace ilrs{
namespace common{
// =====================================================================================================================

// FORWARD DECLARATIONS
// -----------------------------------------------------------------------------------------------------------------
struct LIBDPSLR_EXPORT ConsolidatedRecord;
// -----------------------------------------------------------------------------------------------------------------

// RECORD IDENTIFIERS
// -----------------------------------------------------------------------------------------------------------------
/// Header record identifier strings.
static constexpr std::array<const char*, 5> HeaderIdStr{"H1","H2","H3","H4","H5"};
/// Configuration record identifier strigs.
static constexpr std::array<const char*, 8> CfgIdStr{"C0","C1","C2","C3","C4","C5","C6","C7"};
/// Data record identifier strings.
static constexpr std::array<const char*, 12> DataIdStr{"10","11","12","20","21","30","40","41","42","50","60","70"};
/// End records identifier strings.
static constexpr std::array<const char*, 3> EndIdStr{"H8","H9","99"};
/// Other record identifier strings.
static constexpr const char* CommentIdStr{"00"};
// -----------------------------------------------------------------------------------------------------------------

// ENUMERATIONS
// -----------------------------------------------------------------------------------------------------------------

/// @enum ConsolidatedFileTypeEnum
/// This enum represents the different types of ILRS Consolidated files.
enum class ConsolidatedFileType
{
    CRD_TYPE,      ///< Consolidated Record Data file.
    CPF_TYPE,      ///< Consolidated Prediction File file.
    UNKNOWN_TYPE   ///< Unknown file.
};

/// @enum RecordReadErrorEnum
/// This enum represents the errors that could happen when a CRD/CPF record (single line) is read.
enum class RecordReadError
{
    NOT_ERROR,             ///< No error.
    BAD_SIZE,              ///< Incorrect size errors.
    BAD_TYPE,              ///< Incorrect type errors.
    VERSION_MISMATCH,      ///< Version mismatch
    VERSION_UNKNOWN,       ///< Version unknow (no format header for crd, no basic information for cpf)
    CONVERSION_ERROR,      ///< Conversion error (string to number, string to bool, etc)
    DEPENDENCY_ERROR,      ///< Other struct/data that is necessary for reading the current line is missing.
    OTHER_ERROR,           ///< Other errors  (other exceptions, nullptr pointers, etc)
    NOT_IMPLEMENTED        ///< For not implemented situations.
};

// -----------------------------------------------------------------------------------------------------------------

// COMMON TYPE ALIAS
// -----------------------------------------------------------------------------------------------------------------

/// Vector that contais pairs. Each pair is pair(time_tag (s), time_of_flight (s)).
using FlightTimeData = std::vector<std::pair<long double, long double>>;

/// Pair(time_tag (s), residual (ps)).
template<typename T = long double, typename R = long double>
using ResidualData = std::pair<T, R>;

/// Vector that residual data
template<typename T = long double, typename R = long double>
using ResidualsData = std::vector<ResidualData<T,R>>;

/// Vector that contais pair of residuals.
template<typename T = long double, typename R = long double>
using ResidualBins = std::vector<ResidualsData<T,R>>;

/// Vector that contains tuples. Each tuple represents: <time_tag (s), tof (ps), pred_dist (ps), trop_corr (ps)>
using RangeData = std::vector<std::tuple<long double, long double, long double, long double>>;

/// Pair(record type enum, RecordLine).
using RecordLinePair = std::pair<int, ConsolidatedRecord>;

/// Pair(record type enum, RecordLine).
using RecordLinesVector = std::vector<ConsolidatedRecord>;

/// Map(record type enum, RecordLine). The record type can not be repeated.
using RecordLinesMap = std::map<int, ConsolidatedRecord>;

/// Map(record type enum, RecordLine). The record type enum can be repeated, used for for multiple records.
using RecordLinesMultimap = std::multimap<int, ConsolidatedRecord>;

/// Pair(RecordReadErrorEnum, RecordLine).
using RecordReadErrorPair = std::pair<int, ConsolidatedRecord>;

/// Multimap(RecordReadErrorEnum, RecordLine).
using RecordReadErrorMultimap = std::multimap<int, ConsolidatedRecord>;

// -----------------------------------------------------------------------------------------------------------------

}}} // END NAMESPACES
// =====================================================================================================================
