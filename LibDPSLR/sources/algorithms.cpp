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

#include "LibDPSLR/algorithms.h"
#include "LibDPSLR/utils.h"
#include "LibDPSLR/dpslr_math.h"
#include "LibDPSLR/cpfutils.h"

#include <iostream>

namespace dpslr
{
namespace algorithms
{

FullRateResCalcErr calculateFullRateResiduals(const CPF &cpf, long long mjd, const common::FlightTimeData& ftdata,
                                              const std::vector<CRDData::MeteorologicalRecord> &meteo_records,
                                              const geo::frames::GeodeticPoint<long double>& stat_geodetic,
                                              const geo::frames::GeocentricPoint<long double>& stat_geocentric,
                                              double wl, std::size_t bs, common::ResidualsData<>& rdata,
                                              std::vector<long double> &pred_dist, std::vector<long double> &trop_corr)
{
    // Check the CPF data.
    if (cpf.empty() || cpf.getData().positionRecords().empty())
        return FullRateResCalcErr::CPF_DATA_EMPTY;

    // Clear the output container.
    rdata.clear();

    // Variables and containers.

    dpslr::cpfutils::CPFInterpolator::InterpolationResult interp_data;
    std::size_t meteo_idx = 0;

    // Vapor water pressure model.
    geo::meteo::WtrVapPressModel vwpm = geo::meteo::WtrVapPressModel::GIACOMO_DAVIS;

    // Convert station geodetic location to radians.
    auto interpolator = cpfutils::CPFInterpolator(cpf, stat_geodetic, stat_geocentric);
    geo::frames::GeodeticPoint<long double> stat_geodetic_rad = stat_geodetic;
    stat_geodetic_rad.convert(decltype(stat_geodetic_rad)::AngleType::Unit::RADIANS,
                              decltype(stat_geodetic_rad)::DistType::Unit::METRES);


    // Calculate the residuals for each record.
    for (std::size_t i = 0; i < ftdata.size(); i++)
    {
        // Get the meteo index whose meteo time tag is next to the current time tag.
        while(meteo_idx < meteo_records.size() && ftdata[i].first > meteo_records[meteo_idx].time_tag)
            meteo_idx++;

        // Control day change
        if (i > 0 && ftdata[i].first < ftdata[i - 1].first)
            mjd++;

        // Interpolate the CPF data to get the position for the current time tag.
        // For this algorithm is better to use the Instant Vector mode, as in NP calculation algorithm.
        auto interp_err = interpolator.interpolate(mjd, ftdata[i].first, interp_data,
                                                   cpfutils::CPFInterpolator::INSTANT_VECTOR);

        // Check the interpolation error.
        if (cpfutils::CPFInterpolator::NOT_ERROR == interp_err ||
                cpfutils::CPFInterpolator::INTERPOLATION_NOT_IN_THE_MIDDLE == interp_err)
        {
            long double corr_2w = 0.;
            if (!meteo_records.empty())
            {
                // Get the necessary meteo data for calculating Marini and Murray delay refraction correction.
                std::size_t selected = meteo_idx > 0 ? meteo_idx - 1 : 0;
                double press = meteo_records[selected].surface_pressure;
                double temp = meteo_records[selected].surface_temperature;
                double humid = meteo_records[selected].surface_relative_humidity;
                geo::meas::Angle<double> el(interp_data.elevation, geo::meas::Angle<double>::Unit::DEGREES);
                el.convert(geo::meas::Angle<double>::Unit::RADIANS);

                // Calculate the 2-way refraction correction using Marini and Murray in seconds.
                auto corr = geo::tropo::pathDelayMariniMurray(press, temp, humid, el, wl, stat_geodetic_rad.lat,
                                                              stat_geodetic_rad.alt, vwpm);
                geo::meas::Distance<long double> corr_2w_unit(corr * 2.);
                corr_2w_unit.convert(decltype(corr_2w_unit)::Unit::LIGHT_PS);
                corr_2w = corr_2w_unit;
            }
            
            long double pred_2w_ps = interp_data.tof_2w * math::kSecondToPicosecond;

            // Store the residual in picoseconds and the time tag in seconds.
            rdata.push_back({ftdata[i].first, ftdata[i].second * math::kSecondToPicosecond - pred_2w_ps - corr_2w});
            pred_dist.push_back(pred_2w_ps);
            trop_corr.push_back(corr_2w);
        }
        else
            return FullRateResCalcErr::RESIDS_CALC_FAILED;
    }

    std::vector<std::remove_reference_t<decltype(rdata)>::value_type::first_type> times;
    std::vector<std::remove_reference_t<decltype(rdata)>::value_type::second_type> resid;
    for (const auto& data : rdata)
    {
        times.push_back(data.first);
        resid.push_back(data.second);
    }

    rdata = binPolynomialDetrend(bs, times, resid);

    // Return no error.
    return FullRateResCalcErr::NOT_ERROR;
}


FullRateResCalcErr calculateFullRateResiduals(const CPF &cpf, const CRD &crd,
                                              const geo::frames::GeodeticPoint<long double> &stat_geodetic,
                                              const geo::frames::GeocentricPoint<long double> &stat_geocentric,
                                              std::size_t bs, common::ResidualsData<> &rdata)
{

    // Check the CRD data.
    if (crd.empty() || crd.getData().fullRateRecords().empty())
        return FullRateResCalcErr::CRD_DATA_EMPTY;

    // Check the CRD configuration.
    if (!crd.getConfiguration().systemConfiguration())
        return FullRateResCalcErr::CRD_CFG_NOT_VALID;

    unsigned sec;
    unsigned mjd;
    double sec_fract;

    // Get the start time, meteo data and wavelength in micrometres.
    utils::timePointToModifiedJulianDate(crd.getHeader().sessionHeader()->start_time, mjd, sec, sec_fract);
    auto meteo_records = crd.getData().meteorologicalRecords();
    double wl = crd.getConfiguration().systemConfiguration()->transmit_wavelength * math::kNanometerToMicrometer;

    // Get the full rate ToF data.
    common::FlightTimeData ftdata = crd.getData().fullRateFlightTimeData();
    // TODO: predicted distance and tropospheric correction are discarded. Return?
    std::vector<long double> dummy;

    return calculateFullRateResiduals(cpf, mjd, ftdata, meteo_records, stat_geodetic, stat_geocentric,
                                      wl, bs, rdata, dummy, dummy);
}


ResiStatsCalcErr calculateResidualsStats(std::size_t bs, const common::ResidualsData<>& rdata,
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
        if (rdata[i].first > rdata[t0].first + bs)
        {
            // Calculate the statistics.
            if (BinStatsCalcErr::NOT_ERROR == calcBinStats(bin, bin_stats, rf, tlrnc))
                bins_ok++;

            // Stores the bin, number of points and the bin mask vectors.
            rstats.bins.push_back(bin_stats);
            helpers::insert(rstats.total_bin_stats.amask_rfrms, bin_stats.amask_rfrms);
            helpers::insert(rstats.total_bin_stats.amask_01rms, bin_stats.amask_01rms);

            // Update counters and clear the bin vector.
            t0 = i;
            bin.clear();
        }

        // Stores the residual in the bin.
        bin.push_back(rdata[i].second);
    }

