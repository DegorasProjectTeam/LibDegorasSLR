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
 * @file crd_data.cpp
 * @author Degoras Project Team.
 * @brief This file contains the implementation of the CRDData class that abstracts the data of ILRS CRD format.
 * @copyright EUPL License
 2305.1
***********************************************************************************************************************/

// C++ INCLUDES
// =====================================================================================================================
#include <iostream>
#include <cstring>
#include <algorithm>
#include <array>
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include <LibDegorasSLR/ILRS/formats/crd/records/crd_data.h>
// =====================================================================================================================

// LIBDPBASE INCLUDES
// =====================================================================================================================
#include <LibDegorasBase/Helpers/container_helpers.h>
#include <LibDegorasBase/Helpers/string_helpers.h>
// =====================================================================================================================

// =====================================================================================================================
using namespace dpslr::ilrs::common;
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace ilrs{
namespace crd{
// =====================================================================================================================

// --- CRD DATA CONST EXPRESSIONS --------------------------------------------------------------------------------------
const std::array<const char*, 10> CRDData::DataLineString {"10", "11", "12", "20", "21", "30", "40", "41", "50", "60"};
// ---------------------------------------------------------------------------------------------------------------------

void CRDData::clearAll()
{
    this->clearFullRateRecords();
    this->clearNormalPointRecords();
    this->clearMeteorologicalRecords();
    this->clearCalibrationRecords();
    this->clearRTCalibrationRecords();
    this->clearOverallCalibrationRecord();
    this->clearStatisticsRecord();
}

void CRDData::clearFullRateRecords() {this->fullrate_records.clear();}

void CRDData::clearNormalPointRecords() {this->normalpoint_records.clear();}

void CRDData::clearMeteorologicalRecords() {this->meteo_records.clear();}

void CRDData::clearCalibrationRecords() {this->cal_records.clear();}

void CRDData::clearRTCalibrationRecords() {this->rt_cal_records.clear();}

void CRDData::clearOverallCalibrationRecord() {this->cal_overall_record = {};}

void CRDData::clearStatisticsRecord() {this->stat_record = {};}

void CRDData::addFullRateRecord(const CRDData::FullRateRecord &rec) {this->fullrate_records.push_back(rec);}

void CRDData::addNormalPointRecord(const CRDData::NormalPointRecord &rec) {this->normalpoint_records.push_back(rec);}

void CRDData::addMeteorologicalRecord(const CRDData::MeteorologicalRecord &rec) {this->meteo_records.push_back(rec);}

void CRDData::addRealTimeCalibrationRecord(const CRDData::CalibrationRecord &rec) {this->rt_cal_records.push_back(rec);}

void CRDData::addCalibrationRecord(const CRDData::CalibrationRecord &rec) {this->cal_records.push_back(rec);}

void CRDData::setOverallCalibrationRecord(const CalibrationRecord &rec) {this->cal_overall_record = rec;}

void CRDData::setStatisticsRecord(const StatisticsRecord &rec) {this->stat_record = rec;}

void CRDData::setFullRateRecords(const std::vector<CRDData::FullRateRecord> &rec) {this->fullrate_records = rec;}

void CRDData::setNormalPointRecords(const std::vector<CRDData::NormalPointRecord> &rec)
{this->normalpoint_records = rec;}

void CRDData::setMeteorologicalRecords(const std::vector<CRDData::MeteorologicalRecord> &rec)
{this->meteo_records = rec;}

void CRDData::setCalibrationRecords(const std::vector<CRDData::CalibrationRecord> &rec) {this->cal_records = rec;}

void CRDData::setRealTimeCalibrationRecords(const std::vector<CRDData::CalibrationRecord> &rec)
{this->rt_cal_records = rec;}

algorithms::FlightTimeData CRDData::fullRateFlightTimeData() const
{
    // Container
    FlightTimeData data;

    // Get the X data (time tags).
    std::transform(this->fullrate_records.begin(), this->fullrate_records.end(), std::back_inserter(data),
        [](const CRDData::FullRateRecord& rec){return std::make_pair(rec.time_tag,rec.time_flight);});

    // Return the container.
    return data;
}

algorithms::FlightTimeData CRDData::normalPointFlightTimeData() const
{
    // Container
    FlightTimeData data;

    // Get the X data (time tags).
    std::transform(this->normalpoint_records.begin(), this->normalpoint_records.end(), std::back_inserter(data),
        [](const CRDData::NormalPointRecord& rec){return std::make_pair(rec.time_tag,rec.time_flight);});

    // Return the container.
    return data;
}

const std::vector<CRDData::FullRateRecord> &CRDData::fullRateRecords() const {return this->fullrate_records;}

const std::vector<CRDData::NormalPointRecord> &CRDData::normalPointRecords() const {return this->normalpoint_records;}

const std::vector<CRDData::MeteorologicalRecord> &CRDData::meteorologicalRecords() const {return this->meteo_records;}

const std::vector<CRDData::CalibrationRecord> &CRDData::realTimeCalibrationRecord() const {return this->rt_cal_records;}

const std::vector<CRDData::CalibrationRecord> &CRDData::calibrationRecords() const {return this->cal_records;}

const dpbase::Optional<CRDData::CalibrationRecord> &CRDData::calibrationOverallRecord() const {return this->cal_overall_record;}

const dpbase::Optional<CRDData::StatisticsRecord> &CRDData::statisticsRecord() const {return this->stat_record;}

std::vector<CRDData::FullRateRecord> &CRDData::fullRateRecords() {return this->fullrate_records;}

std::vector<CRDData::NormalPointRecord> &CRDData::normalPointRecords() {return this->normalpoint_records;}

std::vector<CRDData::MeteorologicalRecord> &CRDData::meteorologicalRecords() {return this->meteo_records;}

std::vector<CRDData::CalibrationRecord> &CRDData::realTimeCalibrationRecord() {return this->rt_cal_records;}

std::vector<CRDData::CalibrationRecord> &CRDData::calibrationRecords() {return this->cal_records;}

dpbase::Optional<CRDData::CalibrationRecord> &CRDData::calibrationOverallRecord() {return this->cal_overall_record;}

dpbase::Optional<CRDData::StatisticsRecord> &CRDData::statisticsRecord() {return this->stat_record;}

std::string CRDData::generateDataLines(float version, DataGenerationOption option) const
{
    // TODO: if there are for example no meteo lines, endl is inserted. This fails in several points.
    // Full rate lines
    std::stringstream data;

    // Lines order: 20, 41, 40 real time, 40 overall, 50, 10 / 11
    data << this->generateMeteoLines(version) << std::endl;
    data << this->generateCalibrationLines(version) << std::endl;
    if (this->stat_record)
        data << this->generateStatisticsLine(version) << std::endl;

    // Generate the lines 10 / 11 depending on the option.
    if(option == DataGenerationOption::FULL_RATE || option == DataGenerationOption::BOTH_DATA)
        data << this->generateFullRateLines(version);
    if(option == DataGenerationOption::NORMAL_POINT || option == DataGenerationOption::BOTH_DATA)
        data << this->generateNormalPointLines(version);

    // Return all the lines.
    return data.str();
}

std::string CRDData::generateFullRateLines(float version) const
{
    // Full rate lines
    std::stringstream fr_line;

    // Generate the FR line for each record.
    for(const auto& fr : this->fullrate_records)
    {
        // Add the comment block.
        if(!fr.comment_block.empty())
            fr_line << fr.generateCommentBlock() << std::endl;
        fr_line << fr.generateLine(version) << std::endl;
    }

    // Return all the lines.
    return fr_line.str().substr(0, fr_line.str().find_last_of('\n'));
}

std::string CRDData::generateNormalPointLines(float version) const
{
    // Normal Points lines
    std::stringstream np_line;

    // Generate the NP line for each record.
    for(const auto& np : this->normalpoint_records)
    {
        // Add the comment block.
        if(!np.comment_block.empty())
            np_line << np.generateCommentBlock() << std::endl;
        np_line << np.generateLine(version) << std::endl;
    }

    // Return all the lines.
    return np_line.str().substr(0, np_line.str().find_last_of('\n'));

}

std::string CRDData::generateMeteoLines(float version) const
{
    // Meteorological lines
    std::stringstream meteo_line;

    // Generate the meteo line for each record.
    for(const auto& meteo : this->meteo_records)
    {
        // Add the comment block.
        if(!meteo.comment_block.empty())
            meteo_line << meteo.generateCommentBlock() << std::endl;
        meteo_line << meteo.generateLine(version) << std::endl;
    }

    // Return all the lines.
    return meteo_line.str().substr(0, meteo_line.str().find_last_of('\n'));
}

std::string CRDData::generateCalibrationLines(float version) const
{
    // Calibration lines
    std::stringstream cal_line;

    // Generate the detail calibration line for each record. Lines 41 for v2 or 40 for v1.
    for(const auto& cal : this->cal_records)
    {
        // Add the comment block.
        if(!cal.comment_block.empty())
            cal_line << cal.generateCommentBlock() << std::endl;
        cal_line << cal.generateLine(version) << std::endl;
    }

    // Generate the real time calibration line for each record. Lines 40 for v2.
    if(version >= 2 && version < 3)
        for(const auto& cal : this->rt_cal_records)
        {
            // Add the comment block.
            if(!cal.comment_block.empty())
                cal_line << cal.generateCommentBlock() << std::endl;
            cal_line << cal.generateLine(version) << std::endl;
        }

    // Generate the overall calibration line for v2.
    if(version >= 2 && version < 3 && this->cal_overall_record)
    {
        // Add the comment block.
        if(!this->cal_overall_record->comment_block.empty())
            cal_line << this->cal_overall_record->generateCommentBlock() << std::endl;
        cal_line << this->cal_overall_record->generateLine(version) << std::endl;
    }

    // Return all the lines.
    return cal_line.str().substr(0, cal_line.str().find_last_of('\n'));
}

std::string CRDData::generateStatisticsLine(float version) const
{
    // Statistic line
    std::stringstream stat_line;

    // Generate the statistic line.
    if(this->stat_record)
    {
        // Add the comment block.
        if(!this->stat_record->comment_block.empty())
            stat_line << this->stat_record->generateCommentBlock() << std::endl;
        stat_line << this->stat_record->generateLine(version);
    }

    // Return the line
    return stat_line.str();
}

RecordReadError CRDData::readDataLine(const RecordLinePair &lpair, float version)
{
    switch (static_cast<DataRecordType>(lpair.first))
    {
        case DataRecordType::FULL_RATE_RECORD:
        return this->readFRDataLine(lpair.second, version);

        case DataRecordType::NORMAL_POINT_RECORD:
        return this->readNPDataLine(lpair.second, version);

        case DataRecordType::RANGE_SUPPLEMENT_RECORD:
        return RecordReadError::NOT_IMPLEMENTED;

        case DataRecordType::METEO_RECORD:
        return this->readMeteoDataLine(lpair.second, version);

        case DataRecordType::METEO_SUP_RECORD:
        return RecordReadError::NOT_IMPLEMENTED;

        case DataRecordType::POINTING_ANGLES_RECORD:
        return RecordReadError::NOT_IMPLEMENTED;

        case DataRecordType::CALIBRATION_OVERALL_RECORD:
        case DataRecordType::CALIBRATION_DETAIL_RECORD:
        return this->readCalDataLine(lpair.second, version);

        case DataRecordType::STATISTICS_RECORD:
        return this->readStatisticsData(lpair.second, version);

        case DataRecordType::COMPATIBILITY_RECORD:
        return RecordReadError::NOT_IMPLEMENTED;

        default:
        return RecordReadError::BAD_TYPE;
    }
}

RecordReadErrorMultimap CRDData::readData(const RecordLinesVector &rec_v, float version)
{
    // Aux variables.
    RecordLinePair rec_pair;
    RecordReadErrorMultimap error_map;
    RecordReadError error;
    int pos;

    // First, clear the data.
    this->clearAll();

    // Read the line and store the error, if any.
    for (const auto& rec : rec_v)
    {
        // Check that the record is a header record.
        if(dpbase::helpers::containers::find(DataLineString, rec.getIdToken(), pos))
        {
            // Store the record type in a pair.
            rec_pair = {pos, rec};

            // Read the record to store the data in memory.
            error = this->readDataLine(rec_pair, version);

            // Check for errors.
            if(error != RecordReadError::NOT_ERROR)
                error_map.emplace(static_cast<int>(error), rec);
        }
        else
        {
            error_map.emplace(static_cast<int>(RecordReadError::BAD_TYPE), rec);
        }
    }

    // Return the map with the errors. If no errors, the map will be empty.
    return error_map;
}

RecordReadError CRDData::readFRDataLine(const ConsolidatedRecord &record, float v)
{
    // Struct.
    FullRateRecord fr_record;

    // Get the tokens.
    std::vector<std::string> tokens = record.tokens;

    // Check the data size for each version.
    // For v1.
    if (v >= 1 && v < 2 && tokens.size() != 9)
        return RecordReadError::BAD_SIZE;
    // For v2.
    else if(v >= 2 && v < 3 && tokens.size() != 10)
        return RecordReadError::BAD_SIZE;
    // Check if the record type is correct.
    else if (tokens[0] != DataLineString[static_cast<int>(DataRecordType::FULL_RATE_RECORD)])
        return RecordReadError::BAD_TYPE;
    
    // All ok at this momment.
    else
        try
        {
            // Get the data.
            fr_record.time_tag = std::stold(tokens[1]);
            fr_record.time_flight = std::stold(tokens[2]);
            fr_record.system_cfg_id = tokens[3];
            fr_record.epoch_event = static_cast<EpochEvent>(std::stoi(tokens[4]));
            fr_record.filter_flag = static_cast<FilterFlag>(std::stoi(tokens[5]));
            fr_record.detector_channel = std::stoi(tokens[6]);
            fr_record.stop_number = std::stoi(tokens[7]);
            fr_record.receive_amp = (tokens[8] == "na" || std::stoi(tokens[8]) == 0) ?
                        decltype(fr_record.receive_amp)() : std::stoi(tokens[8]);

            // Only for v2.
            if(v >= 2 && v < 3)
                fr_record.transmit_amp = (tokens[9] == "na" || std::stoi(tokens[9]) == 0) ?
                            decltype(fr_record.transmit_amp)() : std::stoi(tokens[9]);

            // Add the associated comments, the line number, and the tokens.
            fr_record.comment_block = record.comment_block;
            fr_record.line_number = record.line_number;
            fr_record.tokens = record.tokens;

        } catch (...)
        {
            return RecordReadError::CONVERSION_ERROR;
        }

    // All ok, insert the struct.
    this->fullrate_records.push_back(fr_record);

    // Return the result.
    return RecordReadError::NOT_ERROR;
}

RecordReadError CRDData::readNPDataLine(const ConsolidatedRecord &record, float v)
{
    // Struct.
    NormalPointRecord np_record;

    // Get the tokens.
    std::vector<std::string> tokens = record.tokens;

    // Check the data size for each version.
    // For v1.
    if (v >= 1 && v < 2 && tokens.size() != 13)
        return RecordReadError::BAD_SIZE;
    // For v2.
    else if(v >= 2 && v < 3 && tokens.size() != 14)
        return RecordReadError::BAD_SIZE;
    // Check if the record type is correct.
    else if (tokens[0] != DataLineString[static_cast<int>(DataRecordType::NORMAL_POINT_RECORD)])
        return RecordReadError::BAD_TYPE;
    
    // All ok at this momment.
    else
        try
        {
            // Get the data.
            np_record.time_tag = std::stold(tokens[1]);
            np_record.time_flight = std::stold(tokens[2]);
            np_record.system_cfg_id = tokens[3];
            np_record.epoch_event = static_cast<EpochEvent>(std::stoi(tokens[4]));
            np_record.window_length = std::stod(tokens[5]);
            np_record.raw_ranges = std::stoi(tokens[6]);

            // RMS, skew and kurtosis (optional values).
            if(v >= 1 && v < 2)
            {
                np_record.bin_rms = tokens[7] == "-1" ? decltype(np_record.bin_rms)() : std::stod(tokens[7]);
                np_record.bin_skew = tokens[8] == "-1" ? decltype(np_record.bin_skew)() : std::stod(tokens[8]);
                np_record.bin_kurtosis = tokens[9] == "-1" ? decltype(np_record.bin_kurtosis)() : std::stod(tokens[9]);
                np_record.bin_peak = tokens[10] == "-1" ? decltype(np_record.bin_peak)() : std::stod(tokens[10]);

            }
            else if(v>=2 && v<3)
            {
                np_record.bin_rms = tokens[7] == "na" ? decltype(np_record.bin_rms)() : std::stod(tokens[7]);
                np_record.bin_skew = tokens[8] == "na" ? decltype(np_record.bin_skew)() : std::stod(tokens[8]);
                np_record.bin_kurtosis = tokens[9] == "na" ? decltype(np_record.bin_kurtosis)() : std::stod(tokens[9]);
                np_record.bin_peak = tokens[10] == "na" ? decltype(np_record.bin_peak)() : std::stod(tokens[10]);
            }

            // Rest of the data.
            np_record.return_rate = std::stod(tokens[11]);
            np_record.detector_channel = std::stoi(tokens[12]);

            // For version 2 only.
            if(v>=2 && v<3)
            {
                np_record.snr = (tokens[13] == "na") ? decltype(np_record.snr)() : std::stod(tokens[13]);
            }

            // Add the associated comments, the line number, and the tokens.
            np_record.comment_block = record.comment_block;
            np_record.line_number = record.line_number;
            np_record.tokens = record.tokens;

        } catch (...)
        {
            return RecordReadError::CONVERSION_ERROR;
        }

    // All ok, insert the struct.
    this->normalpoint_records.push_back(np_record);

    // Return the result.
    return RecordReadError::NOT_ERROR;
}

RecordReadError CRDData::readMeteoDataLine(const ConsolidatedRecord &record, float v)
{
    // Struct.
    MeteorologicalRecord meteo_record;

    // Get the tokens.
    std::vector<std::string> tokens = record.tokens;

    // Check the data size for each version.
    // For v1.
    if (v >= 1 && v < 3 && tokens.size() != 6)
        return RecordReadError::BAD_SIZE;
    // Check if the record type is correct.
    else if (tokens[0] != DataLineString[static_cast<int>(DataRecordType::METEO_RECORD)])
        return RecordReadError::BAD_TYPE;
    
    // All ok at this momment.
    else
        try
        {
            // Get the data.
            meteo_record.time_tag = std::stold(tokens[1]);
            meteo_record.surface_pressure = std::stod(tokens[2]);
            meteo_record.surface_temperature =  std::stod(tokens[3]);
            meteo_record.surface_relative_humidity = std::stod(tokens[4]);
            meteo_record.values_origin = static_cast<MeteoOrigin>(std::stoi(tokens[5]));

            // Add the associated comments, the line number, and the tokens.
            meteo_record.comment_block = record.comment_block;
            meteo_record.line_number = record.line_number;
            meteo_record.tokens = record.tokens;

        } catch (...)
        {
            return RecordReadError::CONVERSION_ERROR;
        }

    // All ok, insert the struct.
    this->meteo_records.push_back(meteo_record);

    // Return the result.
    return RecordReadError::NOT_ERROR;
}

RecordReadError CRDData::readCalDataLine(const ConsolidatedRecord &record, float v)
{
    // Struct.
    CalibrationRecord cal_record;

    // Get the tokens.
    std::vector<std::string> tokens = record.tokens;

    // Flags for check the record type.
    bool is_detail = tokens[0] == DataLineString[static_cast<int>(DataRecordType::CALIBRATION_DETAIL_RECORD)];
    bool is_overal = tokens[0] == DataLineString[static_cast<int>(DataRecordType::CALIBRATION_OVERALL_RECORD)];

    // Check the data size for each version.
    if (v >= 1 && v < 2 && tokens.size() != 16)
        return RecordReadError::BAD_SIZE;
    else if (v >= 2 && v < 3 && tokens.size() != 18)
        return RecordReadError::BAD_SIZE;

    // Check the version mismatch
    if (v >= 1 && v < 2 && is_detail)
        return RecordReadError::VERSION_MISMATCH;
    // Check the record type.
    else if (!is_detail && !is_overal)
        return RecordReadError::BAD_TYPE;

    // All ok at this momment.
    try
    {
        // Get the data.
        cal_record.time_tag = std::stold(tokens[1]);
        cal_record.data_type = static_cast<DataType>(std::stoi(tokens[2]));
        cal_record.system_cfg_id = tokens[3];

        // dpbase::Optional data.
        cal_record.data_recorded = (tokens[4] == "na" || std::stoi(tokens[4]) == -1) ?
                    dpbase::Optional<int>() : std::stoi(tokens[4]);
        cal_record.data_used = (tokens[5] == "na" || std::stoi(tokens[5]) == -1) ?
                    dpbase::Optional<int>() : std::stoi(tokens[5]);
        cal_record.target_dist_1w = (tokens[6] == "na" || tokens[6] == "-1") ?
                    dpbase::Optional<double>() : std::stod(tokens[6]);

        // Rest of the data.
        cal_record.calibration_delay = std::stod(tokens[7]);
        cal_record.delay_shift = std::stod(tokens[8]);
        cal_record.rms = std::stod(tokens[9]);
        cal_record.skew = std::stod(tokens[10]);
        cal_record.kurtosis = std::stod(tokens[11]);
        cal_record.peak = std::stod(tokens[12]);
        cal_record.cal_type = static_cast<CalibrationType>(std::stoi(tokens[13]));
        cal_record.shift_type = static_cast<ShiftType>(std::stoi(tokens[14]));
        cal_record.detector_channel = std::stoi(tokens[15]);

        // Add the associated comments, the line number, and the tokens.
        cal_record.comment_block = record.comment_block;
        cal_record.line_number = record.line_number;
        cal_record.tokens = record.tokens;

        // For v2 only.
        if (v >= 2 && v < 3)
        {
            cal_record.span = static_cast<CalibrationSpan>(std::stoi(tokens[16]));
            cal_record.return_rate = (tokens[17] == "na") ? decltype (cal_record.return_rate)() : std::stod(tokens[17]);
        }
    } catch (...)
    {
        return RecordReadError::CONVERSION_ERROR;
    }

    // Check the type of calibration for storing it.
    if (v >= 1 && v < 2)
    {
        cal_record.is_overall = false;
        this->cal_records.push_back(cal_record);
    }
    else if(v >= 2 && v < 3)
    {
        // Check span.
        if(cal_record.span == CalibrationSpan::NOT_APPLICABLE)
            return RecordReadError::NOT_IMPLEMENTED;
        else if(is_overal && cal_record.span == CalibrationSpan::REAL_TIME)
        {
            cal_record.is_overall = false;
            this->rt_cal_records.push_back(cal_record);
        }
        else if(is_overal && cal_record.span != CalibrationSpan::REAL_TIME)
        {
            // Set the cal record overall as true.
            cal_record.is_overall = true;

            // Create the new calibration overall.
            this->cal_overall_record = cal_record;
        }
        else if(is_detail && (cal_record.span == CalibrationSpan::COMBINED
                              || cal_record.span == CalibrationSpan::REAL_TIME))
            return RecordReadError::OTHER_ERROR;
        else if(is_detail)
        {
            cal_record.is_overall = false;
            this->cal_records.push_back(cal_record);
        }
        else
            return RecordReadError::OTHER_ERROR;
    }

    // Return the result.
    return RecordReadError::NOT_ERROR;
}

RecordReadError CRDData::readStatisticsData(const ConsolidatedRecord &record, float)
{
    // Delete the current data.
    this->clearStatisticsRecord();

    // Get the tokens.
    std::vector<std::string> tokens = record.tokens;

    // Check the data size.
    if (tokens.size() != 7)
        return RecordReadError::BAD_SIZE;
    // Check if the record type is correct.
    else if (tokens[0] != DataLineString[static_cast<int>(DataRecordType::STATISTICS_RECORD)])
        return RecordReadError::BAD_TYPE;

    // All ok at this momment.
    try
    {
        // Create statistic struct.
        StatisticsRecord sr;

        // Get the data.
        sr.system_cfg_id = tokens[1];
        sr.rms = std::stod(tokens[2]);
        sr.skew = (tokens[3] == "na" || tokens[3] == "-1") ? decltype(sr.skew)() : std::stod(tokens[3]);
        sr.kurtosis = (tokens[4] == "na" || tokens[4] == "-1") ?
                    decltype(sr.kurtosis)() : std::stod(tokens[4]);
        sr.peak = (tokens[5] == "na" || tokens[5] == "-1") ? decltype(sr.peak)() : std::stod(tokens[5]);
        sr.quality = static_cast<DataQuality>(std::stoi(tokens[6]));

        // Add the associated comments, the line number, and the tokens.
        sr.comment_block = record.comment_block;
        sr.line_number = record.line_number;
        sr.tokens = record.tokens;

        // Store stats record
        this->stat_record = std::move(sr);

    } catch (...)
    {
        // Return the error.
        return RecordReadError::CONVERSION_ERROR;
    }

    // Return the result.
    return RecordReadError::NOT_ERROR;
}

std::string CRDData::FullRateRecord::generateLine(float version) const
{
    // Base line.
    std::stringstream line_10;

    // For version 1 and 2.
    if (version >= 1 && version < 3)
    {
        line_10 << "10"
                << ' ' << dpbase::helpers::strings::numberToStr(this->time_tag, 18, 12)
                << ' ' <<dpbase::helpers::strings::numberToStr(this->time_flight, 18, 12)
                << ' ' << this->system_cfg_id
                << ' ' << static_cast<int>(this->epoch_event)
                << ' ' << static_cast<int>(this->filter_flag)
                << ' ' << this->detector_channel
                << ' ' << this->stop_number;

        // Check if we have receive amp.
        if(!this->receive_amp)
            line_10 << ' ' << (version >= 2 ? "na" : std::to_string(0));
        else
            line_10 << ' ' << this->receive_amp.value();
    }

    // For version 2 only.
    if (version >= 2 && version < 3)
    {
        line_10 << ' ' << (!this->transmit_amp ? "na" : std::to_string(this->transmit_amp.value()));
    }

    // Return LINE 10.
    return line_10.str();
}

std::string CRDData::NormalPointRecord::generateLine(float version) const
{
    // Base line.
    std::stringstream line_11;

    // For version 1 and 2.
    if (version >= 1 && version < 3)
    {
        line_11 << "11"
                << ' ' << dpbase::helpers::strings::numberToStr(this->time_tag, 18, 12)
                << ' ' << dpbase::helpers::strings::numberToStr(this->time_flight, 18, 12)
                << ' ' << this->system_cfg_id
                << ' ' << static_cast<int>(this->epoch_event)
                << ' ' << dpbase::helpers::strings::numberToStr(this->window_length, 6, 1)
                << ' ' << this->raw_ranges;

        // dpbase::Optional values.
        if(version>= 1 && version < 2)
        {
            line_11 << ' ' << (!this->bin_rms ? "-1" : dpbase::helpers::strings::numberToStr(this->bin_rms.value(), 9, 1))
                    << ' ' << (!this->bin_skew ? "-1" : dpbase::helpers::strings::numberToStr(this->bin_skew.value(), 7, 3))
                    << ' ' << (!this->bin_kurtosis ? "-1" : dpbase::helpers::strings::numberToStr(this->bin_kurtosis.value(), 7, 3))
                    << ' ' << (!this->bin_peak ? "-1" : dpbase::helpers::strings::numberToStr(this->bin_peak.value(), 9, 1));
        }
        else if (version >= 2 && version < 3)
        {
            line_11 << ' ' << (this->bin_rms ? dpbase::helpers::strings::numberToStr(this->bin_rms.value(), 9, 1) : "na")
                    << ' ' << (this->bin_skew ? dpbase::helpers::strings::numberToStr(this->bin_skew.value(), 7, 3) : "na")
                    << ' ' << (this->bin_kurtosis ? dpbase::helpers::strings::numberToStr(this->bin_kurtosis.value(), 7, 3) : "na")
                    << ' ' << (this->bin_peak ? dpbase::helpers::strings::numberToStr(this->bin_peak.value(), 9, 1) : "na");
        }

        // Rest of the data.
        line_11 << ' ' << dpbase::helpers::strings::numberToStr(this->return_rate, 5, 1)
                << ' ' << this->detector_channel;
    }

    // For version 2 only.
    if (version >= 2 && version < 3)
    {
        line_11 << ' ' << (this->snr ? dpbase::helpers::strings::numberToStr(this->snr.value(), 5, 1) : "na");
    }

    // Return LINE 11.
    return line_11.str();
}

std::string CRDData::MeteorologicalRecord::generateLine(float version) const
{
    // Base line.
    std::stringstream line_20;

    // For version 1 and 2.
    if (version >= 1 && version < 3)
    {
        line_20 << "20"
                << ' ' << dpbase::helpers::strings::numberToStr(this->time_tag, 18, 12)
                << ' ' << dpbase::helpers::strings::numberToStr(this->surface_pressure, 7, 2)
                << ' ' << dpbase::helpers::strings::numberToStr(this->surface_temperature, 6, 2)
                << ' ' << dpbase::helpers::strings::numberToStr(this->surface_relative_humidity, 4, 1)
                << ' ' << static_cast<int>(this->values_origin);
    }

    // Return LINE 20.
    return line_20.str();
}

std::string CRDData::CalibrationRecord::generateLine(float version) const
{
    // Base line.
    std::stringstream line;

    // First, we need to detect if the struct is a line 40 or line 41.
    // For version 1, we only have lines 40.
    if (version >= 1 && version < 2)
        line << "40";

    // For v2, if we have the flag span as real time or combined, then is line 40. Also we need the flag is_overall
    // to true. Remember call check data integrity before call this function to ckeck integrity.
    else if(version >= 2 && version < 3)
    {
        line << ((this->span == CRDData::CalibrationSpan::REAL_TIME || this->is_overall) ? "40" : "41");
    }

    // For v1 and v2.
    if (version >= 1 && version < 3)
    {
        line << ' ' << dpbase::helpers::strings::numberToStr(this->time_tag, 18, 12)
             << ' ' << static_cast<int>(this->data_type)
             << ' ' << this->system_cfg_id;

        // dpbase::Optional values.
        if(version>= 1 && version < 2)
        {
            line << ' ' << this->data_recorded.value_or(-1)
                 << ' ' << this->data_used.value_or(-1)
                 << ' ' << (!this->target_dist_1w ? "-1" : dpbase::helpers::strings::numberToStr(this->target_dist_1w.value(), 7, 3));
        }
        else if(version >= 2 && version < 3)
        {
            line << ' ' << (!this->data_recorded ? "na" : std::to_string(this->data_recorded.value()))
                 << ' ' << (!this->data_used ? "na" : std::to_string(this->data_used.value()))
                 << ' ' << (!this->target_dist_1w ? "na" : dpbase::helpers::strings::numberToStr(this->target_dist_1w.value(), 7, 3));
        }

        // Rest of the data.
        line << ' ' << dpbase::helpers::strings::numberToStr(this->calibration_delay, 10, 1)
             << ' ' << dpbase::helpers::strings::numberToStr(this->delay_shift, 8, 1)
             << ' ' << dpbase::helpers::strings::numberToStr(this->rms, 6, 1)
             << ' ' << dpbase::helpers::strings::numberToStr(this->skew, 7, 3)
             << ' ' << dpbase::helpers::strings::numberToStr(this->kurtosis, 7, 3)
             << ' ' << dpbase::helpers::strings::numberToStr(this->peak, 6, 1)
             << ' ' << static_cast<int>(this->cal_type)
             << ' ' << static_cast<int>(this->shift_type)
             << ' ' << this->detector_channel;
    }

    // For version 2 only.
    if (version >= 2 && version < 3)
        line << ' ' << static_cast<int>(this->span)
             << ' ' << (!this->return_rate ? "na" : dpbase::helpers::strings::numberToStr(this->return_rate.value(), 5, 1));

    // Return line 40 - 41.
    return line.str();
}

std::string CRDData::StatisticsRecord::generateLine(float version) const
{
    // Base line.
    std::stringstream line_50;

    // For version 1 and 2.
    if (version >= 1 && version < 3)
    {
        line_50 << "50"
                << ' ' << this->system_cfg_id
                << ' ' << dpbase::helpers::strings::numberToStr(this->rms, 6, 1);

        // dpbase::Optional data.
        if(version >= 1 && version < 2)
        {
            line_50 << ' ' << dpbase::helpers::strings::numberToStr(this->skew.value_or(-1), 7, 3)
                    << ' ' << dpbase::helpers::strings::numberToStr(this->kurtosis.value_or(-1), 7, 3)
                    << ' ' << dpbase::helpers::strings::numberToStr(this->peak.value_or(-1), 6, 1);
        }
        else if(version >= 2 && version < 3)
        {
            line_50 << ' ' << (!this->skew ? "na" : dpbase::helpers::strings::numberToStr(this->skew.value(), 7, 3))
                    << ' ' << (!this->kurtosis ? "na" : dpbase::helpers::strings::numberToStr(this->kurtosis.value(), 7, 3))
                    << ' ' << (!this->peak ? "na" : dpbase::helpers::strings::numberToStr(this->peak.value(), 6, 1));
        }

        // Rest of the data.
        line_50 << ' ' << static_cast<int>(this->quality);
    }

    // Return LINE 50.
    return line_50.str();
}

// =====================================================================================================================

}}} // END NAMESPACES
// =====================================================================================================================
