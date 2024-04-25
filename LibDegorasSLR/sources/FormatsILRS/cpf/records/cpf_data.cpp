/***********************************************************************************************************************
 *   LibDegorasSLR (Degoras Project SLR Library).                                                                      *
 *                                                                                                                     *
 *   A modern and efficient C++ base library for Satellite Laser Ranging (SLR) software and real-time hardware         *
 *   related developments. Developed as a free software under the context of Degoras Project for the Spanish Navy      *
 *   Observatory SLR station (SFEL) in San Fernando and, of course, for any other station that wants to use it!        *
 *                                                                                                                     *
 *   Copyright (C) 2024 Degoras Project Team                                                                           *
 *                      < Ángel Vera Herrera, avera@roa.es - angeldelaveracruz@gmail.com >                             *
 *                      < Jesús Relinque Madroñal >                                                                    *
 *                                                                                                                     *
 *   This file is part of LibDegorasSLR.                                                                               *
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
 * @file cpf_records.cpp
 * @author Degoras Project Team.
 * @brief This file contains the implementation of the CPFData class that abstracts the data of ILRS CPF format.
 * @copyright EUPL License
 2305.1
***********************************************************************************************************************/

// C++ INCLUDES
// =====================================================================================================================
#include <array>
#include <sstream>
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/FormatsILRS/cpf/records/cpf_data.h"
#include "LibDegorasSLR/Helpers/container_helpers.h"
#include "LibDegorasSLR/Helpers/string_helpers.h"
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

void CPFData::clearAll()
{
    this->clearPositionRecords();
    this->clearVelocityRecords();
    this->clearCorrectionRecords();
    this->clearTransponderRecords();
    this->clearOffsetFromCenterRecords();
    this->clearRotationAngleRecords();
    this->clearEarthOrientationRecords();
}

void CPFData::clearPositionRecords() {this->pos_records_.clear();}

void CPFData::clearVelocityRecords() {this->vel_records_.clear();}

void CPFData::clearCorrectionRecords() {this->corr_records_.clear();}

void CPFData::clearTransponderRecords() {this->transp_records_.clear();}

void CPFData::clearOffsetFromCenterRecords() {this->offset_records_.clear();}

void CPFData::clearRotationAngleRecords() {this->rot_angle_records_ = {};}

void CPFData::clearEarthOrientationRecords() {this->earth_orientation_records_ = {};}

void CPFData::setPositionRecords(const std::vector<PositionRecord> &records) {this->pos_records_ = records;}

void CPFData::setVelocityRecords(const std::vector<VelocityRecord> &records) {this->vel_records_ = records;}

void CPFData::setCorrectionsRecords(const std::vector<CorrectionsRecord> &records) {this->corr_records_ = records;}

void CPFData::setTransponderRecords(const std::vector<TransponderRecord> &records) {this->transp_records_ = records;}

void CPFData::setOffsetFromCenterRecords(const std::vector<OffsetFromCenterRecord> &records)
{this->offset_records_ = records;}

void CPFData::setRotationAngleRecords(const std::vector<RotationAngleRecord> &records)
{this->rot_angle_records_ = records;}

void CPFData::setEarthOrientationRecords(const std::vector<EarthOrientationRecord> &records)
{this->earth_orientation_records_ = records;}

void CPFData::addPositionRecord(const PositionRecord &rec) {this->pos_records_.push_back(rec);}

void CPFData::addVelocityRecord(const VelocityRecord &rec) {this->vel_records_.push_back(rec);}

void CPFData::addCorrectionsRecord(const CorrectionsRecord &rec) {this->corr_records_.push_back(rec);}

void CPFData::addTransponderRecord(const TransponderRecord &rec) {this->transp_records_.push_back(rec);}

void CPFData::addOffsetFromCenterRecord(const OffsetFromCenterRecord &rec) {this->offset_records_.push_back(rec);}

