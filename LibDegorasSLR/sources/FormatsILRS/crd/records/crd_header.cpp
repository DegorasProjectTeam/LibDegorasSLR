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
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <array>
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include <LibDegorasSLR/FormatsILRS/crd/records/crd_header.h>
#include <LibDegorasSLR/Helpers/container_helpers.h>
#include <LibDegorasSLR/Helpers/string_helpers.h>
#include <LibDegorasSLR/Timing/time_utils.h>
#include <LibDegorasSLR/Astronomical/spaceobject_utils.h>
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace ilrs{
namespace crd{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using namespace common;
using namespace timing::types;
// ---------------------------------------------------------------------------------------------------------------------

// CRD HEADER
// =====================================================================================================================

// --- CRD HEADER CONST EXPRESSIONS ------------------------------------------------------------------------------------
const std::array<unsigned, 2> CRDHeader::CRDVersions {1, 2};
// ---------------------------------------------------------------------------------------------------------------------

CRDHeader::CRDHeader(float crd_version) :
    format_header(FormatHeader())
{
    this->format_header->crd_version = crd_version;
}

CRDHeader::CRDHeader() :
    format_header(FormatHeader())
{}

void CRDHeader::clearAll()
{
    // Clear all records
    this->clearFormatHeader();
    this->clearStationHeader();
    this->clearTargetHeader();
    this->clearSessionHeader();
    this->clearPredictionHeader();
}

void CRDHeader::clearFormatHeader() {this->format_header = {};}

void CRDHeader::clearStationHeader() {this->station_header = {};}

void CRDHeader::clearTargetHeader() {this->target_header = {};}

void CRDHeader::clearSessionHeader() {this->session_header = {};}

void CRDHeader::clearPredictionHeader() {this->prediction_header = {};}

const Optional<CRDHeader::FormatHeader> &CRDHeader::formatHeader() const {return this->format_header;}

const Optional<CRDHeader::StationHeader> &CRDHeader::stationHeader() const {return this->station_header;}

const Optional<CRDHeader::TargetHeader> &CRDHeader::targetHeader() const {return this->target_header;}

const Optional<CRDHeader::SessionHeader> &CRDHeader::sessionHeader() const {return this->session_header;}

const Optional<CRDHeader::PredictionHeader> &CRDHeader::predictionHeader() const {return this->prediction_header;}

Optional<CRDHeader::FormatHeader> &CRDHeader::formatHeader() {return this->format_header;}

Optional<CRDHeader::StationHeader> &CRDHeader::stationHeader() {return this->station_header;}

Optional<CRDHeader::TargetHeader> &CRDHeader::targetHeader() {return this->target_header;}

Optional<CRDHeader::SessionHeader> &CRDHeader::sessionHeader() {return this->session_header;}

Optional<CRDHeader::PredictionHeader> &CRDHeader::predictionHeader() {return  this->prediction_header;}

void CRDHeader::setStationHeader(const StationHeader &sh) {this->station_header = sh;}

void CRDHeader::setPredictionHeader(const CRDHeader::PredictionHeader& ph) {this->prediction_header = ph;}

void CRDHeader::setFormatHeader(const FormatHeader &fh) {this->format_header = fh;}

void CRDHeader::setSessionHeader(const CRDHeader::SessionHeader &seh) {this->session_header = seh;}

void CRDHeader::setTargetHeader(const CRDHeader::TargetHeader &th) {this->target_header = th;}

std::string CRDHeader::generateHeaderLines(bool include_format_header)
{
    // TODO: hacer antes de llamar a esta funcion una funcion de chequeo que compruebe la integridad de esto.
    // Station Network ("na")
    // Target type != 0 y < 5 si v1
    // SIC ("na")
    // if lunar surface SIC and Norad to "na"
    // v1 ending year to "-1" and "na" for v2 -> this is for end sesion data

    // Header line
    std::stringstream header_line;

    // For v1 and 2.
    if(this->format_header->crd_version >= 1 && this->format_header->crd_version <=3)
    {
        // Format header.
        if(include_format_header)
        {
            // Add the associated comment block.
            if(!this->format_header->comment_block.empty())
                header_line << this->format_header->generateCommentBlock() << std::endl;
            // Add the line.
            header_line << this->format_header->generateLine() << std::endl;
        }

        // Station header.
        if(this->station_header)
        {
            // Add the associated comment block.
            if(!this->station_header->comment_block.empty())
                header_line << this->station_header->generateCommentBlock() << std::endl;
            // Add the line.
            header_line << this->station_header->generateLine(this->format_header->crd_version) << std::endl;
        }

        // Target header.
        if(this->target_header)
        {
            // Add the associated comment block.
            if(!this->target_header->comment_block.empty())
                header_line << this->target_header->generateCommentBlock() << std::endl;
            // Add the line.
            header_line << this->target_header->generateLine(this->format_header->crd_version) << std::endl;
        }

        // Session header.
        if(this->session_header)
        {
            // Add the associated comment block.
            if(!this->session_header->comment_block.empty())
                header_line << this->session_header->generateCommentBlock() << std::endl;
            // Add the line.
            header_line << this->session_header->generateLine(this->format_header->crd_version) << std::endl;
        }
    }

    // For v2 only.
    if(this->format_header->crd_version >= 2 && this->format_header->crd_version <=3)
    {
        // Prediction header.
        if(this->prediction_header)
        {
            // Add the associated comment block.
            if(!this->prediction_header->comment_block.empty())
                header_line << this->prediction_header->generateCommentBlock() << std::endl;
            // Add the line.
            header_line << this->prediction_header->generateLine(this->format_header->crd_version) << std::endl;
        }
    }

    // Return all the lines.
    return header_line.str().substr(0, header_line.str().find_last_of('\n'));
}

// Generic reading header function.
RecordReadErrorMultimap CRDHeader::readHeader(const RecordLinesVector& rec_v)
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
RecordReadError CRDHeader::readFormatHeader(const ConsolidatedRecord& record)
{
    // Tokens are: H1, CRD, VERSION, YEAR, MONTH, DAY, HOUR
    // This read MUST be ok, because we always need the version for reading the next data.

    // Get the tokens.
    std::vector<std::string> tokens = record.tokens;

    // Variables.
    std::tm date_time;
    RecordReadError result = RecordReadError::NOT_ERROR;

    std::string aux = helpers::strings::toUpper(tokens[0]);

    // Check if size is correct.
    if (tokens.size() != 7)
        result = RecordReadError::BAD_SIZE;
    // Check if the record type is correct.
    else if (helpers::strings::toUpper(tokens[0]) != HeaderIdStr[static_cast<int>(HeaderRecord::FORMAT_HEADER)])
        result = RecordReadError::BAD_TYPE;
    // All ok at this momment.
    else
    {
        try
        {
            // New format header struct.
            FormatHeader fh;

            // Get the line and version.
            fh.crd_version = std::stof(tokens[2]);

            auto it = std::find(CRDVersions.begin(), CRDVersions.end(), static_cast<int>(fh.crd_version));
            if(it == CRDVersions.end())
            {
                result = RecordReadError::VERSION_MISMATCH;
            }
            else
            {
                // Get the file creation time UTC.
                date_time.tm_year = std::stoi(tokens[3]) - 1900;
                date_time.tm_mon = std::stoi(tokens[4]) - 1;
                date_time.tm_mday = std::stoi(tokens[5]);
                date_time.tm_hour = std::stoi(tokens[6]);
                date_time.tm_min = 0;
                date_time.tm_sec = 0;
                date_time.tm_isdst = 0;
                fh.crd_production_date = std::chrono::system_clock::from_time_t(MKGMTIME(&date_time));

                // Add the associated comments, the line number, and the tokens.
                fh.comment_block = record.comment_block;
                fh.line_number = record.line_number;
                fh.tokens = record.tokens;

                // Finally, store header if no converssion error ocurred
                this->format_header = std::move(fh);
            }

        } catch (...)
        {
            result = RecordReadError::CONVERSION_ERROR;
        }
    }

    // If there was any error, clear format header
    if (RecordReadError::NOT_ERROR != result)
        this->clearFormatHeader();

    // Return the result.
    return result;
}

// Specific function for reading H2.
RecordReadError CRDHeader::readStationHeader(const ConsolidatedRecord& record)
{
    // Tokens are: H2, NAME, SYSTEM ID, SYSTEM NUMBER, SYSTEM OCCUPANCY, TIME SCALE, [STATION NETWORK]

    // Delete the current data.
    this->clearStationHeader();

    // Get the tokens.
    std::vector<std::string> tokens = record.tokens;

    // Check if format header is ok
    if(!this->format_header)
        return RecordReadError::VERSION_UNKNOWN;

    // Check the data size for each version.
    // For v1.
    if (this->format_header->crd_version >= 1 && this->format_header->crd_version < 2 && tokens.size() != 6)
        return RecordReadError::BAD_SIZE;
    // For v2.
    else if(this->format_header->crd_version >= 2 && this->format_header->crd_version < 3 && tokens.size() != 7)
        return RecordReadError::BAD_SIZE;
    // Check if the record type is correct.
    else if (helpers::strings::toUpper(tokens[0]) != HeaderIdStr[static_cast<int>(HeaderRecord::STATION_HEADER)])
        return RecordReadError::BAD_TYPE;
    // All ok at this momment.
    try
    {
        // New station header struct.
        StationHeader sh;

        // Get the data.
        sh.official_name = tokens[1];
        sh.cdp_pad_identifier = std::stoi(tokens[2]);
        sh.cdp_system_number = std::stoi(tokens[3]);
        sh.cdp_occupancy_sequence = std::stoi(tokens[4]);
        sh.epoch_timescale = static_cast<TimeScale>(std::stoi(tokens[5]));

        // Only for v2.
        if(this->format_header->crd_version >= 2 && this->format_header->crd_version < 3)
            sh.network = tokens[6];

        // Add the associated comments, the line number, and the tokens.
        sh.comment_block = record.comment_block;
        sh.line_number = record.line_number;
        sh.tokens = record.tokens;

        // If there was no error at reading, store header
        this->station_header = std::move(sh);

    } catch (...)
    {
        return RecordReadError::CONVERSION_ERROR;
    }

    // Return the result.
    return RecordReadError::NOT_ERROR;
}

// Specific function for reading H3.
RecordReadError CRDHeader::readTargetHeader(const ConsolidatedRecord& record)
{
    // Tokens are: H3, NAME, ILRSID, SIC, NORAD, EPOCH TIME SCALE, [CLASS], [LOCATION]

    // Delete the current data.
    this->clearTargetHeader();

    // Get the tokens.
    std::vector<std::string> tokens = record.tokens;

    // Check if format header is ok
    if(!this->format_header)
        return RecordReadError::VERSION_UNKNOWN;

    // Check the data size for each version.
    // For v1.
    if (this->format_header->crd_version >= 1 && this->format_header->crd_version < 2 && tokens.size() != 7)
        return RecordReadError::BAD_SIZE;
    // For v2.
    else if(this->format_header->crd_version >= 2 && this->format_header->crd_version < 3 && tokens.size() != 8)
        return RecordReadError::BAD_SIZE;
    // Check if the record type is correct.
    else if (helpers::strings::toUpper(tokens[0]) != HeaderIdStr[static_cast<int>(HeaderRecord::TARGET_HEADER)])
        return RecordReadError::BAD_TYPE;
    // All ok at this momment.
    try
    {
        // New station header struct.
        TargetHeader th;

        // Get the data.
        th.name = tokens[1];

        // Store the target id.
        std::string target_id = tokens[2];

        // We suppose that is a full COSPAR.
        if(target_id.find('-') != std::string::npos)
        {
            th.cospar = target_id;
            th.ilrsid = spobj::cosparToILRSID(target_id);
        }
        // We suppose that is SHORT COSPAR.
        else if(std::find_if(target_id.begin(), target_id.end(), ::isalpha) != target_id.end())
        {
            th.cospar = spobj::shortcosparToCospar(target_id);
            th.ilrsid = spobj::shortcosparToILRSID(target_id);
        }
        // We suppose that is a ILRS ID.
        else if(target_id.size() == 7)
        {
            th.cospar = spobj::ilrsidToShortcospar(target_id);
            th.ilrsid = target_id;
        }
        else
            throw std::runtime_error(target_id);

        // Rest of the data.
        th.sic = tokens[3];
        th.norad = tokens[4];
        th.sc_epoch_ts = static_cast<SpacecraftEpochTimeScale>(std::stoi(tokens[5]));
        th.target_class = static_cast<TargetClass>(std::stoi(tokens[6]));

        // Only for v2.
        if(this->format_header->crd_version >= 2 && this->format_header->crd_version < 3)
        {
            th.location = static_cast<TargetLocation>(std::stoi(tokens[7]));
        }

        // Add the associated comments, the line number, and the tokens.
        th.comment_block = record.comment_block;
        th.line_number = record.line_number;
        th.tokens = record.tokens;

        // If there was no error at reading, store header
        this->target_header = std::move(th);

    } catch (...)
    {
        return RecordReadError::CONVERSION_ERROR;
    }

    // Return the result.
    return RecordReadError::NOT_ERROR;
}

// Specific function for reading H4.
RecordReadError CRDHeader::readSessionHeader(const ConsolidatedRecord& record)
{
    // Tokens are: H4, TYPE, SYY, SMM, SDD, SHH, Smm, Sss, EYY, EMM, EDD, EHH, Emm, Ess, RELEASE, TROP_FLAG,
    //             MASS_FLAG, AMP, STATION_DELAY_FLAG, SPACECRAFT_DELAY_FLAG, RANGE_TYPE, QUALITY

    // Delete the current data.
    this->clearSessionHeader();

    // Get the tokens.
    std::vector<std::string> tokens = record.tokens;

    // Variables.
    std::tm start_time;
    std::tm end_time;

    // Check if size is correct.
    if (tokens.size() != 22)
        return RecordReadError::BAD_SIZE;
    // Check if the record type is correct.
    else if (helpers::strings::toUpper(tokens[0]) != HeaderIdStr[static_cast<int>(HeaderRecord::SESSION_HEADER)])
        return RecordReadError::BAD_TYPE;
    // All ok at this momment.
    try
    {
        // New session header struct
        SessionHeader sh;

        // Get data type.
        sh.data_type = static_cast<DataType>(std::stoi(tokens[1]));

        // Get the session start time UTC.
        start_time.tm_year = std::stoi(tokens[2]) - 1900;
        start_time.tm_mon = std::stoi(tokens[3]) - 1;
        start_time.tm_mday = std::stoi(tokens[4]);
        start_time.tm_hour = std::stoi(tokens[5]);
        start_time.tm_min = std::stoi(tokens[6]);
        start_time.tm_sec = std::stoi(tokens[7]);
        start_time.tm_isdst = 0;
        sh.start_time = std::chrono::system_clock::from_time_t(MKGMTIME(&start_time));

        // Get the session end time UTC.
        end_time.tm_year = std::stoi(tokens[8]) - 1900;
        end_time.tm_mon = std::stoi(tokens[9]) - 1;
        end_time.tm_mday = std::stoi(tokens[10]);
        end_time.tm_hour = std::stoi(tokens[11]);
        end_time.tm_min = std::stoi(tokens[12]);
        end_time.tm_sec = std::stoi(tokens[13]);
        end_time.tm_isdst = 0;
        sh.end_time = std::chrono::system_clock::from_time_t(MKGMTIME(&end_time));

        // Get the rest of the data.
        sh.data_release = std::stoi(tokens[14]);
        sh.trop_correction_applied = helpers::containers::BoolString(tokens[15]);
        sh.com_correction_applied = helpers::containers::BoolString(tokens[16]);
        sh.rcv_amp_correction_applied = helpers::containers::BoolString(tokens[17]);
        sh.stat_delay_applied = helpers::containers::BoolString(tokens[18]);
        sh.spcraft_delay_applied = helpers::containers::BoolString(tokens[19]);
        sh.range_type = static_cast<RangeType>(std::stoi(tokens[20]));
        sh.data_quality_alert = static_cast<DataQuality>(std::stoi(tokens[21]));

        // Add the associated comments.
        sh.comment_block = record.comment_block;
        sh.line_number = record.line_number;
        sh.tokens = record.tokens;

        // If there was no error at reading, store header
        this->session_header = std::move(sh);

    } catch (...)
    {
        return RecordReadError::CONVERSION_ERROR;
    }

    // Return the result.
    return RecordReadError::NOT_ERROR;
}

// Specific function for reading H5.
RecordReadError CRDHeader::readPredictionHeader(const ConsolidatedRecord& record)
{
    // Tokens are: H5, PRED_TYPE, YEAR_CENTURY_CPF_TLE, DATE_TIME, PRED_PROV, SEQ_NUMBER

    // Clear current data.
    this->clearPredictionHeader();

    // Get the tokens.
    std::vector<std::string> tokens = record.tokens;

    // Check if size is correct.
    if (tokens.size() != 6)
        return RecordReadError::BAD_SIZE;
    // Check if the record type is correct.
    else if (helpers::strings::toUpper(tokens[0]) != HeaderIdStr[static_cast<int>(HeaderRecord::PREDICTION_HEADER)])
        return RecordReadError::BAD_TYPE;
    // All ok at this momment.
    try
    {
        // New session header struct.
        PredictionHeader ph;

        // Get prediction type.
        ph.prediction_type = static_cast<PredictionType>(std::stoi(tokens[1]));

        // Get year of century
        int year = std::stoi(tokens[2]);

        // Get prediction file creation time
        if (ph.prediction_type == PredictionType::CPF)
        {
            // TODO: change this by std::get_time whenever it is available in gcc
            int month = std::stoi(tokens[3].substr(0,2));
            int day = std::stoi(tokens[3].substr(2,2));
            int hour = std::stoi(tokens[3].substr(4,2));
            std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            std::tm* date = std::gmtime(&now);
            // Set timepoint at current century
            date->tm_year -= date->tm_year % 100;
            date->tm_year += year;
            date->tm_mon = month;
            date->tm_mday = day;
            date->tm_hour = hour;
            date->tm_min = 0;
            date->tm_sec = 0;
            std::time_t prod_date = MKGMTIME(date);
            ph.file_creation_time = std::chrono::system_clock::from_time_t(prod_date);
        }
        else if (ph.prediction_type == PredictionType::TLE)
        {
            // Get day with fractional part
            double day = std::stod(tokens[3]);
            ph.file_creation_time = timing::tleDateToTimePoint(year, day);
        }
        else
        {
            return RecordReadError::NOT_IMPLEMENTED;
        }

        // Get prediction provider and sequence number.
        ph.prediction_provider = tokens[4];
        ph.sequence_number = std::stoi(tokens[5]);

        // Add the associated comments, the line number, and the tokens.
        ph.comment_block = record.comment_block;
        ph.line_number = record.line_number;
        ph.tokens = record.tokens;

        // If there was no error at reading, store header
        this->prediction_header = std::move(ph);

    } catch (...)
    {
        return RecordReadError::CONVERSION_ERROR;
    }

    // Return the result.
    return RecordReadError::NOT_ERROR;
}

RecordReadError CRDHeader::readHeaderLine(const RecordLinePair &lpair)
{ 
    RecordReadError result = RecordReadError::NOT_IMPLEMENTED;

    // Select the specific read funtion for each line.
    switch (static_cast<HeaderRecord>(lpair.first))
    {
    case HeaderRecord::FORMAT_HEADER:
        result = this->readFormatHeader(lpair.second);
        break;

    case HeaderRecord::STATION_HEADER:
        result = this->readStationHeader(lpair.second);
        break;

    case HeaderRecord::TARGET_HEADER:
        result = this->readTargetHeader(lpair.second);
        break;

    case HeaderRecord::SESSION_HEADER:
        result = this->readSessionHeader(lpair.second);
        break;

    case HeaderRecord::PREDICTION_HEADER:
        result = this->readPredictionHeader(lpair.second);
        break;
    }

    return result;
}

std::string CRDHeader::FormatHeader::generateLine()
{
    // Base line.
    std::stringstream line_h1;

    // Get the creation time (UTC).
    std::tm *time;
    HRTimePointStd timepoint = HRTimePointStd::clock::now();
    std::time_t datetime = std::chrono::system_clock::to_time_t(timepoint);
    time = std::gmtime(&datetime);

    // Update the production date in the class.
    this->crd_production_date = timepoint;

    // Generate H1 for version 1 (fixed format).
    if (this->crd_version >= 1 && this->crd_version < 2)
    {
        line_h1 << std::fixed << std::right
                << "H1 CRD"
                << std::setw(3) << static_cast<int>(this->crd_version)
                << std::setw(5) << (time->tm_year + 1900)
                << std::setw(3) << (time->tm_mon + 1)
                << std::setw(3) << (time->tm_mday)
                << std::setw(3) << (time->tm_hour);
    }

    // Generate H1 for version 2 (free format).
    if (this->crd_version >= 2  && this->crd_version < 3)
    {
        line_h1 << "H1 CRD"
                << ' ' << this->crd_version          // Free format, so we could print "2.0", "2.1", etc.
                << ' ' << (time->tm_year + 1900)
                << ' ' << (time->tm_mon + 1)
                << ' ' << (time->tm_mday)
                << ' ' << (time->tm_hour);
    }

    // Return the H1
    return line_h1.str();
}

std::string CRDHeader::StationHeader::generateLine(float version) const
{
    // H2 Base line.
    std::stringstream line_h2;

    // For version 1 and 2 (fixed format).
    if (version >= 1 && version < 2)
    {
        line_h2 << std::fixed << std::left
                << "H2 "
                << std::setw(10) << this->official_name.substr(0, 10)
                << std::right
                << std::setw(5)  << this->cdp_pad_identifier
                << std::setw(3)  << this->cdp_system_number
                << std::setw(3)  << this->cdp_occupancy_sequence
                << std::setw(3)  << static_cast<int>(this->epoch_timescale);
    }

    // For v2 only (free format).
    if(version >= 2 && version < 3)
    {
        line_h2 << std::fixed
                << "H2"
                << ' ' << this->official_name
                << ' ' << this->cdp_pad_identifier
                << ' ' << this->cdp_system_number
                << ' ' << this->cdp_occupancy_sequence
                << ' ' << static_cast<int>(this->epoch_timescale)
                << ' ' << (this->network.empty() ? "na" : this->network);
    }

    // Return the H2
    return line_h2.str();
}

std::string CRDHeader::TargetHeader::generateLine(float version) const
{
    // H3 Base line.
    std::stringstream line_h3;

    // For version 1 (fixed format).
    if (version >= 1 && version < 2)
    {
        // TODO: name should be lowercase only in ILRS or always?
        line_h3 << std::fixed << std::left
                << "H3 "
                << std::setw(10) << helpers::strings::toLower(this->name.substr(0, 10))
                << std::right
                << std::setw(9)  << this->ilrsid.substr(0, 8)
                << std::setw(5)  << (this->sic.empty() ? "9999" : this->sic.substr(0, 4))
                << std::setw(9)  << (this->norad.empty() ? "99999999" : this->norad.substr(0, 8))
                << std::setw(2)  << static_cast<int>(this->sc_epoch_ts)
                << std::setw(2)  << static_cast<int>(this->target_class);
    }

    // For v2 only (free format).
    if(version >= 2 && version < 3)
    {
        line_h3 << std::fixed
                << "H3"
                << ' ' << helpers::strings::toLower(this->name)
                << ' ' << this->ilrsid
                << ' ' << (this->sic.empty() ? "na" : this->sic)
                << ' ' << (this->norad.empty() ? "na" : this->norad)
                << ' ' << static_cast<int>(this->sc_epoch_ts)
                << ' ' << static_cast<int>(this->target_class)
                << ' ' << (this->location == TargetLocation::UNKNOWN_LOCATION ?
                               "na" : std::to_string(static_cast<int>(this->location)));
    }

    // Return the H3
    return line_h3.str();
}

std::string CRDHeader::SessionHeader::generateLine(float version) const
{
    // H4 Base line
    std::stringstream line_h4;

    // Get the session times.
    std::time_t start = std::chrono::system_clock::to_time_t(this->start_time);
    std::time_t end = std::chrono::system_clock::to_time_t(this->end_time);
    std::tm start_tm(*std::gmtime(&start));
    std::tm end_tm(*std::gmtime(&end));

    // For version 1 (fixed format).
    if (version >= 1 && version < 2)
    {
        line_h4 << std::fixed << std::right
                << "H4"
                << std::setw(3) << static_cast<int>(this->data_type)
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
                << std::setw(3) << this->data_release
                << std::setw(2) << this->trop_correction_applied
                << std::setw(2) << this->com_correction_applied
                << std::setw(2) << this->rcv_amp_correction_applied
                << std::setw(2) << this->stat_delay_applied
                << std::setw(2) << this->spcraft_delay_applied
                << std::setw(2) << static_cast<int>(this->range_type)
                << std::setw(2) << static_cast<int>(this->data_quality_alert);
    }

    // For version 2 (free format).
    if (version >= 2 && version < 3)
    {
        line_h4 << std::fixed << std::left
                << "H4"
                << ' ' << static_cast<int>(this->data_type)
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
                << ' ' << this->data_release
                << ' ' << this->trop_correction_applied
                << ' ' << this->com_correction_applied
                << ' ' << this->rcv_amp_correction_applied
                << ' ' << this->stat_delay_applied
                << ' ' << this->spcraft_delay_applied
                << ' ' << static_cast<int>(this->range_type)
                << ' ' << static_cast<int>(this->data_quality_alert);
    }

    // Return the H4
    return line_h4.str();
}

std::string CRDHeader::PredictionHeader::generateLine(float version) const
{
    // H5 Base line.
    std::stringstream line_h5;

    // For v2 only (free format).
    if(version >= 2 && version < 3)
    {
        // Prediction type.
        line_h5 << std::fixed
                << "H5"
                << ' ' << static_cast<int>(this->prediction_type);

        if (this->prediction_type == PredictionType::CPF)
        {
            // Get prediction file creation time.
            std::time_t creation = std::chrono::system_clock::to_time_t(this->file_creation_time);
            std::tm creation_tm(*std::gmtime(&creation));

            // Store the data.
            line_h5 << ' ' << creation_tm.tm_year % 100
                    << ' ' << creation_tm.tm_mon << creation_tm.tm_mday << creation_tm.tm_hour;
        }
        else if (this->prediction_type == PredictionType::TLE)
        {
            // Aux variables.
            int year;
            long double fractional;

            // Calculate the fractional day.
            timing::timePointToTLEDate(this->file_creation_time, year, fractional);

            // Store the data.
            line_h5 << std::setprecision(11)
                    << ' ' << year
                    << ' ' << fractional;
        }

        // Provider and sequence number.
        line_h5 << ' ' << this->prediction_provider
                << ' ' << this->sequence_number;
    }

    // Return the H5
    return line_h5.str();
}

// ---------------------------------------------------------------------------------------------------------------------

}}} // END NAMESPACES
// =====================================================================================================================