    // Calculate statistics for last bin
    if (BinStatsCalcErr::NOT_ERROR == calcBinStats(bin, bin_stats, rf, tlrnc))
        bins_ok++;

    rstats.bins.push_back(bin_stats);
    helpers::insert(rstats.total_bin_stats.amask_rfrms, bin_stats.amask_rfrms);
    helpers::insert(rstats.total_bin_stats.amask_01rms, bin_stats.amask_01rms);

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
    std::vector<long double> coefs = math::polynomialFit(xfit, yfit, 4);

    long double fit_step = (xfit.back() - xfit.front()) / (npoints - 1);
    long double xfit_step = xfit.front();
    while ( xfit_step <= xfit.back() )
    {
        auto res = math::applyPolynomial(coefs, xfit_step);
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

FullRateResCalcErr calculateFullRateResiduals(const common::RangeData &ranges_data, std::size_t bs,
                                              common::ResidualsData<> &rdata)
{

    // Calculate the residuals for each record
    std::vector<std::remove_reference_t<decltype(rdata)>::value_type::first_type> times;
    std::vector<std::remove_reference_t<decltype(rdata)>::value_type::second_type> resid;
    for (const auto& data : ranges_data)
    {
        times.push_back(std::get<0>(data));
        resid.push_back(std::get<1>(data) - std::get<2>(data) - std::get<3>(data));
    }

    rdata = binPolynomialDetrend(bs, times, resid);

    // Return no error.
    return FullRateResCalcErr::NOT_ERROR;
}

common::ResidualsData<> binPolynomialDetrend(int bs, const std::vector<long double> &times,
                                               const std::vector<long double> &resids, unsigned int degree)
{
    common::ResidualsData<> result;

    if (times.empty() || resids.empty())
        return result;

    std::vector<long double> xbin, ybin;
    long double time_orig = times.front();

    for (auto time_it = times.begin(), resid_it = resids.begin(); time_it != times.end() && resid_it != resids.end();
         ++time_it, ++resid_it)
    {
        if (*time_it - time_orig > bs)
        {
            auto coefs = dpslr::math::polynomialFit(xbin, ybin, degree);
            for (auto it_x = xbin.begin(), it_y = ybin.begin(); it_x != xbin.end() && it_y != ybin.end(); ++it_x, ++it_y)
            {
                result.push_back({*it_x, *it_y - dpslr::math::applyPolynomial(coefs, *it_x)});
            }
            xbin.clear();
            ybin.clear();
            time_orig = *time_it;

        }

        xbin.push_back(*time_it);
        ybin.push_back(*resid_it);
    }

    if (!xbin.empty() && !ybin.empty())
    {
        auto coefs = dpslr::math::polynomialFit(xbin, ybin, 9);
        for (auto it_x = xbin.begin(), it_y = ybin.begin(); it_x != xbin.end() && it_y != ybin.end(); ++it_x, ++it_y)
        {
            result.push_back({*it_x, *it_y - dpslr::math::applyPolynomial(coefs, *it_x)});
        }
    }

    return result;
}

std::vector<std::size_t> histPrefilterSLR(const std::vector<double> &times, const std::vector<double> &resids,
                                       double bs, double depth, unsigned min_ph, unsigned divisions)
{
    // Check the input data.
    if (times.empty() || resids.empty() || times.size() != resids.size() || depth <= 0 || bs <= 0 || divisions <= 0)
        return {};

    // Containers and auxiliar variables.
    std::vector<std::size_t> selected_ranges;
    std::size_t first = 0;
    double _depth = depth/divisions;
    unsigned _min_ph = min_ph/divisions;
    auto bins_indexes = extractBins(times, resids, bs, BinDivisionEnum::DAY_FIXED);

    for (const auto& bin_indexes : bins_indexes)
    {
        // Compute selected ranges from the bin
        auto bin_resids = helpers::extract(resids, bin_indexes);
        auto bin_selected = histPrefilterBinSLR(bin_resids, _depth, _min_ph);
        std::transform(bin_selected.begin(), bin_selected.end(), std::back_inserter(selected_ranges),
                       [first](const auto& idx){return  idx + first;});

        // Update the first bin index variable.
        first += bin_indexes.size();
    }

    // Return the result container.
    return selected_ranges;
}

std::vector<std::size_t> histPrefilterBinSLR(const std::vector<double> &resids_bin, double depth, unsigned min_ph)
{
    std::vector<std::size_t> selected_ranges;

    // Check if the residuals bin is not empty.
    if(!resids_bin.empty())
    {
        // Compute the range gate width.
        auto edges = std::minmax_element(resids_bin.begin(), resids_bin.end());
        long double rg_width = std::abs(*edges.first) + std::abs(*edges.second);

        // Get the histogram division size.
        std::size_t hist_size = static_cast<std::size_t>(std::floor(rg_width/depth));

        // Calculate histogram of residuals in bin.
        auto histcount_res = dpslr::math::histcounts1D(resids_bin, hist_size, *edges.first, *edges.second);

        // Check which histogram bins have more than min_photons count. They will be selected.
//        std::vector<std::size_t> sel_bins;
//        for (std::size_t bin_idx = 0; bin_idx < histcount_res.size(); bin_idx++)
//            if (std::get<0>(histcount_res[bin_idx]) >= min_photons)
//                sel_bins.push_back(bin_idx);

//        // Get points which are inside of selected histogram bins
//        for (std::size_t res_idx = 0; res_idx < resids_bin.size(); res_idx++)
//        {
//            bool select = false;
//            std::size_t sel_bin_idx = 0;
//            while(!select && sel_bin_idx < sel_bins.size())
//            {
//                const auto& bin_res = histcount_res[sel_bins[sel_bin_idx]];
//                if (resids_bin[res_idx] >= std::get<1>(bin_res) && resids_bin[res_idx] < std::get<2>(bin_res))
//                    select = true;
//                sel_bin_idx++;
//            }

//            // Store the selected range.
//            if (select)
//                selected_ranges.push_back(res_idx);
//        }

//        auto it = std::max_element(histcount_res.begin(), histcount_res.end(),
//                         [](const auto& a, const auto& b){return std::get<0>(a) < std::get<0>(b);});

//        if (it != histcount_res.end() && std::get<0>(*it) >= min_photons)
//        {
//            // Get points which are inside of selected histogram bins
//            for (std::size_t res_idx = 0; res_idx < resids_bin.size(); res_idx++)
//            {
//                if (resids_bin[res_idx] >= std::get<1>(*it) && resids_bin[res_idx] < std::get<2>(*it))
//                    selected_ranges.push_back(res_idx);
//            }
//        }

        auto it = std::max_element(histcount_res.begin(), histcount_res.end(),
                         [](const auto& a, const auto& b){return std::get<0>(a) < std::get<0>(b);});

        if (it != histcount_res.end() && std::get<0>(*it) >= min_ph)
        {
            // Iterator pointing to first bin NOT valid
            auto it_lower = it - 1;
            auto it_upper = it + 1;

            while (it_lower >= histcount_res.begin() && std::get<0>(*it_lower) >= min_ph)
                it_lower--;

            while (it_upper != histcount_res.end() && std::get<0>(*it_upper) >= min_ph)
                it_upper++;

            // Get points which are inside of selected histogram bins
            for (std::size_t res_idx = 0; res_idx < resids_bin.size(); res_idx++)
            {
                bool select = false;
                auto it_bin = it_lower + 1;
                while(!select && it_bin < it_upper)
                {
                    if (resids_bin[res_idx] >= std::get<1>(*it_bin) && resids_bin[res_idx] < std::get<2>(*it_bin))
                        select = true;
                    it_bin++;
                }

                // Store the selected range.
                if (select)
                    selected_ranges.push_back(res_idx);
            }
        }

    }

    return selected_ranges;
}

std::vector<std::size_t> histPostfilterSLR(const std::vector<double> &times, const std::vector<double> &data,
                                            double bs, double depth)
{
    // Call to the prefilter disabling the ph contributions.
    //return histPrefilterSLR(times, data, bs, depth, 0);

    // Auxiliar containers.
    std::vector<double> detrend_resids;
    std::vector<std::size_t> sel_indexes;
    double rf = depth *1.5; // depth / 2 * 2.5
    std::vector<double> y_vec;

    // Detrend the residuals.
    //detrend_resids = dpslr::math::detrend(times, data, 9);

    auto coefs = dpslr::math::polynomialFit(times, data, 9);

    for (std::size_t i = 0; i < data.size(); i++)
    {
        double y_interp = dpslr::math::applyPolynomial(coefs, times[i]);

        if (data[i] >= y_interp - rf && data[i] <= y_interp + rf)
            sel_indexes.push_back(i);

    }
    return sel_indexes;
}

template <typename T>
std::vector<std::size_t> windowPrefilterPrivate(const std::vector<T> &resids, T upper, T lower)
{
    // Check the input.
    if(resids.empty() || upper <= lower)
        return {};

    // Auxiliar containers.
    std::vector<std::size_t> indexes;

    // Get the acepted residuals.
    for(std::size_t i = 0; i < resids.size(); i++)
        if(resids[i] <= upper && resids[i] >= lower)
            indexes.push_back(i);

    // Return the indexes.
    return indexes;
}

std::vector<std::size_t> windowPrefilter(const std::vector<long double> &resids, long double upper, long double lower)
{
    return windowPrefilterPrivate(resids, upper, lower);
}

std::vector<std::size_t> windowPrefilter(const std::vector<double> &resids, double upper, double lower)
{
    return windowPrefilterPrivate(resids, upper, lower);
}

}
}
