/***********************************************************************************************************************
 * Copyright 2023 Degoras Project Team
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they will be approved by the
 * European Commission - subsequent versions of the EUPL (the "Licence");
 *
 * You may not use this work except in compliance with the Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the Licence is distributed on
 * an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the Licence for the
 * specific language governing permissions and limitations under the Licence.
 **********************************************************************************************************************/

#pragma once

// ========== DPSLR INCLUDES ===========================================================================================
#include "libdpslr_global.h"
#include "class_crd.h"
#include "class_cpf.h"
#include "geo.h"
// =====================================================================================================================


// ========== DPSLR NAMESPACES =========================================================================================
namespace dpslr{
namespace crdutils{
// =====================================================================================================================


// ========== ENUMS ====================================================================================================
/**
 * @enum OverallCaliGencErr
 * @brief This enum represents the errors that could happen at overall calibration calculation.
 */
enum class OverallCaliGencErr
{
    NOT_ERROR             = 0,   ///< No error flag activated.
    EMPTY_CAL_RECORDS     = 1,   ///< CRD Full Rate data is empty.
    MISSING_PREPOST       = 2,   ///< If shift option is PRE_POST but we dont have it.
    SHIFT_NOT_IMPLEMENTED = 3    ///< Shift option is not implemented.
};

/**
 * @enum StatsGenErr
 * @brief This enum represents the errors that could happen at overall calibration calculation.
 */
enum class StatsGenErr
{
    NOT_ERROR             = 0,   ///< No error flag activated.
    CPF_DATA_EMPTY        = 1,   ///< CPF is empty or is not valid.
    CRD_CFG_NOT_VALID     = 2,   ///< CRD has no System Configuration record.
    CRD_DATA_EMPTY        = 3,   ///< CRD Full Rate data is empty.
    RESIDS_CALC_FAILED    = 4,   ///< The residuals calculation failed.
    SOME_BINS_CALC_FAILED = 5,   ///< The calculations failed for some bins
    STATS_CALC_FAILED     = 6    ///< The statistics calculation failed
};
// =====================================================================================================================


// ========== FUNCTIONS ================================================================================================
/**
 * @brief Generate residuals from full rate data and calculate distribution statistics for those residuals
 *        using process described by A.T. Sinclair. Then generate a Statistics Record into the CRD.
 * @param[in] bs, the bin size used to divide full rate data in bins in seconds.
 * @param[in]  stat_geodetic, the geodetic position of the station.
 * @param[in]  stat_geocentric, the geocentric position of the station.
 * @param[in] cpf, the CPF used to generate residuals.
 * @param[out] crd, the CRD which contains the full rate data and where the generated record is stored.
 * @param[in] rf, the rejection factor around RMS. It should be 2.5 for single-photon detector and 3 for multiple-photon.
 * @param[in] tlrnc, tolerance factor for the convergence algorithm. Usually 0.1 for all systems.
 * @return The error code associated with the generation process. See ::StatsGenErr for more details.
 */
LIBDPSLR_EXPORT
StatsGenErr generateStatsRecord(std::size_t bs, const geo::frames::GeodeticPoint<long double> &stat_geodetic,
                                const geo::frames::GeocentricPoint<long double> &stat_geocentric,
                                const CPF &cpf, CRD& crd, double rf = 2.5, double tlrnc = 0.1);


/**
 * @brief Generates overall calibration record in a CRD
 * @param[in] shift_option, the delay shift applied
 * @param[out] crd, where the overall calibration record is generated. The old one, if any, will be replaced.
 * @return Overall calibration generation error code.
 * @pre Must have pre and post, and the time of the middle of the pass. The session header data
 *      must be set to obtain this time.
 * @post The old overall record is deleted. The overall calibration data is calculated and stored.
 *       If calculation fails, the struct will be empty.
 */
LIBDPSLR_EXPORT
OverallCaliGencErr generateOverallCalibration(CRDData::ShiftTypeEnum shift_option, CRD &crd);
// =====================================================================================================================

}} // END NAMESPACES
// =====================================================================================================================