void CPFData::addRotationAngleRecord(const RotationAngleRecord &rec) {this->rot_angle_records_.push_back(rec);}

void CPFData::addEarthOrientationRecord(const EarthOrientationRecord &rec)
{this->earth_orientation_records_.push_back(rec);}

const std::vector<CPFData::PositionRecord> &CPFData::positionRecords() const {return this->pos_records_;}

const std::vector<CPFData::VelocityRecord> &CPFData::velocityRecords() const {return this->vel_records_;}

const std::vector<CPFData::CorrectionsRecord> &CPFData::correctionsRecords() const {return this->corr_records_;}

const std::vector<CPFData::TransponderRecord> &CPFData::transponderRecords() const {return this->transp_records_;}

const std::vector<CPFData::OffsetFromCenterRecord> &CPFData::offsetFromCenterRecords() const
{return this->offset_records_;}

const std::vector<CPFData::RotationAngleRecord> &CPFData::rotationAngleRecords() const {return this->rot_angle_records_;}

const std::vector<CPFData::EarthOrientationRecord> &CPFData::earthOrientationRecords() const
{return this->earth_orientation_records_;}

std::vector<CPFData::PositionRecord> &CPFData::positionRecords() {return this->pos_records_;}

std::vector<CPFData::VelocityRecord> &CPFData::velocityRecords() {return this->vel_records_;}

std::vector<CPFData::CorrectionsRecord> &CPFData::correctionsRecords() {return this->corr_records_;}

std::vector<CPFData::TransponderRecord> &CPFData::transponderRecords() {return this->transp_records_;}

std::vector<CPFData::OffsetFromCenterRecord> &CPFData::offsetFromCenterRecords() {return this->offset_records_;}

std::vector<CPFData::RotationAngleRecord> &CPFData::rotationAngleRecords() {return this->rot_angle_records_;}

std::vector<CPFData::EarthOrientationRecord> &CPFData::earthOrientationRecords()
{return this->earth_orientation_records_;}

std::string CPFData::generateDataLines(float version) const
{
    // Data lines
    std::stringstream data;

    // Generate the position records line.
    if (!this->pos_records_.empty())
        data << this->generatePositionRecordsLines(version);

    // TODO: the rest of the lines
    // TODO: do not insert breakline at the end

    // Return all the lines.
    return data.str();
}

std::string CPFData::generatePositionRecordsLines(float version) const
{
    // Position lines
    std::stringstream pr_line;

    // Generate the Position record line for each record.
    for(const auto& pr : this->pos_records_)
    {
        // Add the comment block.
        if(!pr.comment_block.empty())
            pr_line << pr.generateCommentBlock() << std::endl;
        pr_line << pr.generateLine(version) << std::endl;
    }

    // Return all the lines.
    return pr_line.str().substr(0, pr_line.str().find_last_of('\n'));
}

std::string CPFData::generateVelocityRecordsLines(float) const
{
    // TODO
    return "";
}

std::string CPFData::generateCorrectionsRecordsLines(float) const
{
    // TODO
    return "";
}

std::string CPFData::generateTransponderRecordsLines(float) const
{
    // TODO
    return "";
}

std::string CPFData::generateOffsetFromCenterRecordsLine(float) const
{
    // TODO
    return "";
}

std::string CPFData::generateRotationAngleRecordsLines(float) const
{
    // TODO
    return "";
}

std::string CPFData::generateEarthOrientationRecordsLine(float) const
{
    // TODO
    return "";
}

