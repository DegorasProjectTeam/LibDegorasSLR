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


// ========== C++ INCLUDES =============================================================================================
#include <fstream>
#include <iostream>
#include <iomanip>
#include <numeric>
#include <cstring>
#include <algorithm>
#include <array>
// =====================================================================================================================


// ========== LOCAL INCLUDES ===========================================================================================
#include "LibDPSLR/class_crd.h"
#include "LibDPSLR/utils.h"
#include "LibDPSLR/helpers.h"
// =====================================================================================================================



// --- CRD DATA STRUCTS ------------------------------------------------------------------------------------------------
std::string CRDData::FullRateRecord::generateLine(float version) const
{
    // Base line.
    std::stringstream line_10;

    // For version 1 and 2.
    if (version >= 1 && version < 3)
    {
        line_10 << "10"
                << ' ' << dpslr::helpers::numberToStr(this->time_tag, 18, 12)
                << ' ' << dpslr::helpers::numberToStr(this->time_flight, 18, 12)
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
                << ' ' << dpslr::helpers::numberToStr(this->time_tag, 18, 12)
                << ' ' << dpslr::helpers::numberToStr(this->time_flight, 18, 12)
                << ' ' << this->system_cfg_id
                << ' ' << static_cast<int>(this->epoch_event)
                << ' ' << dpslr::helpers::numberToStr(this->window_length, 6, 1)
                << ' ' << this->raw_ranges;

        // Optional values.
        if(version>= 1 && version < 2)
        {
            line_11 << ' ' << (!this->bin_rms ? "-1" : dpslr::helpers::numberToStr(this->bin_rms.value(), 9, 1))
                    << ' ' << (!this->bin_skew ? "-1" : dpslr::helpers::numberToStr(this->bin_skew.value(), 7, 3))
                    << ' ' << (!this->bin_kurtosis ? "-1" : dpslr::helpers::numberToStr(this->bin_kurtosis.value(), 7, 3))
                    << ' ' << (!this->bin_peak ? "-1" : dpslr::helpers::numberToStr(this->bin_peak.value(), 9, 1));
        }
        else if (version >= 2 && version < 3)
        {
            line_11 << ' ' << (this->bin_rms ? dpslr::helpers::numberToStr(this->bin_rms.value(), 9, 1) : "na")
                    << ' ' << (this->bin_skew ? dpslr::helpers::numberToStr(this->bin_skew.value(), 7, 3) : "na")
                    << ' ' << (this->bin_kurtosis ? dpslr::helpers::numberToStr(this->bin_kurtosis.value(), 7, 3) : "na")
                    << ' ' << (this->bin_peak ? dpslr::helpers::numberToStr(this->bin_peak.value(), 9, 1) : "na");
        }

        // Rest of the data.
        line_11 << ' ' << dpslr::helpers::numberToStr(this->return_rate, 5, 1)
                << ' ' << this->detector_channel;
    }

    // For version 2 only.
    if (version >= 2 && version < 3)
    {
        line_11 << ' ' << (this->snr ? dpslr::helpers::numberToStr(this->snr.value(), 5, 1) : "na");
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
                << ' ' << dpslr::helpers::numberToStr(this->time_tag, 18, 12)
                << ' ' << dpslr::helpers::numberToStr(this->surface_pressure, 7, 2)
                << ' ' << dpslr::helpers::numberToStr(this->surface_temperature, 6, 2)
                << ' ' << dpslr::helpers::numberToStr(this->surface_relative_humidity, 4, 1)
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
        line << ' ' << dpslr::helpers::numberToStr(this->time_tag, 18, 12)
             << ' ' << static_cast<int>(this->data_type)
             << ' ' << this->system_cfg_id;

        // Optional values.
        if(version>= 1 && version < 2)
        {
            line << ' ' << this->data_recorded.value_or(-1)
                 << ' ' << this->data_used.value_or(-1)
                 << ' ' << (!this->target_dist_1w ? "-1" : dpslr::helpers::numberToStr(this->target_dist_1w.value(), 7, 3));
        }
        else if(version >= 2 && version < 3)
        {
            line << ' ' << (!this->data_recorded ? "na" : std::to_string(this->data_recorded.value()))
                 << ' ' << (!this->data_used ? "na" : std::to_string(this->data_used.value()))
                 << ' ' << (!this->target_dist_1w ? "na" : dpslr::helpers::numberToStr(this->target_dist_1w.value(), 7, 3));
        }

        // Rest of the data.
        line << ' ' << dpslr::helpers::numberToStr(this->calibration_delay, 10, 1)
             << ' ' << dpslr::helpers::numberToStr(this->delay_shift, 8, 1)
             << ' ' << dpslr::helpers::numberToStr(this->rms, 6, 1)
             << ' ' << dpslr::helpers::numberToStr(this->skew, 7, 3)
             << ' ' << dpslr::helpers::numberToStr(this->kurtosis, 7, 3)
             << ' ' << dpslr::helpers::numberToStr(this->peak, 6, 1)
             << ' ' << static_cast<int>(this->cal_type)
             << ' ' << static_cast<int>(this->shift_type)
             << ' ' << this->detector_channel;
    }

    // For version 2 only.
    if (version >= 2 && version < 3)
        line << ' ' << static_cast<int>(this->span)
             << ' ' << (!this->return_rate ? "na" : dpslr::helpers::numberToStr(this->return_rate.value(), 5, 1));

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
                << ' ' << dpslr::helpers::numberToStr(this->rms, 6, 1);

        // Optional data.
        if(version >= 1 && version < 2)
        {
            line_50 << ' ' << dpslr::helpers::numberToStr(this->skew.value_or(-1), 7, 3)
                    << ' ' << dpslr::helpers::numberToStr(this->kurtosis.value_or(-1), 7, 3)
                    << ' ' << dpslr::helpers::numberToStr(this->peak.value_or(-1), 6, 1);
        }
        else if(version >= 2 && version < 3)
        {
            line_50 << ' ' << (!this->skew ? "na" : dpslr::helpers::numberToStr(this->skew.value(), 7, 3))
                    << ' ' << (!this->kurtosis ? "na" : dpslr::helpers::numberToStr(this->kurtosis.value(), 7, 3))
                    << ' ' << (!this->peak ? "na" : dpslr::helpers::numberToStr(this->peak.value(), 6, 1));
        }

        // Rest of the data.
        line_50 << ' ' << static_cast<int>(this->quality);
    }

    // Return LINE 50.
    return line_50.str();
}
// ---------------------------------------------------------------------------------------------------------------------

