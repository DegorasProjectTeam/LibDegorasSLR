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
 * @file crd.cpp
 * @author Degoras Project Team.
 * @brief This file contains the implementation of the CRD class.
 * @copyright EUPL License
 * @version 2306.1
***********************************************************************************************************************/

// C++ INCLUDES
//======================================================================================================================
#include <array>
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/FormatsILRS/crd/crd.h"
#include "LibDegorasSLR/FormatsILRS/common/consolidated_types.h"
#include "LibDegorasSLR/FormatsILRS/common/consolidated_record.h"
#include "LibDegorasSLR/Astronomical/spaceobject_utils.h"
#include "LibDegorasSLR/Helpers/filedir_helpers.h"
#include "LibDegorasSLR/Helpers/string_helpers.h"
#include "LibDegorasSLR/Helpers/container_helpers.h"
// =====================================================================================================================

// =====================================================================================================================
using namespace dpslr::ilrs::common;
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace ilrs{
namespace crd{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using namespace timing::types;
// ---------------------------------------------------------------------------------------------------------------------

CRD::CRD(float version):
    empty_(false)
{
    // Set the version and creation time at Format Header
    this->header.formatHeader()->crd_version = version;
    this->header.formatHeader()->crd_production_date = HRTimePointStd::clock::now();
}

CRD::CRD(const std::string &crd_filepath, OpenOption option)
{
    this->openCRDFile(crd_filepath, option);
}

void CRD::clearCRD()
{
    // Clear the contents.
    this->clearCRDContents();

    // Clear the error storage.
    this->last_read_error_ = ReadFileError::NOT_ERROR;
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

CRDHeader &CRD::getHeader() {return this->header;}

CRDConfiguration &CRD::getConfiguration() {return this->configuration;}

CRDData &CRD::getData() {return this->data;}

const CRDHeader &CRD::getHeader() const {return this->header;}

const CRDConfiguration &CRD::getConfiguration() const {return this->configuration;}

const CRDData &CRD::getData() const {return this->data;}

const RecordReadErrorMultimap &CRD::getReadHeaderErrors() const {return this->read_header_errors;}

const RecordReadErrorMultimap &CRD::getReadCfgErrors() const {return this->read_cfg_errors;}

const RecordReadErrorMultimap &CRD::getReadDataErrors() const {return this->read_data_errors;}

CRD::ReadFileError CRD::getLastReadError() const {return this->last_read_error_;}

const Optional<ConsolidatedRecord> &CRD::getLastReadErrorRecord() const {return this->last_error_record_;}

const std::string &CRD::getSourceFilename() const {return this->crd_filename;}

const std::string &CRD::getSourceFilepath() const {return this->crd_fullpath;}

std::string CRD::getStandardFilename(TargetIdOption option) const
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
            filename.append(helpers::strings::toLower(this->header.stationHeader()->network) + '_');
    }

    // Get the CDP pad identifier for station
    filename.append(std::to_string(this->header.stationHeader()->cdp_pad_identifier) + '_');

    // Get the target identifier.
    switch (option)
    {
        case TargetIdOption::ILRS_ID:
        filename.append(this->header.targetHeader()->ilrsid + "_crd_");
        break;

        case TargetIdOption::SHORT_COSPAR:
        filename.append(spobj::ilrsidToShortcospar(this->header.targetHeader()->ilrsid) + "_crd_");
        break;

        case TargetIdOption::COSPAR:
        filename.append(spobj::ilrsidToCospar(this->header.targetHeader()->ilrsid) + "_crd_");
        break;

        case TargetIdOption::NORAD:
        filename.append(this->header.targetHeader()->norad + "_crd_");
        break;

        case TargetIdOption::TARGET_NAME:
        filename.append(helpers::strings::toLower(this->header.targetHeader()->name) + "_crd_");
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
        filename.append(CRD::ExtensionsStr[static_cast<int>(this->header.sessionHeader()->data_type)]);
    else if(this->header.formatHeader()->crd_version >= 2 && this->header.formatHeader()->crd_version < 3)
        filename.append(CRD::ExtensionsStr[static_cast<int>(this->header.sessionHeader()->data_type)+2]);

    // Return the convention filename.
    return filename;
}
// ---------------------------------------------------------------------------------------------------------------------

// --- CRD OBSERVERS ---------------------------------------------------------------------------------------------------
bool CRD::empty() const {return this->empty_;}
// ---------------------------------------------------------------------------------------------------------------------

// --- CRD FILE METHODS ------------------------------------------------------------------------------------------------
CRD::ReadFileError CRD::openCRDFile(const std::string &crd_filepath, CRD::OpenOption open_option)
{
    // Variables.
    ReadFileError error = ReadFileError::NOT_ERROR;
    RecordLinesVector data_vector;
    RecordLinesVector cfg_vector;
    RecordLinesVector header_vector;
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
    helpers::files::DegorasInputFileStream crd_stream(crd_filepath);

    // Check if the stream is open.
    if(!crd_stream.is_open())
    {
        this->last_read_error_ = ReadFileError::FILE_NOT_FOUND;
        return ReadFileError::FILE_NOT_FOUND;
    }

    // Check if the stream is empty.
    if(crd_stream.isEmpty())
    {
        this->last_read_error_ = ReadFileError::FILE_EMPTY;
        return ReadFileError::FILE_EMPTY;
    }

    // Store the file path and name.
    this->crd_fullpath = crd_filepath;
    this->crd_filename = helpers::strings::split<std::vector<std::string>>(crd_filepath, "/").back();

    // Open the header.
    while (!read_finished)
    {
        // Auxiliar variables.
        ConsolidatedRecord record;
        CRD::ReadRecordResult read_result;

        // Get the next record.
        read_result = this->readRecord(crd_stream, record);

        // Get the type.
        CRDRecordsType type = static_cast<CRDRecordsType>(record.generic_record_type);

        // Check the errors.
        if(read_result == ReadRecordResult::UNDEFINED_RECORD)
        {
            this->clearCRDContents();
            this->last_read_error_ = ReadFileError::UNDEFINED_RECORD;
            this->last_error_record_ = record;
            return ReadFileError::UNDEFINED_RECORD;
        }
        else if(type == CRDRecordsType::HEADER_RECORD && header_finished)
        {
            this->clearCRDContents();
            this->last_read_error_ = ReadFileError::ORDER_ERROR;
            this->last_error_record_ = record;
            return ReadFileError::ORDER_ERROR;
        }
        else if(type == CRDRecordsType::CFG_RECORD &&  cfg_finished)
        {
            this->clearCRDContents();
            this->last_read_error_ = ReadFileError::ORDER_ERROR;
            this->last_error_record_ = record;
            return ReadFileError::ORDER_ERROR;
        }
        else if(type == CRDRecordsType::DATA_RECORD && data_finished)
        {
            this->clearCRDContents();
            this->last_read_error_ = ReadFileError::ORDER_ERROR;
            this->last_error_record_ = record;
            return ReadFileError::ORDER_ERROR;
        }
        else if(type == CRDRecordsType::EOS_RECORD && eos_finished)
        {
            this->clearCRDContents();
            this->last_read_error_ = ReadFileError::MULTIPLE_EOS;
            this->last_error_record_ = record;
            return ReadFileError::MULTIPLE_EOS;
        }
        else if(type == CRDRecordsType::EOF_RECORD && !eos_finished)
        {
            this->clearCRDContents();
            this->last_read_error_ = ReadFileError::EOS_NOT_FOUND;
            return ReadFileError::EOS_NOT_FOUND;
        }

        // Check the record type.
        if(type == CRDRecordsType::HEADER_RECORD)
        {
             header_vector.push_back(record);
        }
        else if(type == CRDRecordsType::CFG_RECORD)
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
                        this->last_read_error_ = ReadFileError::VERSION_UNKNOWN;
                        return ReadFileError::VERSION_UNKNOWN;
                    }
                }
                else
                {
                    this->clearCRDContents();
                    this->last_read_error_ = ReadFileError::NO_HEADER_FOUND;
                    return ReadFileError::NO_HEADER_FOUND;
                }
            }

            // Store the configuration record.
            cfg_vector.push_back(record);
        }
        else if(type == CRDRecordsType::DATA_RECORD)
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
                    this->last_read_error_ = ReadFileError::NO_CFG_FOUND;
                    return ReadFileError::NO_CFG_FOUND;
                }
            }

            // Store the data record.
            data_vector.push_back(record);
        }
        else if(type == CRDRecordsType::EOS_RECORD)
        {
            // Check if we send the header data to the internal structs.
            if(!data_finished)
            {
                // Check if we hava header records.
                if(data_vector.empty())
                {
                    this->clearCRDContents();
                    this->last_read_error_ = ReadFileError::NO_DATA_FOUND;
                    return ReadFileError::NO_DATA_FOUND;
                }
                else if(!header_finished || !cfg_finished)
                {
                    this->clearCRDContents();
                    this->last_read_error_ = ReadFileError::FILE_TRUNCATED;
                    return ReadFileError::FILE_TRUNCATED;
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
        else if(type == CRDRecordsType::EOF_RECORD)
        {
            eof_finished = true;
        }

        // Update the read finished variable.
        switch (open_option)
        {
            // Update the finished flag.
            case OpenOption::ONLY_HEADER:
            read_finished = header_finished;
            break;
            case OpenOption::ONLY_HEADER_AND_CFG:
            read_finished = (header_finished && cfg_finished);
            break;
            case OpenOption::ALL_DATA:
            read_finished = (header_finished && cfg_finished && data_finished && eof_finished);
            break;
        }

        // Update the finished variable if the stream is empty.
        read_finished = read_finished || crd_stream.isEmpty() || eof_finished;
    }

    // Check if the stream is not empty.
    if(eof_finished && open_option == OpenOption::ALL_DATA && !crd_stream.isEmpty())
    {
        //Clear.
        this->clearCRDContents();

        // Get the next line for error storing.
        std::string line;
        std::vector<std::string> tokens;
        crd_stream.getline(line);
        ConsolidatedRecord rec;
        rec.line_number = crd_stream.getCurrentLineNumber();
        rec.consolidated_type = ConsolidatedFileType::UNKNOWN_TYPE;
        if(!line.empty())
        {
            helpers::strings::split(tokens, line, " ", false);
            rec.tokens = tokens;
        }

        // Store the last error struct.
        this->last_error_record_ = rec;

        error = ReadFileError::CONTENT_AFTER_EOF;
    }
    // Check if the file is truncated.
    else if(!header_finished)
    {
        error = ReadFileError::FILE_TRUNCATED;
    }
    else if(!cfg_finished && open_option >= OpenOption::ONLY_HEADER_AND_CFG)
    {
        error = ReadFileError::FILE_TRUNCATED;
    }
    else if(!data_finished && open_option == OpenOption::ALL_DATA)
    {
        error = ReadFileError::FILE_TRUNCATED;
    }
    else if(!eof_finished && open_option == OpenOption::ALL_DATA)
    {
        error = ReadFileError::EOF_NOT_FOUND;
    }
    // Check if we have issues with the internal stored data.
    else if(this->read_header_errors.empty() && this->read_cfg_errors.empty() && this->read_data_errors.empty())
    {
        error = ReadFileError::NOT_ERROR;
    }
    else if(!this->read_header_errors.empty() && this->read_cfg_errors.empty() && this->read_data_errors.empty())
    {
        error = ReadFileError::HEADER_LOAD_WARNING;
    }
    else if(this->read_header_errors.empty() && !this->read_cfg_errors.empty() && this->read_data_errors.empty())
    {
        error = ReadFileError::CFG_LOAD_WARNING;
    }
    else if(this->read_header_errors.empty() && this->read_cfg_errors.empty() && !this->read_data_errors.empty())
    {
        error = ReadFileError::DATA_LOAD_WARNING;
    }
    else
    {
        error = ReadFileError::RECORDS_LOAD_WARNING;
    }

    // Clear the CRD if neccesary.
    if(error > ReadFileError::DATA_LOAD_WARNING)
        this->clearCRDContents();
    else
        this->empty_ = false;

    this->last_read_error_ = error;

    // Return the error.
    return error;
}

CRD::ReadFileError CRD::openCRDData()
{
    return this->openCRDFile(this->crd_fullpath, CRD::OpenOption::ALL_DATA);
}

CRD::WriteFileError CRD::writeCRDFile(const std::string& crd_filepath, CRDData::DataGenerationOption opt, bool force)
{
    std::ifstream input_file(crd_filepath);
    if (input_file.good() && !force)
        return CRD::WriteFileError::FILE_ALREADY_EXIST;

    if (!this->header.formatHeader())
        return CRD::WriteFileError::VERSION_UNKNOWN;

    // Close the previous.
    input_file.close();

    // Open the file.
    std::ofstream output_file(crd_filepath);

    // Store the records.
    output_file << this->header.generateHeaderLines(true) << std::endl
                << this->configuration.generateConfigurationLines(this->header.formatHeader()->crd_version) << std::endl
                << this->data.generateDataLines(this->header.formatHeader()->crd_version, opt) << std::endl
                << EndIdStr[static_cast<int>(CRDRecordsType::EOS_RECORD)] << std::endl
                << EndIdStr[static_cast<int>(CRDRecordsType::EOF_RECORD)];

    // Close the file.
    output_file.close();

    // Return no error.
    return CRD::WriteFileError::NOT_ERROR;
}

CRD::ReadRecordResult CRD::readRecord(helpers::files::DegorasInputFileStream& stream, ConsolidatedRecord &rec)
{
    // Clear the record.
    rec.clearAll();

    // Check if the file is open.
    if(!stream.is_open())
        return ReadRecordResult::STREAM_NOT_OPEN;

    // Check if the stream is empty.
    if(stream.isEmpty())
        return ReadRecordResult::STREAM_EMPTY;

    std::vector<std::string> tokens;
    std::string line;
    bool record_finished = false;

    // Get the record.
    while(!record_finished && stream.getline(line))
    {
        // Always store the line number.
        rec.line_number = stream.getCurrentLineNumber();
        rec.consolidated_type = ConsolidatedFileType::UNKNOWN_TYPE;

        // Check if the line is empty.
        if(!line.empty())
        {
            // Get the line and split it to get the tokens.
            helpers::strings::split(tokens, line, " ", false);
            tokens[0] = helpers::strings::toUpper(tokens[0]);

            // Check the EOS case.
            if(tokens[0] == EndIdStr[static_cast<int>(CRDRecordsType::EOS_RECORD)])
            {
                rec.consolidated_type = ConsolidatedFileType::CRD_TYPE;
                rec.tokens = tokens;
                rec.generic_record_type = static_cast<int>(CRDRecordsType::EOS_RECORD);
                record_finished = true;
            }
            // Check the EOF case.
            else if(tokens[0] == EndIdStr[static_cast<int>(CRDRecordsType::EOF_RECORD)])
            {
                rec.consolidated_type = ConsolidatedFileType::CRD_TYPE;
                rec.tokens = tokens;
                rec.generic_record_type = static_cast<int>(CRDRecordsType::EOF_RECORD);
                record_finished = true;
            }
            // Check the comment case.
            else if(tokens[0] == CommentIdStr)
            {
                // Check the size for empty comments.
               rec.comment_block.push_back((tokens.size() >= 2 ? line.substr(3) : ""));
            }
            // Check the other records case.
            else
            {
                // Find the token id in the containers.
                if(helpers::containers::contains(HeaderIdStr, tokens[0]))
                {
                    rec.consolidated_type = ConsolidatedFileType::CRD_TYPE;
                    rec.tokens = tokens;
                    rec.generic_record_type = static_cast<int>(CRDRecordsType::HEADER_RECORD);
                    record_finished = true;
                }
                else if(helpers::containers::contains(CfgIdStr, tokens[0]))
                {
                    rec.consolidated_type = ConsolidatedFileType::CRD_TYPE;
                    rec.tokens = tokens;
                    rec.line_number = stream.getCurrentLineNumber();
                    rec.generic_record_type = static_cast<int>(CRDRecordsType::CFG_RECORD);
                    record_finished = true;
                }
                else if(helpers::containers::contains(DataIdStr, tokens[0]))
                {
                    rec.consolidated_type = ConsolidatedFileType::CRD_TYPE;
                    rec.tokens = tokens;
                    rec.line_number = stream.getCurrentLineNumber();
                    rec.generic_record_type = static_cast<int>(CRDRecordsType::DATA_RECORD);
                    record_finished = true;
                }
                else
                {
                    // Store the tokens (for external checking) and return error.
                    rec.tokens = tokens;
                    return ReadRecordResult::UNDEFINED_RECORD;
                }
            } // End check record cases.
        } // End skip empty lines.
    } // End record generation.

    // Check if we have the record finished.
    if(!record_finished)
        return ReadRecordResult::UNDEFINED_RECORD;

    // Return no error.
    return ReadRecordResult::NOT_ERROR;
}

}}} // END NAMESPACES.
// =====================================================================================================================