RecordReadError CPFData::readDataLine(const RecordLinePair &lpair, float version)
{
    // TODO: some of these things are common with all structs and with CRD. Maybe ConsolidatedRecord could include
    // a readfunction that receives a RecordLinePair and version?
    RecordReadError result = RecordReadError::BAD_TYPE;

    switch (static_cast<DataRecordType>(lpair.first))
    {
    case DataRecordType::POSITION_RECORD:
        result = this->readPositionRecord(lpair.second, version);
        break;

    case DataRecordType::VELOCITY_RECORD:
        result = this->readVelocityRecord(lpair.second, version);
        break;

    case DataRecordType::CORRECTIONS_RECORD:
        result = this->readCorrectionsRecord(lpair.second, version);
        break;

    case DataRecordType::TRANSPONDER_RECORD:
        result = this->readTransponderRecord(lpair.second, version);
        break;

    case DataRecordType::OFFSET_FROM_CENTER_RECORD:
        result = this->readOffsetFromCenterRecord(lpair.second, version);
        break;

    case DataRecordType::ROT_ANGLE_RECORD:
        result = this->readRotationAngleRecord(lpair.second, version);
        break;

    case DataRecordType::EARTH_ORIENTATION_RECORD:
        result = this->readEarthOrientationRecord(lpair.second, version);
        break;
    }

    return result;
}