// =====================================================================================================================

// ========== CRD ======================================================================================================

// --- CRD CONST EXPRESSIONS -------------------------------------------------------------------------------------------
const std::array<const char*, 6> CRD::ExtensionsString {"frd", "npt", "fr2", "np2", "qlk", "crd"};
// ---------------------------------------------------------------------------------------------------------------------

// --- CRD CONSTRUCTORS ------------------------------------------------------------------------------------------------
CRD::CRD(float version):
    empty_(false)
{
    // Set the version and creation time at Format Header
    this->header.formatHeader()->crd_version = version;
    this->header.formatHeader()->crd_production_date = dpslr::common::HRTimePoint::clock::now();
}

CRD::CRD(const std::string &crd_filepath, OpenOptionEnum option)
{
    this->openCRDFile(crd_filepath, option);
}
// ---------------------------------------------------------------------------------------------------------------------

// --- CRD CLEAR METHODS -----------------------------------------------------------------------------------------------
void CRD::clearCRD()
{
    // Clear the contents.
    this->clearCRDContents();

    // Clear the error storage.
    this->last_read_error_ = ReadFileErrorEnum::NOT_ERROR;
    this->last_error_record_ = {};
    this->read_header_errors.clear();
    this->read_cfg_errors.clear();
    this->read_data_errors.clear();

    // Clear others.
    this->crd_filename.clear();
    this->crd_fullpath.clear();

}

void CRD::clearCRDContents()
{
    // Clear the data, configuration and header and se to empty.
    this->header.clearAll();
    this->configuration.clearAll();
    this->data.clearAll();

    this->empty_ = true;
}

