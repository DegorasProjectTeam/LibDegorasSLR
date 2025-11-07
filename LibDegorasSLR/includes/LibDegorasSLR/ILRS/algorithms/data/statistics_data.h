/***********************************************************************************************************************
 *   LibDPSLR (Degoras Project SLR Library): A libre base library for SLR related developments.                        *                                      *
 *                                                                                                                     *
 *   Copyright (C) 2024 Degoras Project Team                                                                           *
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
 * @file statistics_data.h
 * @author Degoras Project Team.
 * @brief This file contains the definition of the data types related to ILRS statistics algorithms.
 * @copyright EUPL License
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
// =====================================================================================================================
#include <vector>
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
#include <LibDegorasSLR/Geophysics/types/meteo_data.h>
// =====================================================================================================================

// LIBDPBASE INCLUDES
// =====================================================================================================================
#include <LibDegorasBase/Timing/dates/base_date_types.h>
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace ilrs{
namespace algorithms{
// =====================================================================================================================

// ========== ENUMS ====================================================================================================

enum class BinDivisionEnum
{
    DAY_FIXED             = 0
};

/**
 * @enum FullRateResCalcErr
 * @brief This enum represents the errors that could happen at full rate residuals calculation.
 */
enum class FullRateResCalcErr
{
    NOT_ERROR              = 0,      ///< No error flag activated.
    CPF_DATA_EMPTY        = 1,      ///< CPF is empty or is not valid.
    CRD_CFG_NOT_VALID     = 2,      ///< CRD has no System Configuration record.
    CRD_DATA_EMPTY        = 3,      ///< CRD Full Rate data is empty.
    RESIDS_CALC_FAILED    = 4,      ///< The residuals calculation failed.
};

/**
 * @enum BinStatsCalcErr
 * @brief This enum represents the errors that could happen at residuals statistics calculation.
 */
enum class BinStatsCalcErr
{
    NOT_ERROR                = 0,    ///< No error flag activated.
    REJECTED_RFRMS          = 1,    ///< All points rejected when forming RF*RMS mean.
    NOT_CONVERGED_RFRMS     = 2,    ///< The RF*RMS mean forming process did not converge.
    PEAK_CALC_FAILED_RFRMS  = 3,    ///< The peak calculation fails for RF*RMS.
    REJECTED_1RMS           = 4,    ///< All points rejected when forming 1*RMS mean.
    NOT_CONVERGED_1RMS      = 5,    ///< The 1*RMS mean forming process did not converge.
    PEAK_CALC_FAILED_1RMS   = 6     ///< The peak calculation fails for 1*RMS.
};

/// @enum ResiStatsCalcError
/// @brief This enum represents the errors that could happen at Statistics Record statistics calculation.
enum class ResiStatsCalcErr
{
    NOT_ERROR = 0,                 ///< No error flag activated.
    SOME_BINS_CALC_FAILED = 1,    ///< Some bins statistics calculation failed
    STATS_CALC_FAILED = 2,        ///< All bins statistics calculation failed
};

// COMMON TYPE ALIAS
// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
struct FlightTimeData
{
    long double ts;            ///< Timestamp of range in seconds of day.
    long double tof;           ///< Time of flight 2ways in seconds.
};

using FlightTimeDataV = std::vector<FlightTimeData>;

struct FullRateData
{
    dpbase::timing::dates::MJDate mjd;
    FlightTimeDataV ft_data;
    geo::types::MeteoRecordV meteo_data;
};

struct RangeData
{
    long double ts;             ///< Timestamp of range in seconds.
    long double tof;            ///< Time of flight in picoseconds.
    long double pred_dist;      ///< Predicted time of flight in picoseconds.
    long double trop_corr;      ///< Troposferical correction in picoseconds.
    long double resid;          ///< Calculated residual in picosecond.
};

using RangeDataV = std::vector<RangeData>;

/// @struct DistStats
struct LIBDPSLR_EXPORT DistStats
{
    std::size_t iter;              ///< Iterations until converging.
    std::size_t aptn;              ///< Number of accepted points used for statistics.
    std::size_t rptn;              ///< Number of rejected points.
    long double mean;              ///< Mean of residuals.
    long double rms;               ///< RMS from the mean of residuals.
    long double skew;              ///< Skewness of residuals.
    long double kurt;              ///< Kurtosis of residuals (value of normal dist is 0).
    long double peak;              ///< Peak of residuals.
    double arate;                  ///< Accepted rate (%).
};

/// @struct BinStats
/// @note If error, all points will be rejected. So the rptn of ::DistStats will be equal to the ptn, and all the mask
///       vectors will be false.
struct LIBDPSLR_EXPORT BinStats
{
    double rf;                      ///< Reject factor (RF) for the bin.
    std::size_t ptn;                ///< Total number of points in the bin.
    DistStats stats_rfrms;          ///< Distribution statistics around RF*RMS after converging.
    DistStats stats_01rms;          ///< Distribution statistics around 1*RMS after converging.
    std::vector<bool> amask_rfrms;  ///< Mask that represents if the data in a certain position is accepted for RF*RMS.
    std::vector<bool> amask_01rms;  ///< Mask that represents if the data in a certain position is accepted for 1*RMS.
    BinStatsCalcErr error;          ///< Stores the error. See ::BinStatsCalcError for more details.
};

/// @struct ResidualsStats
/// @note The mean data is calculated only with the bins without errors.
struct LIBDPSLR_EXPORT ResidualsStats
{
    double rf;                      ///< Reject factor (RF). Usually 3 when using multi-photon or 2.5 for single-photon.
    std::size_t bs;                 ///< Bin size in seconds.
    BinStats total_bin_stats;       ///< Final statistics calculated over every bin.
    std::vector<BinStats> bins;     ///< Vector with the statistics of each bin.
    ResiStatsCalcErr error;         ///< Stores the error. See ::ResiStatsCalcErr for more information.
};

// =====================================================================================================================

}}} // END NAMESPACES
// =====================================================================================================================
