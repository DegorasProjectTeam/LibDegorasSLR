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
 * @file statistics.h
 * @author Degoras Project Team.
 * @brief This file contains the functions related to ILRS statistics algorithms.
 * @copyright EUPL License
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
// =====================================================================================================================
#include <cmath>
#include <vector>
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/UtilitiesSLR/predictors/predictor_slr_cpf.h"
#include "LibDegorasSLR/libdegorasslr_global.h"
#include "LibDegorasSLR/ILRS/algorithms/data/statistics_data.h"
#include "LibDegorasSLR/ILRS/formats/cpf/cpf.h"
#include "LibDegorasSLR/ILRS/formats/crd/crd.h"
#include "LibDegorasSLR/Geophysics/types/geocentric_point.h"
#include "LibDegorasSLR/Geophysics/types/geodetic_point.h"
#include "LibDegorasSLR/Geophysics/types/meteo_data.h"
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace ilrs{
namespace algorithms{
// =====================================================================================================================

// ========== ENUMS ====================================================================================================
/**
 * @brief Extracts bins present in the given time values.
 * @param times The list of time values in seconds of day.
 * @param bs The bin size in seconds.
 * @param div_opt The bin division to apply at bin extracting
 * @return The indexes included in each bin.
 */
std::vector<std::vector<std::size_t>> extractBins(const std::vector<long double> &times,
                                                  double bs, BinDivisionEnum div_opt);

/**
 * @brief Extracts bins present in the given time values.
 * @param data The list of ranges. The bins will be calculated using the timestamp of every range.
 * @param bs The bin size in seconds.
 * @param div_opt The bin division to apply at bin extracting
 * @return The indexes included in each bin.
 */
std::vector<std::vector<std::size_t>> extractBins(const RangeDataV& data,
                                                  double bs, BinDivisionEnum div_opt);


/**
 * @brief Generate residuals from full rate data. Also applies the Marini and Murray delay refraction correction.
 * @param[in]  cpf, the filepath of the CPF used to generate residuals.
 * @param[in]  mjd, the modified julian day when full rate starts
 * @param[in]  ftdata, the input flight time data. See ::FlightTimeData for more information.
 * @param[in]  meteo_records, the input meteo records used for calculating refraction correction.
 * @param[in]  stat_geodetic, the geodetic position of the station.
 * @param[in]  stat_geocentric, the geocentric position of the station.
 * @param[in]  wl, the wavelength of the laser used in micrometres.
 * @param[in]  bs, the bin size in seconds used for detrending the residuals.
 * @param[out] rdata, the calculated residuals data. See ::ResidualsData for more information.
 * @param[out] pred_dist, the calculated predicted distance used to calculate each residual
 * @param[out] trop_corr, the troposheric correction used to calculate each residual
 * @return The error code associated with the calculation process. See ::FullRateResCalcError for more information.
 */
LIBDPSLR_EXPORT
FullRateResCalcErr calculateFullRateResiduals(const slr::predictors::PredictorSlrCPF &predictor,
                                              const FullRateData &fr_data,
                                              double wl, std::size_t bs, RangeDataV& ranges);


/**
 * @brief Generate residuals from full rate data. Also applies the Marini and Murray delay refraction correction.
 * @param[in]  cpf, the CPF used to generate residuals.
 * @param[in]  crd, the CRD which contains the full rate data.
 * @param[in]  stat_geodetic, the geodetic position of the station.
 * @param[in]  stat_geocentric, the geocentric position of the station.
 * @param[in]  bs, the bin size in seconds used for detrending the residuals.
 * @param[out] rdata, the calculated residuals data. See ::ResidualsData for more information.
 * @return The error code associated with the calculation process. See ::FullRateResCalcError for more information.
 */
LIBDPSLR_EXPORT
FullRateResCalcErr calculateFullRateResiduals(const std::string &cpf_path, const crd::CRD& crd,
                                              const geo::types::GeodeticPointDeg& stat_geodetic,
                                              const geo::types::GeocentricPoint& stat_geocentric,
                                              std::size_t bs, RangeDataV &ranges);

/**
 * @brief Calculate distribution statistics for residuals using process described by A.T. Sinclair.
 * @param[in]  bs, the bin size used to divide full rate data in bins in seconds.
 * @param[in]  rdata, the input residuals data. See ::ResidualsData for more information.
 * @param[out] stats, the calculated distribution statistics. See ::ResidualsStats for more information.
 * @param[in]  rf, rejection factor around RMS. It should be 2.5 for single-photon detector and 3 for multiple-photon.
 * @param[in]  tlrnc, tolerance factor for the convergence algorithm. Usually 0.1 for all systems.
 * @return The error code associated with the calculation process. See ::FullRateStatsCalcError for more information.
 */
LIBDPSLR_EXPORT
ResiStatsCalcErr calculateResidualsStats(std::size_t bs, const RangeDataV &rdata,
                                         ResidualsStats& stats, double rf = 2.5, double tlrnc = 0.1);


/**
 * @brief Calculate the distribution statistics for a bin using the process described by A.T. Sinclair.
 * @param[in]  data, the bin data whose distribution statistics are calculated.
 * @param[out] stats, the calculated distribution statistics. See ::BinStats for more information.
 * @param[in]  rf, rejection factor arounds RMS. It should be 2.5 for single-photon detector and 3 for multiple-photon.
 * @param[in]  tlrnc, tolerance factor for the convergence algorithm. Usually 0.1 for all systems.
 * @return The error code associated with the calculation process. See ::BinStatsCalcError for more information.
 */
LIBDPSLR_EXPORT
BinStatsCalcErr calcBinStats(const std::vector<long double>& data, BinStats &stats,
                             double rf = 2.5, double tlrnc = 0.1);



/**
 * @brief Calculate peak using gaussian smoothing.
 * @param data, the residuals whose peak is calculated.
 * @param p0, the initial point to look for the peak. It should be the mean around rf*RMS.
 * @param peak, the returned peak of the distribution.
 * @param sigma, the sigma of smoothing function.
 * @param wide, the wide of each smoothing window.
 * @param step, the number of steps of smoothing function
 * @return true if calculation was successful, false otherwise
 */
LIBDPSLR_EXPORT
bool calcGaussianPeak(const std::vector<long double>& data, long double p0, long double& peak,
                      double sigma = 60.0, double wide = 2000.0, double step = 10.0);

/**
 * @brief Detrend residuals by calculating polynomial fit bin by bin.
 * @param bs, the bin size in seconds.
 * @param times, the timestamp of each residual.
 * @param resids, the residual value.
 * @param degree, the degree of the polynomial fit used.
 * @return Residuals with polynomial trend substracted
 */
LIBDPSLR_EXPORT
RangeDataV binPolynomialDetrend(const RangeDataV &ranges, unsigned int bs, unsigned int degree = 15);


}}} // END NAMESPACES
// =====================================================================================================================