void CRD::clearCRDData()
{
    this->data.clearAll();
}

void CRD::clearCRDHeader()
{
   this->header.clearAll();
}

void CRD::clearCRDConfiguration()
{
    this->configuration.clearAll();
}
// ---------------------------------------------------------------------------------------------------------------------

// --- CRD GETTERS -----------------------------------------------------------------------------------------------------
CRDHeader &CRD::getHeader() {return this->header;}

CRDConfiguration &CRD::getConfiguration() {return this->configuration;}

CRDData &CRD::getData() {return this->data;}

const CRDHeader &CRD::getHeader() const {return this->header;}

const CRDConfiguration &CRD::getConfiguration() const {return this->configuration;}

const CRDData &CRD::getData() const {return this->data;}

const dpslr::common::RecordReadErrorMultimap &CRD::getReadHeaderErrors() const {return this->read_header_errors;}

const dpslr::common::RecordReadErrorMultimap &CRD::getReadCfgErrors() const {return this->read_cfg_errors;}

const dpslr::common::RecordReadErrorMultimap &CRD::getReadDataErrors() const {return this->read_data_errors;}

CRD::ReadFileErrorEnum CRD::getLastReadError() const {return this->last_read_error_;}

const dpslr::common::optional<dpslr::common::ConsolidatedRecordStruct> &CRD::getLastReadErrorRecord() const
{return this->last_error_record_;}

const std::string &CRD::getSourceFilename() const {return this->crd_filename;}

const std::string &CRD::getSourceFilepath() const {return this->crd_fullpath;}

std::string CRD::getStandardFilename(TargetIdOptionEnum option) const
{
    // Variables.
    std::string filename;

    // Check the preconditions.
    if(!this->header.formatHeader() || !this->header.sessionHeader() ||
            !this->header.stationHeader() || !this->header.targetHeader())
        return "";

    // For v2 only.
    if(this->header.formatHeader()->crd_version >= 2 && this->header.formatHeader()->crd_version < 3)
    {
        // For non ILRS tracking.
        if(this->header.stationHeader()->network != "ILRS")
            filename.append(dpslr::helpers::toLower(this->header.stationHeader()->network) + '_');
    }

    // Get the CDP pad identifier for station
    filename.append(std::to_string(this->header.stationHeader()->cdp_pad_identifier) + '_');

    // Get the target identifier.
    switch (option)
    {
        case TargetIdOptionEnum::ILRS_ID:
        filename.append(this->header.targetHeader()->ilrsid + "_crd_");
        break;

        case TargetIdOptionEnum::SHORT_COSPAR:
        filename.append(dpslr::utils::ilrsidToShortcospar(this->header.targetHeader()->ilrsid) + "_crd_");
        break;

        case TargetIdOptionEnum::COSPAR:
        filename.append(dpslr::utils::ilrsidToCospar(this->header.targetHeader()->ilrsid) + "_crd_");
        break;

        case TargetIdOptionEnum::NORAD:
        filename.append(this->header.targetHeader()->norad + "_crd_");
        break;

        case TargetIdOptionEnum::TARGET_NAME:
        filename.append(dpslr::helpers::toLower(this->header.targetHeader()->name) + "_crd_");
        break;
    }

    // Append the starting date of the pass from H4.
    std::time_t time = std::chrono::system_clock::to_time_t(this->header.sessionHeader()->start_time);
    std::tm* tm = gmtime(&time);
    char start_date[14];
    std::strftime(start_date, 14, "%Y%m%d_%H%M", tm);
    filename.append(start_date);

    // Append the data release.
    std::stringstream rr;
    rr << std::setw(2) << std::setfill('0') << this->header.sessionHeader()->data_release;
    filename.append('_' + rr.str() + '.');

    // Get the data type.
    if(this->header.formatHeader()->crd_version >= 1 && this->header.formatHeader()->crd_version < 2)
        filename.append(CRD::ExtensionsString[static_cast<int>(this->header.sessionHeader()->data_type)]);
    else if(this->header.formatHeader()->crd_version >= 2 && this->header.formatHeader()->crd_version < 3)
        filename.append(CRD::ExtensionsString[static_cast<int>(this->header.sessionHeader()->data_type)+2]);

    // Return the convention filename.
    return filename;
}
// ---------------------------------------------------------------------------------------------------------------------

