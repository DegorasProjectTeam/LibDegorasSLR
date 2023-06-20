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

#include "LibDPSLR/crdutils.h"
#include "LibDPSLR/utils.h"
#include "LibDPSLR/common.h"
#include "LibDPSLR/algorithms.h"

namespace dpslr{
namespace crdutils{


StatsGenErr generateStatsRecord(std::size_t bs, const geo::frames::GeodeticPoint<long double> &stat_geodetic,
                                const geo::frames::GeocentricPoint<long double> &stat_geocentric,
                                const CPF &cpf, CRD &crd, double rf, double tlrnc)
{
    // Structs for storing data and error variables.
    dpslr::algorithms::ResidualsStats stats;
    CRDData::StatisticsRecord stats_record;
    dpslr::common::ResidualsData<> rdata;
    dpslr::algorithms::FullRateResCalcErr err_rescal;
    dpslr::algorithms::ResiStatsCalcErr err_statscal;

    // Calculate the full rate residuals.
    err_rescal = dpslr::algorithms::calculateFullRateResiduals(cpf, crd, stat_geodetic, stat_geocentric, bs, rdata);

    // If there was an error while calculating residuals, return error
    if(err_rescal != dpslr::algorithms::FullRateResCalcErr::NOT_ERROR)
        return static_cast<StatsGenErr>(err_rescal);

    // Calculate the residuals stats.
    err_statscal = dpslr::algorithms::calculateResidualsStats(bs, rdata, stats, rf, tlrnc);

    if(err_statscal == dpslr::algorithms::ResiStatsCalcErr::STATS_CALC_FAILED)
        return StatsGenErr::STATS_CALC_FAILED;

    // If error is not fatal, store the statistics data in the Record.
    stats_record.rms = stats.total_bin_stats.stats_rfrms.rms;
    stats_record.skew = stats.total_bin_stats.stats_rfrms.skew;
    stats_record.kurtosis = stats.total_bin_stats.stats_rfrms.kurt;
    // TODO: pending to confirm this:
    stats_record.peak = stats.total_bin_stats.stats_rfrms.peak - stats.total_bin_stats.stats_rfrms.mean;

    // TODO: determine quality
    stats_record.quality = CRDData::DataQualityEnum::UNDEFINED_QUALITY;
    stats_record.system_cfg_id = crd.getConfiguration().systemConfiguration()->system_cfg_id;

    // Set the statistic record.
    crd.getData().setStatisticsRecord(std::move(stats_record));

    // Check for non fatal error.
    if(err_statscal == dpslr::algorithms::ResiStatsCalcErr::SOME_BINS_CALC_FAILED)
        return StatsGenErr::SOME_BINS_CALC_FAILED;
    else
        return StatsGenErr::NOT_ERROR;
}


OverallCaliGencErr generateOverallCalibration(CRDData::ShiftTypeEnum shift_mode, CRD &crd)
{
    // Aux variables.
    using std::chrono::duration_cast;
    using std::chrono::duration;

    // Delete the current overall record and set to nullptr.
    crd.getData().clearOverallCalibrationRecord();

    // Calculate the middle of pass as start + ((end - start) / 2)
    auto start_pass_point = crd.getHeader().sessionHeader()->start_time;
    auto end_pass_point = crd.getHeader().sessionHeader()->end_time;

    long double time_tag_middle = dpslr::utils::timePointToSecsDay(start_pass_point) +
            (duration_cast<duration<long double>>(end_pass_point - start_pass_point).count() / 2.0L);

    const std::vector<CRDData::CalibrationRecord>& cal_records = crd.getData().calibrationRecords();

    // Check if we have detail calibrations.
    if(!crd.getData().calibrationRecords().empty())
    {
        // Create new overall record.
        // Then, the common data is automatically assigned.
        CRDData::CalibrationRecord cal_overall_record(cal_records.front());

        // Set calibration overall
        cal_overall_record.is_overall = true;

        // Calculate the overall data.
        if(cal_records.size() > 1)
        {
            cal_overall_record.time_tag = time_tag_middle;
            cal_overall_record.shift_type = shift_mode;

            // In this case, span is combined.
            cal_overall_record.span = CRDData::CalibrationSpan::COMBINED;

            // Calculate the shift data.
            if(shift_mode == CRDData::ShiftTypeEnum::PRE_TO_POST)
            {
                auto itpre = std::find_if(cal_records.begin(), cal_records.end(),
                    [](const CRDData::CalibrationRecord& cal)
                {return cal.span == CRDData::CalibrationSpan::PRE_CALIBRATION;});

                auto itpost = std::find_if(cal_records.begin(), cal_records.end(),
                    [](const CRDData::CalibrationRecord& cal)
                {return cal.span == CRDData::CalibrationSpan::POST_CALIBRATION;});

                if(itpre == cal_records.end() || itpost == cal_records.end())
                    return OverallCaliGencErr::MISSING_PREPOST;
                else
                    cal_overall_record.delay_shift = itpost->calibration_delay - itpre->calibration_delay;
            }
            else if(shift_mode == CRDData::ShiftTypeEnum::MIN_TO_MAX)
            {
                // Comparation lambda.
                auto lambda = [](const CRDData::CalibrationRecord &a, const CRDData::CalibrationRecord &b)
                              {return a.calibration_delay < b.calibration_delay;};

                // Search the maximum and minimum and calculate the shift.
                auto itmax = std::max_element(cal_records.begin(), cal_records.end(), lambda);
                auto itmin = std::min_element(cal_records.begin(), cal_records.end(), lambda);
                cal_overall_record.delay_shift = itmax->calibration_delay - itmin->calibration_delay;
            }
            else
                return OverallCaliGencErr::SHIFT_NOT_IMPLEMENTED;

            // Search for missing data used value.
            auto itdataused = std::find_if(cal_records.begin(), cal_records.end(),
                [](const CRDData::CalibrationRecord& cal){return !cal.data_used;});

            // Search for missing data recorded value.
            auto itdatarecorded = std::find_if(cal_records.begin(), cal_records.end(),
                [](const CRDData::CalibrationRecord& cal){return !cal.data_recorded;});

            // Search for missing return rate value.
            auto itreturnrate = std::find_if(cal_records.begin(), cal_records.end(),
                [](const CRDData::CalibrationRecord& cal){return !cal.return_rate;});

            // If we dont have missing data used values, calculate and stores the summatory.
            if(itdataused != cal_records.end())
                cal_overall_record.data_used = {};
            else
            {
                cal_overall_record.data_used =
                        std::accumulate(cal_records.begin(), cal_records.end(), 0,
                            [](int a, const CRDData::CalibrationRecord& b)
                                {return a + b.data_used.value();});
            }

            // If we dont have missing data recorded values, calculate and stores the summatory.
            if(itdatarecorded != cal_records.end())
                cal_overall_record.data_recorded = {};
            else
            {
                cal_overall_record.data_recorded =
                        std::accumulate(cal_records.begin(), cal_records.end(), 0,
                            [](int a, const CRDData::CalibrationRecord& b)
                                {return a + b.data_recorded.value();});
            }

            // If we dont have missing return rate values, calculate and stores the mean.
            if(itreturnrate != cal_records.end())
                cal_overall_record.return_rate = {};
            else
            {
                cal_overall_record.return_rate =
                        (std::accumulate(cal_records.begin(),
                                         cal_records.end(), 0.0,
                                        [](double a, const CRDData::CalibrationRecord& b)
                                        {return a + b.return_rate.value();}))
                        / cal_records.size();
            }

            // Store the calibration delay mean.
            cal_overall_record.calibration_delay =
                    (std::accumulate(cal_records.begin(),
                                     cal_records.end(), 0.0,
                                    [](double a, const CRDData::CalibrationRecord& b)
                                    {return a + b.calibration_delay;}))
                    / cal_records.size();

            // Store the skew mean.
            cal_overall_record.skew =
                    (std::accumulate(cal_records.begin(),
                                     cal_records.end(), 0.0,
                                    [](double a, const CRDData::CalibrationRecord& b)
                                    {return a + b.skew;}))
                    / cal_records.size();

            // Store the rms mean.
            cal_overall_record.rms =
                    (std::accumulate(cal_records.begin(),
                                     cal_records.end(), 0.0,
                                    [](double a, const CRDData::CalibrationRecord& b)
                                    {return a + b.rms;}))
                    / cal_records.size();

            // Store the kurtosis mean.
            cal_overall_record.kurtosis =
                    (std::accumulate(cal_records.begin(),
                                     cal_records.end(), 0.0,
                                    [](double a, const CRDData::CalibrationRecord& b)
                                    {return a + b.kurtosis;}))
                    / cal_records.size();

            // Store the peak mean.
            cal_overall_record.peak =
                    (std::accumulate(cal_records.begin(),
                                     cal_records.end(), 0.0,
                                    [](double a, const CRDData::CalibrationRecord& b)
                                    {return a + b.peak;}))
                    / cal_records.size();
        }

        // Set the calibration struct and return no error.
        crd.getData().setOverallCalibrationRecord(std::move(cal_overall_record));
        return OverallCaliGencErr::NOT_ERROR;
    }
    else
        return OverallCaliGencErr::EMPTY_CAL_RECORDS;  // Empty calibration records.
}

}}
