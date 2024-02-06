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
 * @file cpf.cpp
 * @author Degoras Project Team.
 * @brief This file contains the implementation of the CPF class.
 * @copyright EUPL License
 * @version 2305.1
***********************************************************************************************************************/

// C++ INCLUDES
//======================================================================================================================
#include <array>
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include <LibDegorasSLR/FormatsILRS/cpf/cpf.h>
#include <LibDegorasSLR/FormatsILRS/common/consolidated_types.h>
#include <LibDegorasSLR/FormatsILRS/common/consolidated_record.h>
#include <LibDegorasSLR/Timing/time_utils.h>
#include <LibDegorasSLR/SpaceObject/spaceobject_utils.h>
#include <LibDegorasSLR/Helpers/Helpers>
// =====================================================================================================================

// =====================================================================================================================
using namespace dpslr::ilrs::common;
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace ilrs{
namespace cpf{
// =====================================================================================================================

CPF::CPF(float version) :
    empty_(false)
{
    // Set the version and creation time at Format Header
    this->header_.basicInfo1Header()->cpf_version = version;
    this->header_.basicInfo1Header()->cpf_production_date = timing::HRTimePointStd::clock::now();
}

CPF::CPF(const std::string &cpf_filepath, OpenOptionEnum option)
{
    this->openCPFFile(cpf_filepath, option);
}

void CPF::clearCPF()
{
    // Clear the CPF contents and leave it empty
    this->clearCPFContents();

    // Clear the error storage.
    this->last_read_error_ = ReadFileErrorEnum::NOT_ERROR;
    this->last_read_error_ = {};
    this->read_header_errors_.clear();
    this->read_data_errors_.clear();

    // Clear others.
    this->cpf_filename_.clear();
    this->cpf_fullpath_.clear();
}

void CPF::clearCPFContents()
{
    // Clear header and data and leave CPF empty
    this->header_.clearAll();
    this->data_.clearAll();
    this->empty_ = true;
}

void CPF::clearCPFHeader(){this->header_.clearAll();}

void CPF::clearCPFData(){this->data_.clearAll();}

CPFHeader &CPF::getHeader() {return this->header_;}

CPFData &CPF::getData() {return this->data_;}

const CPFHeader &CPF::getHeader() const {return this->header_;}

const CPFData &CPF::getData() const {return this->data_;}

const RecordReadErrorMultimap &CPF::getReadHeaderErrors() const {return this->read_header_errors_;}

const RecordReadErrorMultimap &CPF::getReadDataErrors() const {return this->read_data_errors_;}

CPF::ReadFileErrorEnum CPF::getLastReadError() const {return this->last_read_error_;}

const Optional<ConsolidatedRecord> &CPF::getLastReadErrorRecord() const {return this->last_error_record_;}

const std::string &CPF::getSourceFilename() const {return this->cpf_filename_;}

const std::string &CPF::getSourceFilepath() const {return this->cpf_fullpath_;}

void CPF::getAvailableTimeWindow(timing::common::MJDate &mjd_start, timing::common::SoD &secs_start,
                                 timing::common::MJDate &mjd_end, timing::common::SoD &secs_end) const
{
    if (this->empty_)
    {
        mjd_start = 0LL;
        secs_start = 0.L;
        mjd_end = 0LL;
        secs_end = 0.L;
    }
    else
    {
        mjd_start = this->getData().positionRecords().front().mjd;
        secs_start = this->getData().positionRecords().front().sod;
        mjd_end = this->getData().positionRecords().back().mjd;
        secs_end = this->getData().positionRecords().back().sod;
    }
}

math::Interval<long double> CPF::getAvailableTimeInterval() const
{
    // Default empty interval.
    math::Interval<long double> interval;
    // Include min and max.
    interval.setIncludeMin(true);
    interval.setIncludeMax(true);
    // Check the data.
    if (!this->empty_)
    {
        // Get the start time.
        timing::MJDateTime mjdt_start = timing::mjdAndSecsToMjdt(this->getData().positionRecords().front().mjd,
                                                          this->getData().positionRecords().front().sod);
        // Get the stop time.
        timing::MJDateTime mjdt_stop = timing::mjdAndSecsToMjdt(this->getData().positionRecords().back().mjd,
                                                         this->getData().positionRecords().back().sod);
        // Update the interval.
        interval.setMin(mjdt_start);
        interval.setMax(mjdt_stop);
    }
    // Return the interval.
    return interval;
}

std::string CPF::getStandardFilename(TargetIdOptionEnum option) const
{
    // Variables.
    std::string filename;

    // Check the preconditions.
    if(!this->header_.basicInfo1Header() || !this->header_.basicInfo2Header())
        return "";

    // Get the target identifier.
    switch (option)
    {
    case TargetIdOptionEnum::ILRS_ID:
        filename.append(this->header_.basicInfo2Header()->id);
        break;

    case TargetIdOptionEnum::SHORT_COSPAR:
        filename.append(spobj::ilrsidToShortcospar(this->header_.basicInfo2Header()->id));
        break;

    case TargetIdOptionEnum::COSPAR:
        filename.append(spobj::ilrsidToCospar(this->header_.basicInfo2Header()->id));
        break;

    case TargetIdOptionEnum::NORAD:
        filename.append(this->header_.basicInfo2Header()->norad);
        break;

    case TargetIdOptionEnum::TARGET_NAME:
        filename.append(helpers::strings::toLower(this->header_.basicInfo1Header()->target_name));
        break;
    }

    // Append consoldiated separator.
    filename.append("_cpf_");

    // Append the starting date of the pass from H4.
    std::time_t time = timing::HRTimePointStd::clock::to_time_t(this->header_.basicInfo2Header()->start_time);
    std::tm* tm = gmtime(&time);
    char start_date[7];
    std::strftime(start_date, 7, "%y%m%d", tm);
    filename.append(start_date);

    // Append the cpf version and source as file format.
    std::stringstream rr;

    if(this->header_.basicInfo1Header()->cpf_version >= 1 && this->header_.basicInfo1Header()->cpf_version < 2)
    {
        rr << std::setw(4) << std::setfill('0') << this->header_.basicInfo1Header()->cpf_sequence_number;
    }
    else if(this->header_.basicInfo1Header()->cpf_version >= 2 && this->header_.basicInfo1Header()->cpf_version < 3)
    {
        rr << std::setw(3) << std::setfill('0') << this->header_.basicInfo1Header()->cpf_sequence_number;
        rr << std::setw(2) << this->header_.basicInfo1Header()->cpf_subsequence_number;
    }
    filename.append('_' + rr.str() + '.' + this->header_.basicInfo1Header()->cpf_source);

    // Return the convention filename.
    return filename;
}

bool CPF::isEmpty() const {return this->empty_;}

bool CPF::hasData() const
{
    return (!this->isEmpty() && !this->getData().positionRecords().empty());
}

CPF::ReadFileErrorEnum CPF::openCPFFile(const std::string &cpf_filepath, CPF::OpenOptionEnum open_option)
{
    // Variables.
    ReadFileErrorEnum error = ReadFileErrorEnum::NOT_ERROR;
    RecordLinesVector data_vector;
    RecordLinesVector cfg_vector;
    RecordLinesVector header_vector;
    float version = 1.;
    bool header_finished = false;
    bool data_finished = false;
    bool read_finished = false;

    // Open the file using our custom input file stream.
    helpers::files::InputFileStream cpf_stream(cpf_filepath);

    // Clear the CPF.
    this->clearCPF();

    // Check if the stream is open.
    if(!cpf_stream.is_open())
    {
        this->last_read_error_ = ReadFileErrorEnum::FILE_NOT_FOUND;
        return ReadFileErrorEnum::FILE_NOT_FOUND;
    }

    // Check if the stream is empty.
    if(cpf_stream.isEmpty())
    {
        this->last_read_error_ = ReadFileErrorEnum::FILE_EMPTY;
        return ReadFileErrorEnum::FILE_EMPTY;
    }

    // Store the file path and name.
    this->cpf_fullpath_ = cpf_filepath;
    this->cpf_filename_ = helpers::strings::split<StringV>(cpf_filepath, "/").back();

    // Open the header.
    while (!read_finished)
    {
        // Auxiliar variables.
        ConsolidatedRecord record;
        CPF::ReadRecordResultEnum read_result;

        // Get the next record.
        read_result = this->readRecord(cpf_stream, record);

        // Get the type.
        CPFRecordsType type = static_cast<CPFRecordsType>(record.generic_record_type);

        // Check the errors.
        if(read_result == ReadRecordResultEnum::UNDEFINED_RECORD)
        {
            this->clearCPFContents();
            this->last_read_error_ = ReadFileErrorEnum::UNDEFINED_RECORD;
            this->last_error_record_ = record;
            return ReadFileErrorEnum::UNDEFINED_RECORD;
        }
        else if(type == CPFRecordsType::HEADER_RECORD && header_finished)
        {
            this->clearCPFContents();
            this->last_read_error_ = ReadFileErrorEnum::ORDER_ERROR;
            this->last_error_record_ = record;
            return ReadFileErrorEnum::ORDER_ERROR;
        }
        else if(type == CPFRecordsType::DATA_RECORD && (!header_finished || data_finished))
        {
            this->clearCPFContents();
            this->last_read_error_ = ReadFileErrorEnum::ORDER_ERROR;
            this->last_error_record_ = record;
            return ReadFileErrorEnum::ORDER_ERROR;
        }
        else if(type == CPFRecordsType::EOH_RECORD && header_finished)
        {
            this->clearCPFContents();
            this->last_read_error_ = ReadFileErrorEnum::MULTIPLE_EOH;
            this->last_error_record_ = record;
            return ReadFileErrorEnum::MULTIPLE_EOH;
        }

        // Check the record type.
        if(type == CPFRecordsType::HEADER_RECORD)
        {
            header_vector.push_back(record);
        }
        else if (type == CPFRecordsType::EOH_RECORD)
        {
            // Check if we hava header records.
            if(!header_vector.empty())
            {
                // Save the possible issues.
                this->read_header_errors_ = this->header_.readHeader(header_vector);
                header_finished = true;

                // Get the version:
                if(this->header_.basicInfo1Header())
                    version = this->header_.basicInfo1Header()->cpf_version;
                else
                {
                    this->clearCPFContents();
                    this->last_read_error_ = ReadFileErrorEnum::VERSION_UNKNOWN;
                    return ReadFileErrorEnum::VERSION_UNKNOWN;
                }
            }
            else
            {
                this->clearCPFContents();
                this->last_read_error_ = ReadFileErrorEnum::NO_HEADER_FOUND;
                return ReadFileErrorEnum::NO_HEADER_FOUND;
            }
        }
        else if(type == CPFRecordsType::DATA_RECORD)
        {
            // Store the data record.
            data_vector.push_back(record);
        }
        else if(type == CPFRecordsType::EOE_RECORD)
        {
            // Check if we hava header records.
            if(data_vector.empty())
            {
                this->clearCPFContents();
                this->last_read_error_ = ReadFileErrorEnum::NO_DATA_FOUND;
                return ReadFileErrorEnum::NO_DATA_FOUND;
            }
            else if(!header_finished)
            {
                this->clearCPFContents();
                this->last_read_error_ = ReadFileErrorEnum::FILE_TRUNCATED;
                return ReadFileErrorEnum::FILE_TRUNCATED;
            }
            else
            {
                // Save the possible issues.
                this->read_data_errors_ = this->data_.readData(data_vector, version);
                data_finished = true;
            }

        }

        // Update the read finished variable.
        switch (open_option)
        {
            // Update the finished flag.
            case OpenOptionEnum::ONLY_HEADER:
            read_finished = header_finished;
            break;
            case OpenOptionEnum::ALL_DATA:
            read_finished = (header_finished && data_finished);
            break;
        }

        // Update the finished variable if the stream is empty.
        read_finished =  read_finished || cpf_stream.isEmpty();
    }

    // Check if the stream is not empty.
    if(data_finished && open_option == OpenOptionEnum::ALL_DATA && !cpf_stream.isEmpty())
    {
        //Clear.
        this->clearCPFContents();

        // Get the next line for error storing.
        std::string line;
        std::vector<std::string> tokens;
        cpf_stream.getline(line);
        ConsolidatedRecord rec;
        rec.line_number = cpf_stream.getLineNumber();
        rec.consolidated_type = ConsolidatedFileType::UNKNOWN_TYPE;
        if(!line.empty())
        {
            helpers::strings::split(tokens, line, " ", false);
            rec.tokens = tokens;
        }

        // Store the last error struct.
        this->last_error_record_ = rec;

        error = ReadFileErrorEnum::CONTENT_AFTER_EOE;
    }
    // Check if the file is truncated.
    else if(!header_finished)
    {
        error = ReadFileErrorEnum::FILE_TRUNCATED;
    }
    else if(!data_finished && open_option == OpenOptionEnum::ALL_DATA)
    {
        error = ReadFileErrorEnum::EOE_NOT_FOUND;
    }
    // Check if we have issues with the internal stored data.
    else if(this->read_header_errors_.empty() && this->read_data_errors_.empty())
    {
        error = ReadFileErrorEnum::NOT_ERROR;
    }
    else if(!this->read_header_errors_.empty() && this->read_data_errors_.empty())
    {
        error = ReadFileErrorEnum::HEADER_LOAD_WARNING;
    }
    else if(this->read_header_errors_.empty() && !this->read_data_errors_.empty())
    {
        error = ReadFileErrorEnum::DATA_LOAD_WARNING;
    }
    else
    {
        error = ReadFileErrorEnum::RECORDS_LOAD_WARNING;
    }

    // Clear the CPF if neccesary.
    if(error > ReadFileErrorEnum::DATA_LOAD_WARNING)
        this->clearCPFContents();
    else
        this->empty_ = false;

    this->last_read_error_ = error;

    // Return the error.
    return error;
}

CPF::ReadFileErrorEnum CPF::openCPFData()
{
    // Send path as copy to prevent clearing the path
    return this->openCPFFile(std::string(this->cpf_fullpath_), CPF::OpenOptionEnum::ALL_DATA);
}

CPF::WriteFileErrorEnum CPF::writeCPFFile(const std::string &cpf_filepath, bool force)
{
    std::ifstream input_file(cpf_filepath);
    if (input_file.good() && !force)
        return CPF::WriteFileErrorEnum::FILE_ALREADY_EXIST;

    if (!this->header_.basicInfo1Header())
        return CPF::WriteFileErrorEnum::VERSION_UNKNOWN;

    // Close the previous.
    input_file.close();

    // Open the file.
    std::ofstream output_file(cpf_filepath);

    // Store the records.
    output_file << this->header_.generateHeaderLines() << std::endl
                << EndIdStr[static_cast<int>(CPFRecordsType::EOH_RECORD)] << std::endl
                << this->data_.generateDataLines(this->header_.basicInfo1Header()->cpf_version) << std::endl
                << EndIdStr[static_cast<int>(CPFRecordsType::EOE_RECORD)];

    // Close the file.
    output_file.close();

    // Return no error.
    return CPF::WriteFileErrorEnum::NOT_ERROR;
}

CPF::ReadRecordResultEnum CPF::readRecord(helpers::files::InputFileStream& stream, ConsolidatedRecord& rec)
{
    // Clear the record.
    rec.clearAll();

    // Check if the file is open.
    if(!stream.is_open())
        return ReadRecordResultEnum::STREAM_NOT_OPEN;

    // Check if the stream is empty.
    if(stream.isEmpty())
        return ReadRecordResultEnum::STREAM_EMPTY;



    std::vector<std::string> tokens;
    std::string line;
    bool record_finished = false;

    // Get the record.
    while(!record_finished && stream.getline(line))
    {
        // Always store the line number.
        rec.line_number = stream.getLineNumber();
        rec.consolidated_type = ConsolidatedFileType::UNKNOWN_TYPE;

        // Check if the line is empty.
        if(!line.empty())
        {
            // Get the line and split it to get the tokens.
            helpers::strings::split(tokens, line, " ", false);
            tokens[0] = helpers::strings::toUpper(tokens[0]);

            // Check the EOH case (H9).
            if(tokens[0] == EndIdStr[static_cast<int>(CPFRecordsType::EOH_RECORD)])
            {
                rec.consolidated_type = ConsolidatedFileType::CPF_TYPE;
                rec.tokens = tokens;
                rec.generic_record_type = static_cast<int>(CPFRecordsType::EOH_RECORD);
                record_finished = true;
            }
            // Check the EOE case (99).
            else if(tokens[0] == EndIdStr[static_cast<int>(CPFRecordsType::EOE_RECORD)])
            {
                rec.consolidated_type = ConsolidatedFileType::CPF_TYPE;
                rec.tokens = tokens;
                rec.generic_record_type = static_cast<int>(CPFRecordsType::EOE_RECORD);
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
                    rec.consolidated_type = ConsolidatedFileType::CPF_TYPE;
                    rec.tokens = tokens;
                    rec.generic_record_type = static_cast<int>(CPFRecordsType::HEADER_RECORD);
                    record_finished = true;
                }
               else if(helpers::containers::contains(DataIdStr, tokens[0]))
                {
                    rec.consolidated_type = ConsolidatedFileType::CPF_TYPE;
                    rec.tokens = tokens;
                    rec.line_number = stream.getLineNumber();
                    rec.generic_record_type = static_cast<int>(CPFRecordsType::DATA_RECORD);
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

}}} // END NAMESPACES.
// =====================================================================================================================