// --- CRD OBSERVERS ---------------------------------------------------------------------------------------------------
bool CRD::empty() const {return this->empty_;}
// ---------------------------------------------------------------------------------------------------------------------

// --- CRD FILE METHODS ------------------------------------------------------------------------------------------------
CRD::ReadFileErrorEnum CRD::openCRDFile(const std::string &crd_filepath, CRD::OpenOptionEnum open_option)
{
    // Variables.
    ReadFileErrorEnum error = ReadFileErrorEnum::NOT_ERROR;
    dpslr::common::RecordLinesVector data_vector;
    dpslr::common::RecordLinesVector cfg_vector;
    dpslr::common::RecordLinesVector header_vector;
    float version = 1.;
    bool header_finished = false;
    bool cfg_finished = false;
    bool data_finished = false;
    bool eos_finished = false;
    bool eof_finished = false;
    bool read_finished = false;

    // Clear the CRD.
    this->clearCRD();

    // Open the file using our custom input file stream.
    dpslr::helpers::InputFileStream crd_stream(crd_filepath);

    // Check if the stream is open.
    if(!crd_stream.is_open())
    {
        this->last_read_error_ = ReadFileErrorEnum::FILE_NOT_FOUND;
        return ReadFileErrorEnum::FILE_NOT_FOUND;
    }

    // Check if the stream is empty.
    if(crd_stream.isEmpty())
    {
        this->last_read_error_ = ReadFileErrorEnum::FILE_EMPTY;
        return ReadFileErrorEnum::FILE_EMPTY;
    }

    // Store the file path and name.
    this->crd_fullpath = crd_filepath;
    this->crd_filename = dpslr::helpers::split<std::vector<std::string>>(crd_filepath, "/").back();

    // Open the header.
    while (!read_finished)
    {
        // Auxiliar variables.
        dpslr::common::ConsolidatedRecordStruct record;
        CRD::ReadRecordResultEnum read_result;

        // Get the next record.
        read_result = this->readRecord(crd_stream, record);

        // Get the type.
        CRDRecordsTypeEnum type = static_cast<CRDRecordsTypeEnum>(record.generic_record_type);

        // Check the errors.
        if(read_result == ReadRecordResultEnum::UNDEFINED_RECORD)
        {
            this->clearCRDContents();
            this->last_read_error_ = ReadFileErrorEnum::UNDEFINED_RECORD;
            this->last_error_record_ = record;
            return ReadFileErrorEnum::UNDEFINED_RECORD;
        }
        else if(type == CRDRecordsTypeEnum::HEADER_RECORD && header_finished)
        {
            this->clearCRDContents();
            this->last_read_error_ = ReadFileErrorEnum::ORDER_ERROR;
            this->last_error_record_ = record;
            return ReadFileErrorEnum::ORDER_ERROR;
        }
        else if(type == CRDRecordsTypeEnum::CFG_RECORD &&  cfg_finished)
        {
            this->clearCRDContents();
            this->last_read_error_ = ReadFileErrorEnum::ORDER_ERROR;
            this->last_error_record_ = record;
            return ReadFileErrorEnum::ORDER_ERROR;
        }
        else if(type == CRDRecordsTypeEnum::DATA_RECORD && data_finished)
        {
            this->clearCRDContents();
            this->last_read_error_ = ReadFileErrorEnum::ORDER_ERROR;
            this->last_error_record_ = record;
            return ReadFileErrorEnum::ORDER_ERROR;
        }
        else if(type == CRDRecordsTypeEnum::EOS_RECORD && eos_finished)
        {
            this->clearCRDContents();
            this->last_read_error_ = ReadFileErrorEnum::MULTIPLE_EOS;
            this->last_error_record_ = record;
            return ReadFileErrorEnum::MULTIPLE_EOS;
        }
        else if(type == CRDRecordsTypeEnum::EOF_RECORD && !eos_finished)
        {
            this->clearCRDContents();
            this->last_read_error_ = ReadFileErrorEnum::EOS_NOT_FOUND;
            return ReadFileErrorEnum::EOS_NOT_FOUND;
        }

        // Check the record type.
        if(type == CRDRecordsTypeEnum::HEADER_RECORD)
        {
             header_vector.push_back(record);
        }
        else if(type == CRDRecordsTypeEnum::CFG_RECORD)
        {
            // Check if we send the header data to the internal structs.
            if(!header_finished)
            {
                // Check if we hava header records.
                if(!header_vector.empty())
                {
                    // Save the possible issues.
                    this->read_header_errors = this->header.readHeader(header_vector);
                    header_finished = true;

                    // Get the version:
                    if(this->header.formatHeader())
                        version = this->header.formatHeader()->crd_version;
                    else
                    {
                        this->clearCRDContents();
                        this->last_read_error_ = ReadFileErrorEnum::VERSION_UNKNOWN;
                        return ReadFileErrorEnum::VERSION_UNKNOWN;
                    }
                }
                else
                {
                    this->clearCRDContents();
                    this->last_read_error_ = ReadFileErrorEnum::NO_HEADER_FOUND;
                    return ReadFileErrorEnum::NO_HEADER_FOUND;
                }
            }

            // Store the configuration record.
            cfg_vector.push_back(record);
        }
        else if(type == CRDRecordsTypeEnum::DATA_RECORD)
        {
            // Check if we send the header data to the internal structs.
            if(!cfg_finished)
            {
                // Check if we hava header records.
                if(!cfg_vector.empty())
                {
                    // Save the possible issues.
                    this->read_cfg_errors = this->configuration.readConfiguration(cfg_vector, version);
                    cfg_finished = true;
                }
                else
                {
                    this->clearCRDContents();
                    this->last_read_error_ = ReadFileErrorEnum::NO_CFG_FOUND;
                    return ReadFileErrorEnum::NO_CFG_FOUND;
                }
            }

            // Store the data record.
            data_vector.push_back(record);
        }
        else if(type == CRDRecordsTypeEnum::EOS_RECORD)
        {
            // Check if we send the header data to the internal structs.
            if(!data_finished)
            {
                // Check if we hava header records.
                if(data_vector.empty())
                {
                    this->clearCRDContents();
                    this->last_read_error_ = ReadFileErrorEnum::NO_DATA_FOUND;
                    return ReadFileErrorEnum::NO_DATA_FOUND;
                }
                else if(!header_finished || !cfg_finished)
                {
                    this->clearCRDContents();
                    this->last_read_error_ = ReadFileErrorEnum::FILE_TRUNCATED;
                    return ReadFileErrorEnum::FILE_TRUNCATED;
                }
                else if(!data_vector.empty())
                {
                    // Save the possible issues.
                    this->read_data_errors = this->data.readData(data_vector, version);
                    data_finished = true;
                }
            }

            // Store the data record.
            eos_finished = true;
        }
        else if(type == CRDRecordsTypeEnum::EOF_RECORD)
        {
            eof_finished = true;
        }

        // Update the read finished variable.
        switch (open_option)
        {
            // Update the finished flag.
            case OpenOptionEnum::ONLY_HEADER:
            read_finished = header_finished;
            break;
            case OpenOptionEnum::ONLY_HEADER_AND_CFG:
            read_finished = (header_finished && cfg_finished);
            break;
            case OpenOptionEnum::ALL_DATA:
            read_finished = (header_finished && cfg_finished && data_finished && eof_finished);
            break;
        }

        // Update the finished variable if the stream is empty.
        read_finished = read_finished || crd_stream.isEmpty() || eof_finished;
    }

    // Check if the stream is not empty.
    if(eof_finished && open_option == OpenOptionEnum::ALL_DATA && !crd_stream.isEmpty())
    {
        //Clear.
        this->clearCRDContents();

        // Get the next line for error storing.
        std::string line;
        std::vector<std::string> tokens;
        crd_stream.getline(line);
        dpslr::common::ConsolidatedRecordStruct rec;
        rec.line_number = crd_stream.getLineNumber();
        rec.consolidated_type = dpslr::common::ConsolidatedFileTypeEnum::UNKNOWN_TYPE;
        if(!line.empty())
        {
            dpslr::helpers::split(tokens, line, " ", false);
            rec.tokens = tokens;
        }

        // Store the last error struct.
        this->last_error_record_ = rec;

        error = ReadFileErrorEnum::CONTENT_AFTER_EOF;
    }
    // Check if the file is truncated.
    else if(!header_finished)
    {
        error = ReadFileErrorEnum::FILE_TRUNCATED;
    }
    else if(!cfg_finished && open_option >= OpenOptionEnum::ONLY_HEADER_AND_CFG)
    {
        error = ReadFileErrorEnum::FILE_TRUNCATED;
    }
    else if(!data_finished && open_option == OpenOptionEnum::ALL_DATA)
    {
        error = ReadFileErrorEnum::FILE_TRUNCATED;
    }
    else if(!eof_finished && open_option == OpenOptionEnum::ALL_DATA)
    {
        error = ReadFileErrorEnum::EOF_NOT_FOUND;
    }
    // Check if we have issues with the internal stored data.
    else if(this->read_header_errors.empty() && this->read_cfg_errors.empty() && this->read_data_errors.empty())
    {
        error = ReadFileErrorEnum::NOT_ERROR;
    }
    else if(!this->read_header_errors.empty() && this->read_cfg_errors.empty() && this->read_data_errors.empty())
    {
        error = ReadFileErrorEnum::HEADER_LOAD_WARNING;
    }
    else if(this->read_header_errors.empty() && !this->read_cfg_errors.empty() && this->read_data_errors.empty())
    {
        error = ReadFileErrorEnum::CFG_LOAD_WARNING;
    }
    else if(this->read_header_errors.empty() && this->read_cfg_errors.empty() && !this->read_data_errors.empty())
    {
        error = ReadFileErrorEnum::DATA_LOAD_WARNING;
    }
    else
    {
        error = ReadFileErrorEnum::RECORDS_LOAD_WARNING;
    }

    // Clear the CRD if neccesary.
    if(error > ReadFileErrorEnum::DATA_LOAD_WARNING)
        this->clearCRDContents();
    else
        this->empty_ = false;

    this->last_read_error_ = error;

    // Return the error.
    return error;
}

