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
 * @file statistics.cpp
 * @author Degoras Project Team.
 * @brief This file contains the functions related to ILRS statistics algorithms.
 * @copyright EUPL License
***********************************************************************************************************************/

// C++ INCLUDES
// =====================================================================================================================
#include <numeric>
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/ILRS/algorithms/statistics.h"
#include "LibDegorasSLR/Geophysics/utils/tropo_utils.h"
// =====================================================================================================================

// LIBDPBASE INCLUDES
// =====================================================================================================================
#include "LibDegorasBase/Timing/utils/time_utils.h"
#include "LibDegorasBase/Helpers/container_helpers.h"
#include "LibDegorasBase/Statistics/fitting.h"
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace ilrs{
namespace algorithms{
// =====================================================================================================================

FullRateResCalcErr calculateFullRateResiduals(const slr::predictors::PredictorSlrCPF &predictor,
                                              const FullRateData &fr_data,
                                              double wl, std::size_t bs, RangeDataV& ranges)
{
    using slr::predictors::PredictorSlrCPF;

    // Clear the output container.
    ranges.clear();

    // Check if predictor can be used.
    if (!predictor.isReady())
        return FullRateResCalcErr::CPF_DATA_EMPTY;

    // Variables and containers.
    slr::predictors::PredictionSLR pred_result;
    std::size_t meteo_idx = 0;
    dpbase::timing::dates::MJDate mjd = fr_data.mjd;
    geo::types::GeodeticPointRad geod = predictor.getGeodeticLocation<dpbase::math::units::Radians>();
    RangeData range;

    // Vapor water pressure model.
    geo::meteo::WtrVapPressModel vwpm = geo::meteo::WtrVapPressModel::GIACOMO_DAVIS;

    // Calculate the residuals for each record.
    for (std::size_t i = 0; i < fr_data.ft_data.size(); i++)
    {
        dpbase::timing::dates::MJDateTime mjdt(mjd, dpbase::timing::types::SoD(fr_data.ft_data[i].ts));
        // Get the meteo index whose meteo time tag is next to the current time tag.
        while(meteo_idx < fr_data.meteo_data.size() && mjdt > fr_data.meteo_data[meteo_idx].mjdt)
            meteo_idx++;

        // Interpolate the CPF data to get the position for the current time tag.
        // For this algorithm is better to use the Instant Vector mode, as in NP calculation algorithm.
        auto error = predictor.predict(mjdt, pred_result);

        // Check the interpolation error.
        if (static_cast<decltype(error)>(PredictorSlrCPF::PredictionError::NOT_ERROR) == error ||
            static_cast<decltype(error)>(PredictorSlrCPF::PredictionError::INTERPOLATION_NOT_IN_THE_MIDDLE) == error)
        {
            long double corr_2w = 0.L;
            if (!fr_data.meteo_data.empty())
            {
                // Get the necessary meteo data for calculating Marini and Murray delay refraction correction.
                std::size_t selected = meteo_idx > 0 ? meteo_idx - 1 : 0;
                double press = fr_data.meteo_data[selected].pressure;
                double temp = fr_data.meteo_data[selected].temperature;
                double humid = fr_data.meteo_data[selected].rel_humidity;
                dpbase::math::units::Radians el_rad = dpbase::math::units::degToRad(
                    pred_result.instant_data->altaz_coord.el);



                // Calculate the 2-way refraction correction using Marini and Murray in seconds.
                auto corr = geo::tropo::pathDelayMariniMurray(press, temp, humid, el_rad, wl, geod.lat, geod.alt, vwpm);
                dpbase::math::units::Distance<long double> corr_2w_unit(corr * 2.L);
                corr_2w_unit.convert(decltype(corr_2w_unit)::Unit::LIGHT_PS);
                corr_2w = corr_2w_unit;
            }

            long double pred_2w_ps = pred_result.instant_data->tof_2w * dpbase::math::units::kSecToPs;

            // Store the range data.
            range.ts = fr_data.ft_data[i].ts;
            range.tof = fr_data.ft_data[i].tof * dpbase::math::units::kSecToPs;
            range.pred_dist = pred_2w_ps;
            range.trop_corr = corr_2w;
            range.resid = range.tof - range.pred_dist - range.trop_corr;
            ranges.push_back(std::move(range));
            range = {};
        }
        else
            return FullRateResCalcErr::RESIDS_CALC_FAILED;

        // Control day change
        if (i < fr_data.ft_data.size() - 1 && fr_data.ft_data[i + 1].ts < fr_data.ft_data[i].ts)
            mjd++;
    }

    ranges = binPolynomialDetrend(ranges, bs);

    // Return no error.
    return FullRateResCalcErr::NOT_ERROR;
}


FullRateResCalcErr calculateFullRateResiduals(const std::string &cpf_path, const crd::CRD &crd,
                                              const geo::types::GeodeticPointDeg &stat_geodetic,
                                              const geo::types::GeocentricPoint &stat_geocentric,
                                              std::size_t bs, RangeDataV &ranges)
{

    // Check the CRD data.
    if (crd.empty() || crd.getData().fullRateRecords().empty())
        return FullRateResCalcErr::CRD_DATA_EMPTY;

    // Check the CRD configuration.
    if (!crd.getConfiguration().systemConfiguration())
        return FullRateResCalcErr::CRD_CFG_NOT_VALID;

    slr::predictors::PredictorSlrCPF predictor(cpf_path, stat_geodetic, stat_geocentric);
    FullRateData fr_data;

    // Get the start time, meteo data, flight time data and wavelength in micrometres.
    fr_data.mjd = dpbase::timing::timePointToModifiedJulianDateTime(crd.getHeader().sessionHeader()->start_time).date();
    fr_data.ft_data = crd.getData().fullRateFlightTimeData();
    auto crd_meteo_records = crd.getData().meteorologicalRecords();
    geo::types::MeteoRecordV meteo_records;
    dpbase::timing::dates::MJDate current_mjd = fr_data.mjd;
    dpbase::timing::types::SoD last_ts(-1);
    for (const auto& rec : crd_meteo_records)
    {
        // Control day change for meteorological records
        if (rec.time_tag < last_ts)
        {
            current_mjd++;
        }
        last_ts = rec.time_tag;

        geo::types::MeteoRecord meteo_record;
        meteo_record.pressure = rec.surface_pressure;
        meteo_record.rel_humidity = rec.surface_relative_humidity;
        meteo_record.temperature = rec.surface_temperature;
        meteo_record.mjdt = dpbase::timing::dates::MJDateTime(current_mjd, dpbase::timing::types::SoD(rec.time_tag));
    }
    fr_data.meteo_data = std::move(meteo_records);
    double wl = crd.getConfiguration().systemConfiguration()->transmit_wavelength * 1e-3;

    return calculateFullRateResiduals(predictor, fr_data, wl, bs, ranges);
}


ResiStatsCalcErr calculateResidualsStats(std::size_t bs, const RangeDataV& rdata,
                                         ResidualsStats &rstats, double rf, double tlrnc)
{
    // Error variable.
    ResiStatsCalcErr error = ResiStatsCalcErr::NOT_ERROR;

    // Variables and containers.
    BinStats bin_stats;
    std::vector<long double> bin;
    std::size_t t0 = 0;
    std::size_t bins_ok = 0;

    // Clear the output container.
    rstats.total_bin_stats = {};
    rstats.bins.clear();
    rstats.total_bin_stats.stats_01rms = {0,0,0,0.L,0.L,0.L,0.L,0.L,0.};
    rstats.total_bin_stats.stats_rfrms = {0,0,0,0.L,0.L,0.L,0.L,0.L,0.};

    // Store data.
    rstats.rf = rf;
    rstats.bs = bs;
    rstats.total_bin_stats.rf = rf;
    rstats.total_bin_stats.ptn = rdata.size();
    rstats.total_bin_stats.error = BinStatsCalcErr::NOT_ERROR;
    rstats.error = error;

    // Calculate all the bin stats.
    for (std::size_t i = 0; i < rdata.size(); i++)
    {
        // If bin is finished, calculate statistics for it
        if (rdata[i].ts > rdata[t0].ts + bs)
        {
            // Calculate the statistics.
            if (BinStatsCalcErr::NOT_ERROR == calcBinStats(bin, bin_stats, rf, tlrnc))
                bins_ok++;

            // Stores the bin, number of points and the bin mask vectors.
            rstats.bins.push_back(bin_stats);
            dpbase::helpers::containers::insert(rstats.total_bin_stats.amask_rfrms, bin_stats.amask_rfrms);
            dpbase::helpers::containers::insert(rstats.total_bin_stats.amask_01rms, bin_stats.amask_01rms);

            // Update counters and clear the bin vector.
            t0 = i;
            bin.clear();
        }

        // Stores the residual in the bin.
        bin.push_back(rdata[i].resid);
    }

    // Calculate statistics for last bin
    if (BinStatsCalcErr::NOT_ERROR == calcBinStats(bin, bin_stats, rf, tlrnc))
        bins_ok++;

    rstats.bins.push_back(bin_stats);
    dpbase::helpers::containers::insert(rstats.total_bin_stats.amask_rfrms, bin_stats.amask_rfrms);
    dpbase::helpers::containers::insert(rstats.total_bin_stats.amask_01rms, bin_stats.amask_01rms);

    // Statistics calculation failed in every bin.
    if (0 == bins_ok)
    {
        error = ResiStatsCalcErr::STATS_CALC_FAILED;
        rstats.error = error;
        return error;
    }

    // Check if some bins calculations have failed.
    if (rstats.bins.size() != bins_ok)
    {
        error = ResiStatsCalcErr::SOME_BINS_CALC_FAILED;
        rstats.error = error;
    }

    // Get mean for necessary statistics.
    for (const auto& bin_stat : rstats.bins)
    {
        // If bin calculation is not succesful do not include its results in total, only rejected points
        if (BinStatsCalcErr::NOT_ERROR != bin_stat.error)
        {
            rstats.total_bin_stats.stats_rfrms.rptn += bin_stat.stats_rfrms.rptn;
            rstats.total_bin_stats.stats_01rms.rptn += bin_stat.stats_01rms.rptn;
        }
        else
        {

            // Sum RF*RMS data from all bins.
            rstats.total_bin_stats.stats_rfrms.iter += bin_stat.stats_rfrms.iter;
            rstats.total_bin_stats.stats_rfrms.aptn += bin_stat.stats_rfrms.aptn;
            rstats.total_bin_stats.stats_rfrms.rptn += bin_stat.stats_rfrms.rptn;
            rstats.total_bin_stats.stats_rfrms.mean += bin_stat.stats_rfrms.mean;
            rstats.total_bin_stats.stats_rfrms.rms += bin_stat.stats_rfrms.rms;
            rstats.total_bin_stats.stats_rfrms.skew += bin_stat.stats_rfrms.skew;
            rstats.total_bin_stats.stats_rfrms.kurt += bin_stat.stats_rfrms.kurt;
            rstats.total_bin_stats.stats_rfrms.peak += bin_stat.stats_rfrms.peak;

            // Sum 1*RMS data from all bins.
            rstats.total_bin_stats.stats_01rms.iter += bin_stat.stats_01rms.iter;
            rstats.total_bin_stats.stats_01rms.aptn += bin_stat.stats_01rms.aptn;
            rstats.total_bin_stats.stats_01rms.rptn += bin_stat.stats_01rms.rptn;
            rstats.total_bin_stats.stats_01rms.mean += bin_stat.stats_01rms.mean;
            rstats.total_bin_stats.stats_01rms.rms += bin_stat.stats_01rms.rms;
            rstats.total_bin_stats.stats_01rms.skew += bin_stat.stats_01rms.skew;
            rstats.total_bin_stats.stats_01rms.kurt += bin_stat.stats_01rms.kurt;
            rstats.total_bin_stats.stats_01rms.peak += bin_stat.stats_01rms.peak;
        }
    }

    // Calculate total accepted rate
    rstats.total_bin_stats.stats_rfrms.arate =
        (rstats.total_bin_stats.stats_rfrms.aptn/static_cast<double>(rstats.total_bin_stats.ptn))*100.;
    rstats.total_bin_stats.stats_01rms.arate =
        (rstats.total_bin_stats.stats_01rms.aptn/static_cast<double>(rstats.total_bin_stats.ptn))*100.;

    // RF*RMS stats calculated as mean of all bins. Accepted, rejected and accepted rate are absolute for all bins.
    rstats.total_bin_stats.stats_rfrms.iter /= bins_ok;
    rstats.total_bin_stats.stats_rfrms.mean /= bins_ok;
    rstats.total_bin_stats.stats_rfrms.rms /= bins_ok;
    rstats.total_bin_stats.stats_rfrms.skew /= bins_ok;
    rstats.total_bin_stats.stats_rfrms.kurt /= bins_ok;
    rstats.total_bin_stats.stats_rfrms.peak /= bins_ok;

    // 1*RMS stats calculated as mean of all bins. Accepted, rejected and accepted rate are absolute for all bins.
    rstats.total_bin_stats.stats_01rms.iter /= bins_ok;
    rstats.total_bin_stats.stats_01rms.mean /= bins_ok;
    rstats.total_bin_stats.stats_01rms.rms /= bins_ok;
    rstats.total_bin_stats.stats_01rms.skew /= bins_ok;
    rstats.total_bin_stats.stats_01rms.kurt /= bins_ok;
    rstats.total_bin_stats.stats_01rms.peak /= bins_ok;

    // Return the error.
    return error;
}


BinStatsCalcErr calcBinStats(const std::vector<long double> &data, BinStats &stats, double rf, double tlrnc)
{
    // Hardcoded configuration values.
    constexpr std::size_t maxiter = 20;

    // Error variable.
    BinStatsCalcErr error = BinStatsCalcErr::NOT_ERROR;

    // Variables.
    std::size_t i = 0;
    std::size_t npt_rfrms = 0, npt_1rms = 0;
    long double last_mean = std::numeric_limits<long double>::max();
    long double rej = std::numeric_limits<long double>::max();
    long double delta = 0.0L;
    long double rms_rfrms = 0.0L, rms_1rms = 0.0L;
    long double mean_rfrms = 0.0L;
    long double mean_1rms = 0.0L;
    long double c2 = 0.L, c3 = 0.L, c4 = 0.L;
    long double skew_rfrms, kurt_rfrms, peak, skew_1rms, kurt_1rms, peak_1rms;
    bool converged = false;
    bool peak_ok;
    std::vector<bool> msk_1rms;
    std::vector<bool> msk_rfrms;

    // Store the common statistic data.
    stats.ptn = data.size();
    stats.rf = rf;
    stats.error = error;
    stats.stats_01rms.rptn = data.size();
    stats.stats_01rms.aptn = 0;
    stats.stats_rfrms.rptn = data.size();
    stats.stats_rfrms.aptn = 0;
    stats.stats_rfrms.arate = 0;

    // Do the convergence process to form the mean around RF*RMS.
    // Remember that RF is the rejection factor (rej_fact variable).
    while(i < maxiter && !converged)
    {
        // Aux variables.
        long double sumres = 0.0L;
        long double sumressq = 0.0L;
        unsigned int npt = 0;

        // Iterate the data comparing the data with the RF.
        for (const auto& elem : data)
        {
            long double res = elem - mean_rfrms;
            if (std::abs(res) <= rej)
            {
                sumres += res;
                sumressq += res*res;
                npt++;
            }
        }

        // If there are no points, then they have been rejected.
        if (npt == 0)
        {
            // Stores the error, put the mask all to false, and return the error.
            stats.amask_rfrms = std::vector<bool>(data.size(), false);
            stats.amask_01rms = std::vector<bool>(data.size(), false);
            error = BinStatsCalcErr::REJECTED_RFRMS;
            stats.error = error;
            return error;
        }
        else
        {
            // If there are points, update the mean, rms and reject values.
            delta = sumres / npt;
            mean_rfrms += delta;
            rms_rfrms = std::sqrt(sumressq / npt - delta * delta);
            rej = rf * rms_rfrms;
            // Check if the algorithm has converged.
            if (std::abs(mean_rfrms - last_mean) < tlrnc)
                converged = true;
            // Update the last mean for the next iteration and increment the iteration number.
            last_mean = mean_rfrms;
            i++;
        }
    } // End the convergence process for RF*RMS.

    // If RF*RMS calculation fails, stop the calculation.
    if (!converged)
    {
        // Stores the error, put the mask all to false, and return the error.
        stats.amask_rfrms = std::vector<bool>(data.size(), false);
        stats.amask_01rms = std::vector<bool>(data.size(), false);
        error = BinStatsCalcErr::NOT_CONVERGED_RFRMS;
        stats.error = error;
        return error;
    }

    // Store the iterations until convergence for RF*RMS.
    stats.stats_rfrms.iter = i;

    // Get kurtosis and skew around RF*RMS.
    for (const auto& elem : data)
    {
        long double res = elem - mean_rfrms;
        if (std::abs(res) <= rej)
        {
            long double res_mult = res * res;
            c2 += res_mult;
            c3 += (res_mult *= res);
            c4 += (res_mult * res);
            npt_rfrms++;
            msk_rfrms.push_back(true);
        }
        else
            msk_rfrms.push_back(false);
    }

    // Calculates the skew and kurtosis.
    c2 /= npt_rfrms;
    c3 /= npt_rfrms;
    c4 /= npt_rfrms;
    skew_rfrms = c3 / std::pow(c2, 1.5L);
    kurt_rfrms = c4 / (c2 * c2);

    // Gaussian peak calculation for RF*RMS.
    peak_ok = calcGaussianPeak(data, mean_rfrms, peak);

    // Check the peak calculation and store it.
    if (!peak_ok)
    {
        // Stores the error, put the mask all to false, and return the error.
        stats.amask_rfrms = std::vector<bool>(data.size(), false);
        stats.amask_01rms = std::vector<bool>(data.size(), false);
        error = BinStatsCalcErr::PEAK_CALC_FAILED_RFRMS;
        stats.error = error;
        return error;
    }
    else
        stats.stats_rfrms.peak = peak;

    // Form mean around 1 * RMS
    last_mean = std::numeric_limits<long double>::max();
    mean_1rms = mean_rfrms;
    rej = rms_rfrms;
    converged = false;
    i = 0;

    // Do the convergence process to form the mean around 1*RMS.
    // Remember that RF is the rejection factor (rej_fact variable).
    while(i < maxiter && !converged)
    {
        // Aux variables.
        long double sumres = 0.0L;
        long double sumressq = 0.0L;
        npt_1rms = 0;

        // Iterate the data comparing the data with the RF.
        for (const auto& elem : data)
        {
            long double res = elem - mean_1rms;
            if (std::abs(res) <= rej)
            {
                sumres += res;
                sumressq += res*res;
                npt_1rms++;

            }
        }

        // If there are no points, then they have been rejected.
        if (npt_1rms == 0)
        {
            // Stores the error, put the mask all to false, and return the error.
            stats.amask_rfrms = std::vector<bool>(data.size(), false);
            stats.amask_01rms = std::vector<bool>(data.size(), false);
            error = BinStatsCalcErr::REJECTED_1RMS;
            stats.error = error;
            return error;
        }
        else
        {
            // If there are points, update the mean and rms values.
            delta = sumres / npt_1rms;
            mean_1rms += delta;
            rms_1rms = std::sqrt(sumressq / npt_1rms - delta * delta);
            // Check if the algorithm has converged.
            if (std::abs(mean_1rms - last_mean) < tlrnc)
                converged = true;
            // Update the last mean for the next iteration and increment the iteration number.
            last_mean = mean_1rms;
            i++;
        }
    }

    // Check the 1*RMS convergence.
    if (!converged)
    {
        // Stores the error, put the mask all to false, and return the error.
        stats.amask_rfrms = std::vector<bool>(data.size(), false);
        stats.amask_01rms = std::vector<bool>(data.size(), false);
        error = BinStatsCalcErr::NOT_CONVERGED_1RMS;
        stats.error = error;
        return error;
    }
    else
    {
        // Reset varaibles.
        c2 = 0.L;
        c3 = 0.L;
        c4 = 0.L;

        // Store the iterations until convergence for RF*RMS.
        stats.stats_01rms.iter = i;

        // Get kurtosis and skew around 1*RMS.
        for (const auto& elem : data)
        {
            long double res = elem - mean_1rms;
            if (std::abs(res) <= rej)
            {
                long double res_mult = res * res;
                c2 += res_mult;
                c3 += (res_mult *= res);
                c4 += (res_mult * res);
                msk_1rms.push_back(true);
            }
            else
                msk_1rms.push_back(false);
        }

        // Calculates the skew and kurtoisis.
        c2 /= npt_1rms;
        c3 /= npt_1rms;
        c4 /= npt_1rms;
        skew_1rms = c3 / std::pow(c2, 1.5L);
        kurt_1rms = c4 / (c2 * c2);

        // Gaussian peak calculation for 1*RMS.
        peak_ok = calcGaussianPeak(data, mean_1rms, peak_1rms);

        // Check the peak calculation.
        if (!peak_ok)
        {
            // Stores the error, put the mask all to false, and return the error.
            stats.amask_rfrms = std::vector<bool>(data.size(), false);
            stats.amask_01rms = std::vector<bool>(data.size(), false);
            error = BinStatsCalcErr::PEAK_CALC_FAILED_1RMS;
            stats.error = error;
            return error;
        }
        else
            stats.stats_01rms.peak = peak_1rms;

        // Store the statistic data for 1*RMS.
        // Number of iterations is already stored.
        // Peak is already stored.
        stats.stats_01rms.kurt = kurt_1rms - 3.L; // ILRS convention.
        stats.stats_01rms.mean = mean_1rms;
        stats.stats_01rms.aptn = npt_1rms;
        stats.stats_01rms.rptn = data.size() - npt_1rms;
        stats.stats_01rms.rms = rms_1rms;
        stats.stats_01rms.skew = skew_1rms;
        stats.stats_01rms.arate = npt_1rms * 100 / static_cast<double>(data.size());
        stats.amask_01rms = msk_1rms;
    }

    // Store the statistic data for RF*RMS.
    // Number of iterations is already stored.
    // Peak is already stored.
    stats.stats_rfrms.kurt = kurt_rfrms - 3.L; // ILRS convention.
    stats.stats_rfrms.mean = mean_rfrms;
    stats.stats_rfrms.aptn = npt_rfrms;
    stats.stats_rfrms.rptn = data.size() - npt_rfrms;
    stats.stats_rfrms.rms = rms_rfrms;
    stats.stats_rfrms.skew = skew_rfrms;
    stats.stats_rfrms.arate = npt_rfrms * 100 / static_cast<double>(data.size());
    stats.amask_rfrms = msk_rfrms;
    stats.error = error;

    // Return the error value.
    return error;
}


bool calcGaussianPeak(const std::vector<long double> &data, long double p0, long double &peak,
                      double sigma, double wide, double step)
{
    if (wide < 0. || step < 0.)
        return false;

    if (data.empty())
        return false;

    // Variable and constants initialization
    long double fine_step = step / 10.L;
    const std::size_t npoints = static_cast<std::size_t>(std::round(wide/step)) + 1;
    const std::size_t centre = (npoints - 1) / 2;
    long double x0 = p0 - (centre + 1) * step;
    peak = 0.0L;


    std::vector<long double> x(npoints);
    std::vector<long double> y(npoints);
    std::vector<long double> xfit;
    std::vector<long double> yfit;

    std::generate(x.begin(), x.end(), [x = x0, step] () mutable {return x += step;});

    auto gaussian_pdf = [](auto x, auto mean, auto var)
    {
        return std::exp(-std::pow((x - mean)/var, 2.0L) / 2.0L);
    };

    // Get coarse location of peak by evaluating gaussian distribution functions at steps
    // around p0 using each residual in data as mean.
    for (unsigned int i = 0; i < npoints; i++)
    {
        y[i] = std::accumulate(data.begin(), data.end(), 0.0L,
                               [gaussian_pdf, x = x[i], sigma](long double accum, long double n)
                               {
                                   return accum + gaussian_pdf(x, n, sigma);
                               });
    }

    auto itmax = std::max_element(y.begin(), y.end());
    std::size_t index_max = static_cast<std::size_t>(std::distance(y.begin(), itmax));

    long double xfine = x[index_max > 0 ? index_max - 1 : index_max];
    long double xmax_fine = 0.0L;
    long double ymax_fine = 0.0L;

    // Now get fine location of peak. Look around the maximum of the coarse peak.
    for (unsigned int i = 0; i < 19; i++)
    {
        xfine += fine_step;

        long double sum = std::accumulate(data.begin(), data.end(), 0.0L,
            [gaussian_pdf, x = xfine, sigma](long double accum, long double n)
            {
                return accum + gaussian_pdf(x, n, sigma);
            });

        if (sum > ymax_fine)
        {
            xmax_fine = xfine;
            ymax_fine = sum;
        }
    }

    // Replace coarse peak with fine peak
    x[index_max] = xmax_fine;
    y[index_max] = ymax_fine;
    peak = xmax_fine;

    // Normalise distribution to maximum value 100
    long double factor = 100.L/ymax_fine;

    std::transform(y.begin(), y.end(), y.begin(), [factor](const auto& elem){return factor * elem;});

    // Find peaks within FWHM
    std::size_t npeaks = 1;
    int slope_sign = 1;

    // Find peaks to the left of max.
    auto i = index_max;
    while (i > 0 && y[i] >= 50.0L)
    {
        i--;

        if (y[i] > y[i + 1])
        {
            if (slope_sign == 1)
                npeaks++;

            slope_sign = -1;
        }
        else
            slope_sign = 1;

    }

    // Find peaks to the right of max
    slope_sign = -1;
    i = index_max;
    while (i < y.size() - 1 && y[i] >= 50.0L)
    {
        i++;

        if (y[i] > y[i - 1])
        {
            if (slope_sign == -1)
                npeaks++;

            slope_sign = 1;
        }
        else
            slope_sign = -1;

    }

    // TODO: 0 peaks is a fail?
    // If there is no peak, return false, if there is only one peak, then return true.
    if (npeaks == 0)
        return false;
    else if (npeaks == 1)
        return true;

    // Otherwise, use polynomial fit to find the peak
    // Get points with y > 40 for polynomial fitting
    for (std::size_t i = 0; i < x.size(); i++)
    {
        if (y[i] >= 40.L)
        {
            xfit.push_back(x[i]);
            yfit.push_back(y[i]);
        }
    }

    // TODO: Set x axis to relative distance? - max / span (span = (last - first) /2)
    // TODO: Polynomial fit inviable? matrix singular?
    long double max_peak_fit = -std::numeric_limits<long double>::max();
    std::vector<long double> coefs = dpbase::stats::polynomialFit(xfit, yfit, 4);

    long double fit_step = (xfit.back() - xfit.front()) / (npoints - 1);
    long double xfit_step = xfit.front();
    while ( xfit_step <= xfit.back() )
    {
        auto res = dpbase::stats::applyPolynomial(coefs, xfit_step);
        if (res > max_peak_fit)
            max_peak_fit = res;

        xfit_step += fit_step;
    }

    long double xder = max_peak_fit - peak;

    // Use Newton-Raphson method to find zero of derivate (max)
    for (std::size_t i = 0; i < 4; i++)
    {
        long double x2 = xder * xder;
        long double x3 = x2 * xder;

        long double f = coefs[1] + 2.0L * coefs[2] * xder + 3.0L * coefs[3] * x2 + 4.0L * coefs[4] * x3;
        long double fd = 2.0L * coefs[2] + 6.0L * coefs[3] * xder + 12.0L * coefs[4] * x2;

        xder -= f/fd;
    }

    peak = xder;

    return true;
}


RangeDataV binPolynomialDetrend(const RangeDataV &ranges, unsigned int bs, unsigned int degree)
{
    if (ranges.empty())
        return ranges;

    RangeDataV result(ranges);
    auto result_it = result.begin();

    long double bin_start_ts = ranges.front().ts;
    std::vector<long double> xbin, ybin;

    for (const auto &range : ranges)
    {
        if (range.ts - bin_start_ts > bs)
        {
            auto coefs = dpbase::stats::polynomialFit(xbin, ybin, degree);
            for (auto it_x = xbin.begin(), it_y = ybin.begin(); it_x != xbin.end() && it_y != ybin.end(); ++it_x, ++it_y)
            {
                result_it->resid = result_it->resid - dpbase::stats::applyPolynomial(coefs, *it_x);
                result_it++;
            }
            xbin.clear();
            ybin.clear();
            bin_start_ts = range.ts;

        }

        xbin.push_back(range.ts);
        ybin.push_back(range.resid);
    }

    if (!xbin.empty() && !ybin.empty())
    {
        auto coefs = dpbase::stats::polynomialFit(xbin, ybin, 9);
        for (auto it_x = xbin.begin(), it_y = ybin.begin(); it_x != xbin.end() && it_y != ybin.end(); ++it_x, ++it_y)
        {
            result_it->resid = result_it->resid - dpbase::stats::applyPolynomial(coefs, *it_x);
            result_it++;
        }
    }

    return result;
}

std::vector<std::vector<std::size_t> > extractBins(const RangeDataV &data, double bs, BinDivisionEnum div_opt)
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
        int last_bin = static_cast<int>(std::floor(data[0].ts/bs) + 1);

        // Generate the bins.
        for (std::size_t i = 0; i < data.size(); i++)
        {
            // Get the current bin.
            int bin = static_cast<int>(std::floor(data[i].ts/bs) + 1);

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

std::vector<std::vector<std::size_t>> extractBins(const std::vector<long double> &times,
                                                   double bs, BinDivisionEnum div_opt)
{
    // Check the input data.
    if (times.empty() || bs <= 0)
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


}}} // END NAMESPACES
// =====================================================================================================================