RecordReadErrorMultimap CPFData::readData(const RecordLinesVector &rec_v, float version)
{
    // TODO: this function is common. It could be somehow inherited or shared
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
        if(dpslr::helpers::containers::find(DataIdStr, rec.getIdToken(), pos))
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

RecordReadError CPFData::readPositionRecord(
    const ConsolidatedRecord &rec, float v)
{
    PositionRecord pos_record(rec);

    // Get the tokens.
    std::vector<std::string> tokens = rec.tokens;

    // Check the data size for each version.
    // For v1 and v2
    if (v >= 1 && v < 3 && tokens.size() != 8)
        return RecordReadError::BAD_SIZE;
    // Check if the record type is correct.
    else if (tokens[0] != DataIdStr[static_cast<int>(DataRecordType::POSITION_RECORD)])
        return RecordReadError::BAD_TYPE;

    // All ok at this momment. Try to fill data from tokens.
    try
    {
        // Get the data.
        pos_record.dir_flag = static_cast<CPFData::DirectionFlag>(std::stoi(tokens[1]));
        pos_record.mjd = std::stoi(tokens[2]);
        pos_record.sod = std::stold(tokens[3]);
        pos_record.leap_second = std::stoi(tokens[4]);
        pos_record.geo_pos = {std::stold(tokens[5]), std::stold(tokens[6]), std::stold(tokens[7])};

    } catch (...)
    {
        return RecordReadError::CONVERSION_ERROR;
    }

    // All ok, insert the struct.
    this->pos_records_.push_back(pos_record);

    // Return the result.
    return RecordReadError::NOT_ERROR;
}

RecordReadError CPFData::readVelocityRecord(
    const ConsolidatedRecord &rec, float v)
{
    // Struct.
    VelocityRecord vel_record(rec);

    // Get the tokens.
    std::vector<std::string> tokens = rec.tokens;

    // Check the data size for each version.
    // For v1 and v2.
    if (v >= 1 && v < 3 && tokens.size() != 5)
        return RecordReadError::BAD_SIZE;
    // Check if the record type is correct.
    else if (tokens[0] != DataIdStr[static_cast<int>(DataRecordType::VELOCITY_RECORD)])
        return RecordReadError::BAD_TYPE;

    // All OK. Try to fill data from tokens.
    try
    {
        // Get the data.
        vel_record.dir_flag = static_cast<CPFData::DirectionFlag>(std::stoi(tokens[1]));
        vel_record.geo_vel = {std::stold(tokens[2]), std::stold(tokens[3]), std::stold(tokens[4])};

    } catch (...)
    {
        // If error occure, return conversion error code.
        return RecordReadError::CONVERSION_ERROR;
    }

    // All ok, insert the struct.
    this->vel_records_.push_back(std::move(vel_record));

    // Return the result.
    return RecordReadError::NOT_ERROR;

}

RecordReadError CPFData::readCorrectionsRecord(
    const ConsolidatedRecord &rec, float v)
{
    // Struct.
    CorrectionsRecord corr_record(rec);

    // Get the tokens.
    std::vector<std::string> tokens = rec.tokens;

    // Check the data size for each version.
    // For v1 and v2.
    if (v >= 1 && v < 3 && tokens.size() != 6)
        return RecordReadError::BAD_SIZE;
    // Check if the record type is correct.
    else if (tokens[0] != DataIdStr[static_cast<int>(DataRecordType::CORRECTIONS_RECORD)])
        return RecordReadError::BAD_TYPE;

    // All ok at this momment. Try to fill data from tokens.
    try
    {
        // Get the data.
        corr_record.dir_flag = static_cast<CPFData::DirectionFlag>(std::stoi(tokens[1]));
        corr_record.aberration_correction = {std::stold(tokens[2]), std::stold(tokens[3]), std::stold(tokens[4])};
        corr_record.range_correction = std::stold(tokens[5]);

    } catch (...)
    {
        return RecordReadError::CONVERSION_ERROR;
    }

    // All ok, insert the struct.
    this->corr_records_.push_back(corr_record);

    // Return the result.
    return RecordReadError::NOT_ERROR;
}

RecordReadError CPFData::readTransponderRecord(const ConsolidatedRecord &, float)
{
    // TODO
    return RecordReadError::NOT_IMPLEMENTED;
}

RecordReadError CPFData::readOffsetFromCenterRecord(
    const ConsolidatedRecord &, float)
{
    // TODO
    return RecordReadError::NOT_IMPLEMENTED;
}

RecordReadError CPFData::readRotationAngleRecord(
    const ConsolidatedRecord &, float)
{
    // TODO
    return RecordReadError::NOT_IMPLEMENTED;
}

RecordReadError CPFData::readEarthOrientationRecord(
    const ConsolidatedRecord &, float)
{
    // TODO
    return RecordReadError::NOT_IMPLEMENTED;
}

// ---------------------------------------------------------------------------------------------------------------------

// --- CPF DATA STRUCTS ------------------------------------------------------------------------------------------------

CPFData::PositionRecord::PositionRecord(const common::ConsolidatedRecord &rec) : ConsolidatedRecord(rec)
{

}

std::string CPFData::PositionRecord::generateLine(float) const
{
    // Base line.
    std::stringstream line_10;
    // Prepare the line.
    line_10 << DataIdStr[static_cast<int>(DataRecordType::POSITION_RECORD)]
            << ' ' << static_cast<int>(this->dir_flag)
            << ' ' << this->mjd
            << ' ' << helpers::strings::numberToStr(this->sod, 13, 6)
            << ' ' << this->leap_second
            << ' ' << helpers::strings::numberToStr(this->geo_pos.x, 17, 3)
            << ' ' << helpers::strings::numberToStr(this->geo_pos.y, 17, 3)
            << ' ' << helpers::strings::numberToStr(this->geo_pos.z, 17, 3);
    // Return the line.
    return line_10.str();
}

CPFData::VelocityRecord::VelocityRecord(const common::ConsolidatedRecord &rec) : ConsolidatedRecord(rec)
{

}

std::string CPFData::VelocityRecord::generateLine(float) const
{
    // TODO
    return "";
}


CPFData::CorrectionsRecord::CorrectionsRecord(const common::ConsolidatedRecord &rec) : ConsolidatedRecord(rec)
{

}

std::string CPFData::CorrectionsRecord::generateLine(float) const
{
    // TODO
    return "";
}

std::string CPFData::TransponderRecord::generateLine(float) const
{
    // TODO
    return "";
}

std::string CPFData::OffsetFromCenterRecord::generateLine(float) const
{
    // TODO
    return "";
}

std::string CPFData::RotationAngleRecord::generateLine(float) const
{
    // TODO
    return "";
}

std::string CPFData::EarthOrientationRecord::generateLine(float) const
{
    // TODO
    return "";
}

// ---------------------------------------------------------------------------------------------------------------------

// =====================================================================================================================
}}} // END NAMESPACES
// =====================================================================================================================