CRD::ReadFileErrorEnum CRD::openCRDData()
{
    return this->openCRDFile(this->crd_fullpath, CRD::OpenOptionEnum::ALL_DATA);
}

CRD::WriteFileErrorEnum CRD::writeCRDFile(const std::string &crd_filepath, CRDData::DataGenerationOptionEnum data_opt,
                                         bool force)
{
    std::ifstream input_file(crd_filepath);
    if (input_file.good() && !force)
        return CRD::WriteFileErrorEnum::FILE_ALREADY_EXIST;

    if (!this->header.formatHeader())
        return CRD::WriteFileErrorEnum::VERSION_UNKNOWN;

    // Close the previous.
    input_file.close();

    // Open the file.
    std::ofstream output_file(crd_filepath);

    // Store the records.
    output_file << this->header.generateHeaderLines(true) << std::endl
                << this->configuration.generateConfigurationLines(this->header.formatHeader()->crd_version) << std::endl
                << this->data.generateDataLines(this->header.formatHeader()->crd_version, data_opt) << std::endl
                << this->EndRecordsString[static_cast<int>(CRDRecordsTypeEnum::EOS_RECORD)] << std::endl
                << this->EndRecordsString[static_cast<int>(CRDRecordsTypeEnum::EOF_RECORD)];

    // Close the file.
    output_file.close();

    // Return no error.
    return CRD::WriteFileErrorEnum::NOT_ERROR;
}

