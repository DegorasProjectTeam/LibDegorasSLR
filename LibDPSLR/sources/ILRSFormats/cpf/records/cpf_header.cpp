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
 * @file cpf_header.cpp
 * @author Degoras Project Team.
 * @brief This file contains the implementation of the CRDHeader class that abstracts the header of ILRS CRD format.
 * @copyright EUPL License
 * @version 2305.1
***********************************************************************************************************************/

// C++ INCLUDES
//======================================================================================================================
#include <array>
#include <sstream>
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include <LibDPSLR/ILRSFormats/cpf/records/cpf_header.h>
#include <LibDPSLR/Helpers/container_helpers.h>
#include <LibDPSLR/Helpers/string_helpers.h>
#include <LibDPSLR/Timing/time_utils.h>
// =====================================================================================================================

// =====================================================================================================================
using namespace dpslr::ilrs::common;
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace ilrs{
namespace cpf{
// =====================================================================================================================

// --- CPF HEADER CONST EXPRESSIONS ------------------------------------------------------------------------------------
const std::array<unsigned, 2> CPFHeader::CPFVersions {1, 2};  // Add new main versions here.
// ---------------------------------------------------------------------------------------------------------------------

CPFHeader::CPFHeader(float cpf_version) :
    basic_info1_header(BasicInfo1Header())
{
    this->basic_info1_header->cpf_version = cpf_version;
}

void CPFHeader::clearAll()
{
    // Clear all records
    this->clearBasicInfo1Header();
    this->clearBasicInfo2Header();
    this->clearExpectedAccuracyHeader();
    this->clearTransponderInfoHeader();
    this->clearCoMCorrectionHeader();
}

void CPFHeader::clearBasicInfo1Header() {this->basic_info1_header = {};}

void CPFHeader::clearBasicInfo2Header() {this->basic_info2_header = {};}

void CPFHeader::clearExpectedAccuracyHeader() {this->exp_accuracy_header = {};}

void CPFHeader::clearTransponderInfoHeader() {this->transp_info_header = {};}

void CPFHeader::clearCoMCorrectionHeader() {this->com_corr_header = {};}

const Optional<CPFHeader::BasicInfo1Header> &CPFHeader::basicInfo1Header() const {return this->basic_info1_header;}

const Optional<CPFHeader::BasicInfo2Header> &CPFHeader::basicInfo2Header() const {return this->basic_info2_header;}

const Optional<CPFHeader::ExpectedAccuracyHeader> &CPFHeader::expectedAccuracyHeader() const {return this->exp_accuracy_header;}

const Optional<CPFHeader::TransponderInfoHeader> &CPFHeader::transponderInfoHeader() const{return this->transp_info_header;}

const Optional<CPFHeader::CoMCorrectionHeader> &CPFHeader::coMCorrectionHeader() const{return this->com_corr_header;}

Optional<CPFHeader::BasicInfo1Header> &CPFHeader::basicInfo1Header(){return this->basic_info1_header;}

Optional<CPFHeader::BasicInfo2Header> &CPFHeader::basicInfo2Header(){return this->basic_info2_header;}

Optional<CPFHeader::ExpectedAccuracyHeader> &CPFHeader::expectedAccuracyHeader(){return this->exp_accuracy_header;}

Optional<CPFHeader::TransponderInfoHeader> &CPFHeader::transponderInfoHeader(){return this->transp_info_header;}

Optional<CPFHeader::CoMCorrectionHeader> &CPFHeader::coMCorrectionHeader(){return this->com_corr_header;}

void CPFHeader::setBasicInfo1Header(const BasicInfo1Header &bi1h) {this->basic_info1_header = bi1h;}

void CPFHeader::setBasicInfo2Header(const BasicInfo2Header &bi2h) {this->basic_info2_header = bi2h;}

void CPFHeader::setExpectedAccuracyHeader(const ExpectedAccuracyHeader &eah) {this->exp_accuracy_header = eah;}

void CPFHeader::setTransponderInfoHeader(const TransponderInfoHeader &tih) {this->transp_info_header = tih;}

void CPFHeader::setCoMCorrection(const CoMCorrectionHeader &comh) {this->com_corr_header = comh;}

std::string CPFHeader::generateHeaderLines()
{
    // Header line
    std::stringstream header_line;

    // Lines are not generated if there is no basic info 1 header.
    if (!this->basic_info1_header)
        return "";

    // For v1 and 2.
    if(this->basic_info1_header->cpf_version >= 1 && this->basic_info1_header->cpf_version <=3)
    {
        // Basic info 1 header.
        if(this->basic_info1_header)
        {
            // Add the associated comment block.
            if(!this->basic_info1_header->comment_block.empty())
                header_line << this->basic_info1_header->generateCommentBlock() << std::endl;
            // Add the line.
            header_line << this->basic_info1_header->generateLine() << std::endl;
        }

        // Basic info 2 header.
        if(this->basic_info2_header)
        {
            // Add the associated comment block.
            if(!this->basic_info2_header->comment_block.empty())
                header_line << this->basic_info2_header->generateCommentBlock() << std::endl;
            // Add the line.
            header_line << this->basic_info2_header->generateLine(this->basic_info1_header->cpf_version) << std::endl;
        }

        // Expected accuracy header.
        if(this->exp_accuracy_header)
        {
            // Add the associated comment block.
            if(!this->exp_accuracy_header->comment_block.empty())
                header_line << this->exp_accuracy_header->generateCommentBlock() << std::endl;
            // Add the line.
            header_line << this->exp_accuracy_header->generateLine(this->basic_info1_header->cpf_version) << std::endl;
        }

        // Transponder info header.
        if(this->transp_info_header)
        {
            // Add the associated comment block.
            if(!this->transp_info_header->comment_block.empty())
                header_line << this->transp_info_header->generateCommentBlock() << std::endl;
            // Add the line.
            header_line << this->transp_info_header->generateLine(this->basic_info1_header->cpf_version) << std::endl;
        }

        // CoM correction info header.
        if(this->com_corr_header)
        {
            // Add the associated comment block.
            if(!this->com_corr_header->comment_block.empty())
                header_line << this->com_corr_header->generateCommentBlock() << std::endl;
            // Add the line.
            header_line << this->com_corr_header->generateLine(this->basic_info1_header->cpf_version) << std::endl;
        }
    }

    // Return all the lines.
    return header_line.str().substr(0, header_line.str().find_last_of('\n'));
}

// Generic reading header function.
RecordReadErrorMultimap CPFHeader::readHeader(const RecordLinesVector &rec_v)
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
        if(helpers::containers::find(HeaderIdStr, rec.getIdToken(), pos))
        {
            // Store the record type in a pair.
            rec_pair = {pos, rec};

            // Read the record to store the data in memory.
            error = this->readHeaderLine(rec_pair);

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

// Specific function for reading H1.
RecordReadError CPFHeader::readBasicInfo1Header(const ConsolidatedRecord& record)
{
    // Tokens: H1, CPF, VERSION, SOURCE, YEAR, MONTH, DAY, HOUR, SEQ NUMBER,
    //         SUB-DAILY SEQ NUMBER [only for v2], TARGET NAME, NOTES
    // This read MUST be ok, because we always need the version for reading the next data.

    // Get the tokens.
    std::vector<std::string> tokens = record.tokens;

    // Variables.
    std::tm date_time;
    RecordReadError result = RecordReadError::NOT_ERROR;

    // Check if the record type is correct.
    if (helpers::strings::toUpper(tokens[0]) !=
             HeaderIdStr[static_cast<int>(HeaderRecordEnum::BASIC_INFO_1_HEADER)])
        result = RecordReadError::BAD_TYPE;
    // All ok at this momment.
    else
    {
        try
        {
            // New format header struct.
            BasicInfo1Header bi1h;

            // Get the line and version.
            bi1h.cpf_version = std::stof(tokens[2]);

            // Check the data size for each version. Notes field is optional
            // For v1.
            if (bi1h.cpf_version >= 1 && bi1h.cpf_version < 2 && tokens.size() < 10)
                result = RecordReadError::BAD_SIZE;
            // For v2.
            else if(bi1h.cpf_version >= 2 && bi1h.cpf_version < 3 && tokens.size() != 11 && tokens.size() != 12)
                result = RecordReadError::BAD_SIZE;
            else
            {
                // TODO: version with decimals?
                auto it = std::find(CPFVersions.begin(), CPFVersions.end(), static_cast<int>(bi1h.cpf_version));
                if(it == CPFVersions.end())
                {
                    result = RecordReadError::VERSION_MISMATCH;
                }
                else
                {
                    bi1h.cpf_source = tokens[3];
                    // Get the file creation time UTC.
                    date_time.tm_year = std::stoi(tokens[4]) - 1900;
                    date_time.tm_mon = std::stoi(tokens[5]) - 1;
                    date_time.tm_mday = std::stoi(tokens[6]);
                    date_time.tm_hour = std::stoi(tokens[7]);
                    date_time.tm_min = 0;
                    date_time.tm_sec = 0;
                    date_time.tm_isdst = 0;
                    bi1h.cpf_production_date = std::chrono::system_clock::from_time_t(MKGMTIME(&date_time));
                    bi1h.cpf_sequence_number = std::stoi(tokens[8]);

                    if (bi1h.cpf_version >= 1.f && bi1h.cpf_version < 2.f)
                    {
                        // Target name and notes can have spaces
                        std::string last;
                        unsigned int i = 9;
                        while (i < tokens.size())
                        {
                            last.insert(last.end(), tokens[i].cbegin(), tokens[i].cend());
                            i++;
                        }
                        bi1h.target_name = last.substr(0, 10);
                        if (last.size() > 11)
                            bi1h.cpf_notes = last.substr(11, 10);
                    }
                    else
                    {
                        bi1h.cpf_subsequence_number = std::stoi(tokens[9]);
                        bi1h.target_name = tokens[10];
                        if (tokens.size() > 11)
                            bi1h.cpf_notes = tokens[11];
                    }

                    // Add the associated comments, the line number, and the tokens.
                    bi1h.comment_block = record.comment_block;
                    bi1h.line_number = record.line_number;
                    bi1h.tokens = record.tokens;

                    // Finally, store header if no converssion error ocurred
                    this->basic_info1_header = std::move(bi1h);
                }
            }

        } catch (...)
        {
            result = RecordReadError::CONVERSION_ERROR;
        }
    }

    // If there was any error, clear format header
    if (RecordReadError::NOT_ERROR != result)
        this->clearBasicInfo1Header();

    // Return the result.
    return result;
}

// Specific function for reading H2.
RecordReadError CPFHeader::readBasicInfo2Header(const ConsolidatedRecord& record)
{
    // Tokens: H2, ILRSID, SIC, Norad, Start Year, Start Month, Start Day, Start Hour, Start Minute, Start Second,
    //         End Year, End Month, End Day, End Hour, End Minute, End Second, Time between entries, TIV compatibility,
    //         Target class, Reference frame, Rotational angle type, CoM correction applied, Target dynamics

    // Delete the current data.
    this->clearBasicInfo2Header();

    // Get the tokens.
    std::vector<std::string> tokens = record.tokens;

    // Check if format header is ok
    if(!this->basic_info1_header)
        return RecordReadError::VERSION_UNKNOWN;

    // Check the data size for each version.
    // For v1.
    if (this->basic_info1_header->cpf_version >= 1 && this->basic_info1_header->cpf_version < 2 && tokens.size() != 22)
        return RecordReadError::BAD_SIZE;
    // For v2.
    else if(this->basic_info1_header->cpf_version >= 2 &&
            this->basic_info1_header->cpf_version < 3 && tokens.size() != 23)
        return RecordReadError::BAD_SIZE;
    // Check if the record type is correct.
    else if (helpers::strings::toUpper(tokens[0]) !=
             HeaderIdStr[static_cast<int>(HeaderRecordEnum::BASIC_INFO_2_HEADER)])
        return RecordReadError::BAD_TYPE;
    // All ok at this momment.
    try
    {
        // New station header struct.
        BasicInfo2Header bi2h;
        std::tm date_time;

        // Get the data.
        bi2h.id = tokens[1];
        if ("-1" != tokens[2])
            bi2h.sic = tokens[2];
        // Sometimes Norad comes with leading zeroes. We will trim them. Norad 0 is not valid.
        auto norad = tokens[3];
        norad.erase(0, norad.find_first_not_of('0'));
        bi2h.norad = norad;
        // Get the file start time UTC.
        date_time.tm_year = std::stoi(tokens[4]) - 1900;
        date_time.tm_mon = std::stoi(tokens[5]) - 1;
        date_time.tm_mday = std::stoi(tokens[6]);
        date_time.tm_hour = std::stoi(tokens[7]);
        date_time.tm_min = std::stoi(tokens[8]);
        date_time.tm_sec = std::stoi(tokens[9]);
        date_time.tm_isdst = 0;
        bi2h.start_time = std::chrono::system_clock::from_time_t(MKGMTIME(&date_time));

        // Get the file end time UTC.
        date_time.tm_year = std::stoi(tokens[10]) - 1900;
        date_time.tm_mon = std::stoi(tokens[11]) - 1;
        date_time.tm_mday = std::stoi(tokens[12]);
        date_time.tm_hour = std::stoi(tokens[13]);
        date_time.tm_min = std::stoi(tokens[14]);
        date_time.tm_sec = std::stoi(tokens[15]);
        date_time.tm_isdst = 0;
        bi2h.end_time = std::chrono::system_clock::from_time_t(MKGMTIME(&date_time));
        bi2h.time_between_entries = std::chrono::seconds(std::stoi(tokens[16]));
        bi2h.tiv_compatible = "1" == tokens[17];
        bi2h.target_class = static_cast<TargetClassEnum>(std::stoi(tokens[18]));
        bi2h.reference_frame = static_cast<ReferenceFrameEnum>(std::stoi(tokens[19]));
        bi2h.rot_angle_type = static_cast<RotAngleTypeEnum>(std::stoi(tokens[20]));
        bi2h.com_applied = "1" == tokens[21];

        // Only for v2.
        if(this->basic_info1_header->cpf_version >= 2 && this->basic_info1_header->cpf_version < 3)
            bi2h.target_dynamics = static_cast<TargetDynamicsEnum>(std::stoi(tokens[22]));

        // Add the associated comments, the line number, and the tokens.
        bi2h.comment_block = record.comment_block;
        bi2h.line_number = record.line_number;
        bi2h.tokens = record.tokens;

        // If there was no error at reading, store header
        this->basic_info2_header = std::move(bi2h);

    } catch (...)
    {
        return RecordReadError::CONVERSION_ERROR;
    }

    // Return the result.
    return RecordReadError::NOT_ERROR;
}

// Specific function for reading H3.
RecordReadError CPFHeader::readExpectedAccuracyHeader(const ConsolidatedRecord& )
{
    return RecordReadError::NOT_IMPLEMENTED;
}

// Specific function for reading H4.
RecordReadError CPFHeader::readTransponderInfoHeader(const ConsolidatedRecord& )
{
    return RecordReadError::NOT_IMPLEMENTED;
}

// Specific function for reading H5.
RecordReadError CPFHeader::readCoMCorrectionHeader(const ConsolidatedRecord& record)
{
    // Tokens are: H5, CoM correction

    // Clear current data.
    this->clearCoMCorrectionHeader();

    // Get the tokens.
    std::vector<std::string> tokens = record.tokens;

    // Check if size is correct.
    if (tokens.size() != 2)
        return RecordReadError::BAD_SIZE;
    // Check if the record type is correct.
    else if (helpers::strings::toUpper(tokens[0]) !=
             HeaderIdStr[static_cast<int>(HeaderRecordEnum::COM_CORRECTION_HEADER)])
        return RecordReadError::BAD_TYPE;
    // All ok at this momment.
    try
    {
        // New session header struct.
        CoMCorrectionHeader comh;

        // Get prediction type.
        comh.com_correction = std::stod(tokens[1]);

        // Add the associated comments, the line number, and the tokens.
        comh.comment_block = record.comment_block;
        comh.line_number = record.line_number;
        comh.tokens = record.tokens;

        // If there was no error at reading, store header
        this->com_corr_header = std::move(comh);

    } catch (...)
    {
        return RecordReadError::CONVERSION_ERROR;
    }

    // Return the result.
    return RecordReadError::NOT_ERROR;
}

RecordReadError CPFHeader::readHeaderLine(const RecordLinePair &lpair)
{
    RecordReadError result = RecordReadError::NOT_IMPLEMENTED;

    // Select the specific read funtion for each line.
    switch (static_cast<HeaderRecordEnum>(lpair.first))
    {
        case HeaderRecordEnum::BASIC_INFO_1_HEADER:
        result = this->readBasicInfo1Header(lpair.second);
        break;

        case HeaderRecordEnum::BASIC_INFO_2_HEADER:
        result = this->readBasicInfo2Header(lpair.second);
        break;

        case HeaderRecordEnum::EXPECTED_ACCURACY_HEADER:
        result = this->readExpectedAccuracyHeader(lpair.second);
        break;

        case HeaderRecordEnum::TRANSPONDER_INFO_HEADER:
        result = this->readTransponderInfoHeader(lpair.second);
        break;

        case HeaderRecordEnum::COM_CORRECTION_HEADER:
        result = this->readCoMCorrectionHeader(lpair.second);
        break;
    }

    return result;
}

std::string CPFHeader::BasicInfo1Header::generateLine()
{
    // Base line.
    std::stringstream line_h1;

    // Get the creation time (UTC).
    std::tm *time;
    timing::HRTimePointStd timepoint = timing::HRTimePointStd::clock::now();
    std::time_t datetime = std::chrono::system_clock::to_time_t(timepoint);
    time = std::gmtime(&datetime);

    // Update the production date in the class.
    this->cpf_production_date = timepoint;

    // Generate H1 for version 1 (fixed format).
    if (this->cpf_version >= 1 && this->cpf_version < 2)
    {
        line_h1 << std::fixed << std::right
                << "H1 CPF"
                << std::setw(3) << static_cast<int>(this->cpf_version)
                << "  " << this->cpf_source.substr(0,3)
                << std::setw(5) << (time->tm_year + 1900)
                << std::setw(3) << (time->tm_mon + 1)
                << std::setw(3) << (time->tm_mday)
                << std::setw(3) << (time->tm_hour)
                << std::setw(6) << this->cpf_sequence_number
                << ' ' << this->target_name.substr(0, 10)
                << ' ' << this->cpf_notes.substr(0, 10);
    }

    // Generate H1 for version 2 (free format).
    if (this->cpf_version >= 2  && this->cpf_version < 3)
    {
        line_h1 << "H1 CPF"
                << ' ' << this->cpf_version          // Free format, so we could print "2.0", "2.1", etc.
                << ' ' << this->cpf_source
                << ' ' << (time->tm_year + 1900)
                << ' ' << (time->tm_mon + 1)
                << ' ' << (time->tm_mday)
                << ' ' << (time->tm_hour)
                << ' ' << this->cpf_sequence_number
                << ' ' << this->cpf_subsequence_number
                << ' ' << this->target_name
                << ' ' << this->cpf_notes;
    }

    // Return the H1
    return line_h1.str();
}

std::string CPFHeader::BasicInfo2Header::generateLine(float version) const
{
    // H2 Base line.
    std::stringstream line_h2;

    // Get the session times.
    std::time_t start = std::chrono::system_clock::to_time_t(this->start_time);
    std::time_t end = std::chrono::system_clock::to_time_t(this->end_time);
    std::tm start_tm(*std::gmtime(&start));
    std::tm end_tm(*std::gmtime(&end));

    // For version 1 (fixed format).
    if (version >= 1 && version < 2)
    {
        // TODO: name should be lowercase only in ILRS or always?
        line_h2 << std::fixed << std::left
                << "H2" << std::right
                << std::setw(9)  << this->id.substr(0, 8)
                << std::setw(5)  << (this->sic ? this->sic->substr(0, 4) : "9999")
                << std::setw(9)  << (this->norad.empty() ? "99999999" : this->norad.substr(0, 8))
                << std::setw(5) << (start_tm.tm_year + 1900)
                << std::setw(3) << (start_tm.tm_mon + 1)
                << std::setw(3) << (start_tm.tm_mday)
                << std::setw(3) << (start_tm.tm_hour)
                << std::setw(3) << (start_tm.tm_min)
                << std::setw(3) << (start_tm.tm_sec)
                << std::setw(5) << (end_tm.tm_year + 1900)
                << std::setw(3) << (end_tm.tm_mon + 1)
                << std::setw(3) << (end_tm.tm_mday)
                << std::setw(3) << (end_tm.tm_hour)
                << std::setw(3) << (end_tm.tm_min)
                << std::setw(3) << (end_tm.tm_sec)
                << std::setw(6) << this->time_between_entries.count()
                << std::setw(2) << (this->tiv_compatible ? 1 : 0)
                << std::setw(2) << static_cast<int>(this->target_class)
                << std::setw(3) << static_cast<int>(this->reference_frame)
                << std::setw(2) << static_cast<int>(this->rot_angle_type)
                << std::setw(2) << (this->com_applied ? 1 : 0);
    }

    // For v2 only (free format).
    if(version >= 2 && version < 3)
    {
        line_h2 << std::fixed
                << "H2"
                << ' ' << this->id
                << ' ' << (this->sic ? *this->sic : "na")
                << ' ' << (this->norad.empty() ? "na" : this->norad)
                << ' ' << (start_tm.tm_year + 1900)
                << ' ' << (start_tm.tm_mon + 1)
                << ' ' << (start_tm.tm_mday)
                << ' ' << (start_tm.tm_hour)
                << ' ' << (start_tm.tm_min)
                << ' ' << (start_tm.tm_sec)
                << ' ' << (end_tm.tm_year + 1900)
                << ' ' << (end_tm.tm_mon + 1)
                << ' ' << (end_tm.tm_mday)
                << ' ' << (end_tm.tm_hour)
                << ' ' << (end_tm.tm_min)
                << ' ' << (end_tm.tm_sec)
                << ' ' << this->time_between_entries.count()
                << ' ' << (this->tiv_compatible ? 1 : 0)
                << ' ' << static_cast<int>(this->target_class)
                << ' ' << static_cast<int>(this->reference_frame)
                << ' ' << static_cast<int>(this->rot_angle_type)
                << ' ' << (this->com_applied ? 1 : 0)
                << ' ' << static_cast<int>(this->target_dynamics);
    }

    // Return the H2
    return line_h2.str();
}

std::string CPFHeader::ExpectedAccuracyHeader::generateLine(float) const
{
    return "";
}

std::string CPFHeader::TransponderInfoHeader::generateLine(float) const
{
    return "";
}

std::string CPFHeader::CoMCorrectionHeader::generateLine(float version) const
{
    // H5 Base line.
    std::stringstream line_h5;

    // For version 1 (fixed format).
    if (version >= 1 && version < 2)
    {
        line_h5 << std::fixed << std::left
                << "H5 " << helpers::strings::numberToStr(this->com_correction, 6, 4);
    }

    // For v2 only (free format).
    if(version >= 2 && version < 3)
    {
        line_h5 << std::fixed
                << "H5"
                << ' ' << this->com_correction;
    }

    // Return the H5
    return line_h5.str();
}

// =====================================================================================================================
}}} // END NAMESPACES
// =====================================================================================================================
