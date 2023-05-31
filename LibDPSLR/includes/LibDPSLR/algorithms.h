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
#include "geo.h"
#include "class_cpf.h"
#include "class_crd.h"
// =====================================================================================================================

// ========== DPSLR NAMESPACES =========================================================================================
namespace dpslr{
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

/// @struct DistStats
struct DistStats
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
struct BinStats
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
struct ResidualsStats
{
    double rf;                      ///< Reject factor (RF). Usually 3 when using multi-photon or 2.5 for single-photon.
    std::size_t bs;                 ///< Bin size in seconds.
    BinStats total_bin_stats;       ///< Final statistics calculated over every bin.
    std::vector<BinStats> bins;     ///< Vector with the statistics of each bin.
    ResiStatsCalcErr error;         ///< Stores the error. See ::ResiStatsCalcErr for more information.
};
// =====================================================================================================================


// ========== FUNCTIONS ================================================================================================

template<typename T, typename R>
std::vector<std::vector<std::size_t>> extractBins(const std::vector<T> &times, const std::vector<R> &resids,
                                                  double bs, BinDivisionEnum div_opt)
{
    // Check the input data.
    if (times.empty() || resids.empty() || times.size() != resids.size() || bs <= 0)
        return {};

    // Containers and auxiliar variables.
    std::vector<std::vector<std::size_t>> bins;
    std::vector<std::size_t> current_bin;

    // Day fixed option.
    if(div_opt == BinDivisionEnum::DAY_FIXED)
    {
        // Get the first bin.
        int last_bin = static_cast<int>(std::floor(times[0]/bs) + 1);

        // Generate the bins.
        for (std::size_t i = 0; i < times.size(); i++)
        {
            // Get the current bin.
            int bin = static_cast<int>(std::floor(times[i]/bs) + 1);

            // Check if the current bin has changed.
            if(last_bin != bin)
            {
                // New bin section.
                last_bin = bin;
                bins.push_back(std::move(current_bin));

                // Clear the bin container.
                current_bin = {};
            }

            // Stores the bin data.
            current_bin.push_back(i);
        }

        // Store the last bin.
        bins.push_back(std::move(current_bin));
    }

    // Return the bins.
    return bins;
}

template<typename T, typename R>
std::vector<std::vector<std::size_t>> extractBins(const common::ResidualsData<T,R>& data,
                                                  double bs, BinDivisionEnum div_opt)
{
    // Check the input data.
    if (data.empty() || bs <= 0)
        return {};

    // Containers and auxiliar variables.
    std::vector<std::vector<std::size_t>> bins;
    std::vector<std::size_t> current_bin;

    // Day fixed option.
    if(div_opt == BinDivisionEnum::DAY_FIXED)
    {
        // Get the first bin.
        int last_bin = static_cast<int>(std::floor(data[0].first/bs) + 1);

        // Generate the bins.
        for (std::size_t i = 0; i < data.size(); i++)
        {
            // Get the current bin.
            int bin = static_cast<int>(std::floor(data[i].first/bs) + 1);

            // Check if the current bin has changed.
            if(last_bin != bin)
            {
                // New bin section.
                last_bin = bin;
                bins.push_back(std::move(current_bin));

                // Clear the bin container.
                current_bin = {};
            }

            // Stores the bin data.
            current_bin.push_back(i);
        }

        // Store the last bin.
        bins.push_back(std::move(current_bin));
    }

    // Return the bins.
    return bins;
}


/**
 * @brief Generate residuals from full rate data. Also applies the Marini and Murray delay refraction correction.
 * @param[in]  cpf, the CPF used to generate residuals.
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
FullRateResCalcErr calculateFullRateResiduals(const CPF &cpf, long long mjd, const common::FlightTimeData& ftdata,
                                              const std::vector<CRDData::MeteorologicalRecord> &meteo_records,
                                              const geo::frames::GeodeticPoint<long double>& stat_geodetic,
                                              const geo::frames::GeocentricPoint<long double>& stat_geocentric,
                                              double wl, std::size_t bs, common::ResidualsData<>& rdata,
                                              std::vector<long double> &pred_dist, std::vector<long double> &trop_corr);

/**
 * @brief Generate residuals from full rate data. Also applies the Marini and Murray delay refraction correction.
 * @param[in]  ranges_data, the ranges data including time_tag, tof, pred_dist and trop_corr.
 * @param[in]  bs, the bin size in seconds used for detrending the residuals.
 * @param[out] rdata, the calculated residuals data. See ::ResidualsData for more information.
 * @return The error code associated with the calculation process. See ::FullRateResCalcError for more information.
 */
LIBDPSLR_EXPORT
FullRateResCalcErr calculateFullRateResiduals(const common::RangeData &ranges_data, std::size_t bs,
                                              common::ResidualsData<>& rdata);

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
FullRateResCalcErr calculateFullRateResiduals(const CPF& cpf, const CRD& crd,
                                              const geo::frames::GeodeticPoint<long double>& stat_geodetic,
                                              const geo::frames::GeocentricPoint<long double>& stat_geocentric,
                                              std::size_t bs, common::ResidualsData<> &rdata);

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
ResiStatsCalcErr calculateResidualsStats(std::size_t bs, const common::ResidualsData<> &rdata,
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
BinStatsCalcErr calcBinStats(const std::vector<long double>& data, BinStats &stats, double rf = 2.5, double tlrnc = 0.1);



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
common::ResidualsData<> binPolynomialDetrend(int bs, const std::vector<long double> &times,
                                               const std::vector<long double> &resids, unsigned int degree = 15);
/*
 * @brief Determine what residuals are within a given prefilter window.
 *
 * Determine what residuals are within a given prefilter window. The window is generated using the upper and lower
 * limits as input parameters. All the input data must be coherent. If the residuals are given as distance, the
 * limits should be given as distance, and if the residuals are given as time, the limits should be given as time.
 * The generated acceptance interval will be closed at both ends. The return is a vector with the indexes of the
 * accepted residuals and it will be empty if there is a problem with the inputs.
 *
 * @param[in] resids, vector with the residuals that will be analyzed.
 * @param[in]  upper, the upper limit of the prefilter window.
 * @param[in]  lower, the lower limit of the prefilter window.
 * @return A vector with the indexes of the accepted residuals. It will be empty if there is an error.
 */
LIBDPSLR_EXPORT
std::vector<std::size_t> windowPrefilter(const std::vector<long double> &resids, long double upper, long double lower);

LIBDPSLR_EXPORT
std::vector<std::size_t> windowPrefilter(const std::vector<double> &resids, double upper, double lower);

LIBDPSLR_EXPORT
std::vector<std::size_t> histPrefilterSLR(const std::vector<double> &times, const std::vector<double> &resids,
                                       double bs, double depth, unsigned min_ph, unsigned divisions);

LIBDPSLR_EXPORT
std::vector<std::size_t> histPrefilterBinSLR(const std::vector<double>& resids_bin, double depth, unsigned min_ph);

LIBDPSLR_EXPORT
std::vector<std::size_t> histPostfilterSLR(const std::vector<double> &times, const std::vector<double> &resids,
                                           double bs, double depth);

// =====================================================================================================================

}} // END NAMESPACES
// =====================================================================================================================