CRD::ReadRecordResultEnum CRD::readRecord(dpslr::helpers::InputFileStream& stream,
                                          dpslr::common::ConsolidatedRecordStruct &rec)
{
    // Clear the record.
    rec.clearAll();

    // Check if the file is open.
    if(!stream.is_open())
        return ReadRecordResultEnum::STREAM_NOT_OPEN;

    // Check if the stream is empty.
    if(stream.isEmpty())
        return ReadRecordResultEnum::STREAM_EMPTY;

    // Aux containers and variables.
    constexpr int comment_enum_pos = static_cast<int>(
                dpslr::common::ConsolidatedRecordStruct::CommonRecords::COMMENT_RECORD);

    std::vector<std::string> tokens;
    std::string line;
    bool record_finished = false;

    // Get the record.
    while(!record_finished && stream.getline(line))
    {
        // Always store the line number.
        rec.line_number = stream.getLineNumber();
        rec.consolidated_type = dpslr::common::ConsolidatedFileTypeEnum::UNKNOWN_TYPE;

        // Check if the line is empty.
        if(!line.empty())
        {
            // Get the line and split it to get the tokens.
            dpslr::helpers::split(tokens, line, " ", false);
            tokens[0] = dpslr::helpers::toUpper(tokens[0]);

            // Check the EOS case.
            if(tokens[0] == EndRecordsString[static_cast<int>(CRDRecordsTypeEnum::EOS_RECORD)])
            {
                rec.consolidated_type = dpslr::common::ConsolidatedFileTypeEnum::CRD_TYPE;
                rec.tokens = tokens;
                rec.generic_record_type = static_cast<int>(CRDRecordsTypeEnum::EOS_RECORD);
                record_finished = true;
            }
            // Check the EOF case.
            else if(tokens[0] == EndRecordsString[static_cast<int>(CRDRecordsTypeEnum::EOF_RECORD)])
            {
                rec.consolidated_type = dpslr::common::ConsolidatedFileTypeEnum::CRD_TYPE;
                rec.tokens = tokens;
                rec.generic_record_type = static_cast<int>(CRDRecordsTypeEnum::EOF_RECORD);
                record_finished = true;
            }
            // Check the comment case.
            else if(tokens[0] == dpslr::common::ConsolidatedRecordStruct::CommonRecordsString[comment_enum_pos])
            {
                // Check the size for empty comments.
               rec.comment_block.push_back((tokens.size() >= 2 ? line.substr(3) : ""));
            }
            // Check the other records case.
            else
            {
                // Find the token id in the containers.
                if(dpslr::helpers::contains(CRDHeader::HeaderLineString, tokens[0]))
                {
                    rec.consolidated_type = dpslr::common::ConsolidatedFileTypeEnum::CRD_TYPE;
                    rec.tokens = tokens;
                    rec.generic_record_type = static_cast<int>(CRDRecordsTypeEnum::HEADER_RECORD);
                    record_finished = true;
                }
                else if(dpslr::helpers::contains(CRDConfiguration::CfgLineString, tokens[0]))
                {
                    rec.consolidated_type = dpslr::common::ConsolidatedFileTypeEnum::CRD_TYPE;
                    rec.tokens = tokens;
                    rec.line_number = stream.getLineNumber();
                    rec.generic_record_type = static_cast<int>(CRDRecordsTypeEnum::CFG_RECORD);
                    record_finished = true;
                }
                else if(dpslr::helpers::contains(CRDData::DataLineString, tokens[0]))
                {
                    rec.consolidated_type = dpslr::common::ConsolidatedFileTypeEnum::CRD_TYPE;
                    rec.tokens = tokens;
                    rec.line_number = stream.getLineNumber();
                    rec.generic_record_type = static_cast<int>(CRDRecordsTypeEnum::DATA_RECORD);
                    record_finished = true;
                }
                else
                {
                    // Store the tokens (for external checking) and return error.
                    rec.tokens = tokens;
                    return ReadRecordResultEnum::UNDEFINED_RECORD;
                }
            } // End check record cases.
        } // End skip empty lines.
    } // End record generation.

    // Check if we have the record finished.
    if(!record_finished)
        return ReadRecordResultEnum::UNDEFINED_RECORD;

    // Return no error.
    return ReadRecordResultEnum::NOT_ERROR;
}

// ---------------------------------------------------------------------------------------------------------------------

// =====================================================================================================================
