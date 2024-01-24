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
 * @file cpf_data.h
 * @author Degoras Project Team.
 * @brief This file contains the declaration of the CPFData class that abstracts the data of ILRS CPF format.
 * @copyright EUPL License
 * @version 2305.1
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
//======================================================================================================================
#include <array>
#include <string>
#include <vector>
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdpslr_global.h"
#include "LibDegorasSLR/FormatsILRS/common/consolidated_types.h"
#include "LibDegorasSLR/FormatsILRS/common/consolidated_record.h"
#include "LibDegorasSLR/Mathematics/containers/vector3d.h"
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace ilrs{
namespace cpf{
// =====================================================================================================================

// =====================================================================================================================
using math::Vector3D;
// =====================================================================================================================

// CPF DATA
// =====================================================================================================================

class LIBDPSLR_EXPORT CPFData
{
public:

    // CPF DATA ENUMS
    // -----------------------------------------------------------------------------------------------------------------

    /**
     * @enum DataRecordType
     * @brief Data record types.
     */
    enum class DataRecordType
    {
        POSITION_RECORD               = 0,     ///< Line 10. Position record.
        VELOCITY_RECORD               = 3,     ///< Line 20. Velocity record.
        CORRECTIONS_RECORD            = 5,     ///< Line 30. Corrections record.
        TRANSPONDER_RECORD            = 6,     ///< Line 40. Transponder specific record.
        OFFSET_FROM_CENTER_RECORD     = 9,     ///< Line 50. Offset from center of main body record.
        ROT_ANGLE_RECORD              = 10,    ///< Line 60. Rotation angle of offset record.
        EARTH_ORIENTATION_RECORD      = 11     ///< Line 70. Earth orientation record.
    };

    /**
     * @enum DirectionFlag
     * @brief Direction flag.
     */
    enum class DirectionFlag
    {
        COMMON_EPOCH  = 0,           ///< Common epoch. Instantaneous vector between geocenter and target.
        TRANSMIT      = 1,           ///< Transmit. Iterated travel time from geocenter to target at transmit epoch.
        RECEIVE       = 2            ///< Receive. Iterated travel time from target to geocenter at receive epoch.
    };

    // -----------------------------------------------------------------------------------------------------------------

    // CPF DATA STRUCTS
    // -----------------------------------------------------------------------------------------------------------------
    /**
     * @brief 10 - POSITION RECORD
     */
    struct PositionRecord : common::ConsolidatedRecord
    {
        // Members.
        DirectionFlag dir_flag;             ///< Direction flag.
        long long mjd;                      ///< Modified Julian Date.
        long double sod;                    ///< Second of day (UTC).
        int leap_second;                    ///< Leap second flag (0 or the value of new leap second).
        Vector3D<long double> position;     ///< Geocentric position in meters (x, y, z).

        /**
         * @brief Generate the line for this record.
         * @param version
         * @return The generated line.
         * @todo
         */
        std::string generateLine(float version) const;
    };

    /**
     * @brief 20 - VELOCITY RECORD
     */
    struct VelocityRecord : common::ConsolidatedRecord
    {
        // Members.
        DirectionFlag dir_flag;              ///< Direction flag.
        Vector3D<long double> velocity;      ///< Geocentric velocity in m/s (x, y, z).

        /**
         * @brief Generate the line for this record.
         * @param version
         * @return The generated line.
         * @todo
         */
        std::string generateLine(float version) const;
    };

    /**
     * @brief 30 - CORRECTIONS RECORD
     */
    struct CorrectionsRecord : common::ConsolidatedRecord
    {
        // Members.
        DirectionFlag dir_flag;                         ///<< Direction flag
        std::array<long double, 3> aberration_correction;   ///<< Stellar aberration correction in meters (x, y, z)
        double range_correction;                            ///<< Relativistic range correction in ns (positive)
        // Functions.
        /**
         * @brief Generate the line for this record.
         * @param version
         * @return The generated line.
         * @todo
         */
        std::string generateLine(float version) const;
    };

    /**
     * @brief TODO: 40 - TRANSPONDER RECORD
     * @todo
     */
    struct TransponderRecord : common::ConsolidatedRecord
    {
        // Members.

        // Functions.
        /**
         * @brief Generate the line for this record.
         * @param version
         * @return The generated line.
         * @todo
         */
        std::string generateLine(float version) const;
    };

    /**
     * @brief 50 - OFFSET FROM CENTER OF MAIN BODY RECORD
     * @todo
     */
    struct OffsetFromCenterRecord : common::ConsolidatedRecord
    {
        // Members

        // Functions
        /**
         * @brief Generate the line for this record.
         * @param version
         * @return The generated line.
         * @todo
         */
        std::string generateLine(float version) const;
    };

    /**
     * @brief TODO: 60 - ROTATION ANGLE OF OFFSET RECORD
     */
    struct RotationAngleRecord : common::ConsolidatedRecord
    {
        // Members

        // Functions
        /**
         * @brief Generate the line for this record.
         * @param version
         * @return The generated line.
         * @todo
         */
        std::string generateLine(float version) const;
    };

    /**
     * @brief TODO: 70 - EARTH ORIENTATION RECORD
     */
    struct EarthOrientationRecord : common::ConsolidatedRecord
    {
        // Members

        // Functions
        /**
         * @brief Generate the line for this record.
         * @param version
         * @return The generated line.
         * @todo
         */
        std::string generateLine(float version) const;
    };

    // Constructor.
    CPFData() = default;

    // Copy and movement constructors and assign operators
    CPFData(const CPFData&) = default;
    CPFData(CPFData&&) = default;
    CPFData& operator = (const CPFData&) = default;
    CPFData& operator = (CPFData&&) = default;

    // Destructor.
    ~CPFData() = default;

    // Clear methods.
    /**
     * @brief Clear all records.
     */
    void clearAll();
    /**
     * @brief Clears all the position records
     */
    void clearPositionRecords();
    /**
     * @brief Clears all the Position Records
     */
    void clearVelocityRecords();
    /**
     * @brief Clears all the Velocity Records
     */
    void clearCorrectionRecords();
    /**
     * @brief Clears all the Correction Records
     */
    void clearTransponderRecords();
    /**
     * @brief Clears all the Transponder Records
     */
    void clearOffsetFromCenterRecords();
    /**
     * @brief Clears all the Offset From Center Records
     */
    void clearRotationAngleRecords();
    /**
     * @brief Clears all the Earth Orientation Records
     */
    void clearEarthOrientationRecords();

    // Const getters
    const std::vector<PositionRecord> &positionRecords() const;
    const std::vector<VelocityRecord> &velocityRecords() const;
    const std::vector<CorrectionsRecord> &correctionsRecords() const;
    const std::vector<TransponderRecord> &transponderRecords() const;
    const std::vector<OffsetFromCenterRecord> &offsetFromCenterRecords() const;
    const std::vector<RotationAngleRecord> &rotationAngleRecords() const;
    const std::vector<EarthOrientationRecord> &earthOrientationRecords() const;

    // Non-const getters
    std::vector<PositionRecord> &positionRecords();
    std::vector<VelocityRecord> &velocityRecords();
    std::vector<CorrectionsRecord> &correctionsRecords();
    std::vector<TransponderRecord> &transponderRecords();
    std::vector<OffsetFromCenterRecord> &offsetFromCenterRecords();
    std::vector<RotationAngleRecord> &rotationAngleRecords();
    std::vector<EarthOrientationRecord> &earthOrientationRecords();


    // Setter methods
    void setPositionRecords(const std::vector<PositionRecord>&);
    void setVelocityRecords(const std::vector<VelocityRecord>&);
    void setCorrectionsRecords(const std::vector<CorrectionsRecord>&);
    void setTransponderRecords(const std::vector<TransponderRecord>&);
    void setOffsetFromCenterRecords(const std::vector<OffsetFromCenterRecord>&);
    void setRotationAngleRecords(const std::vector<RotationAngleRecord>&);
    void setEarthOrientationRecords(const std::vector<EarthOrientationRecord>&);


    // Records individual setter methods.
    /**
     * @brief Adds a position record to the end of the current list of position records.
     * @param rec, the position record to append.
     */
    void addPositionRecord(const PositionRecord &rec);
    /**
     * @brief Adds a velocity record to the end of the current list of velocity records.
     * @param rec, the velocity record to append.
     */
    void addVelocityRecord(const VelocityRecord &rec);
    /**
     * @brief Adds a corrections record to the end of the current list of corrections records.
     * @param rec, the corretions record to append.
     */
    void addCorrectionsRecord(const CorrectionsRecord &rec);
    /**
     * @brief Adds a transponder record to the end of the current list of transponder records.
     * @param rec, the transponder record to append.
     */
    void addTransponderRecord(const TransponderRecord &rec);
    /**
     * @brief Adds an offset from center record to the end of the current list of offset from center records.
     * @param rec, the offset from center record to append.
     */
    void addOffsetFromCenterRecord(const OffsetFromCenterRecord &rec);
    /**
     * @brief Adds a rotation angle record to the end of the current list of rotation angle records.
     * @param rec, the rotation angle record to append.
     */
    void addRotationAngleRecord(const RotationAngleRecord &rec);
    /**
     * @brief Adds an earth orientation record to the end of the current list of earth orientation records.
     * @param rec, the earth orientation record to append.
     */
    void addEarthOrientationRecord(const EarthOrientationRecord &rec);

    // Generate CPF lines
    /**
     * @brief Generates all CPF data lines.
     * @param version
     * @return The CPF data lines generated.
     * @todo
     */
    std::string generateDataLines(float version) const;

    // Generate specific CPF lines.
    /**
     * @brief Generates position records (10) lines.
     * @param version
     * @return The CPF position records generated lines.
     * @todo
     */
    std::string generatePositionRecordsLines(float version) const;
    /**
     * @brief Generates velocity records (20) lines.
     * @param version
     * @return The CPF velocity records generated lines.
     * @todo
     */
    std::string generateVelocityRecordsLines(float version) const;
    /**
     * @brief Generates corrections records (30) lines.
     * @param version
     * @return The CPF corrections records generated lines.
     * @todo
     */
    std::string generateCorrectionsRecordsLines(float version) const;
    /**
     * @brief Generates transponder records (40) lines.
     * @param version
     * @return The CPF transponder records generated lines.
     * @todo
     */
    std::string generateTransponderRecordsLines(float version) const;
    /**
     * @brief Generates offset from center records (50) lines.
     * @param version
     * @return The CPF offset from center records generated lines.
     * @todo
     */
    std::string generateOffsetFromCenterRecordsLine(float version) const;
    /**
     * @brief Generates rotation angle records (60) lines.
     * @param version
     * @return The CPF rotation angle records generated lines.
     * @todo
     */
    std::string generateRotationAngleRecordsLines(float version) const;
    /**
     * @brief Generates earth orientation records (70) lines.
     * @param version
     * @return The CPF earth orientation records generated lines.
     * @todo
     */
    std::string generateEarthOrientationRecordsLine(float version) const;

    // Generic read methods.
    // Calls to clearAll()
    common::RecordReadErrorMultimap readData(const common::RecordLinesVector &rec_v, float version);

    // Specific read methods.
    // The read data-line functions append the data to the proper vector. Do nothing if error.
    // The read data functions clear the pointers (struct) and create a new one (or nullptr if error).
    // TODO Mover esto y aplicar patrón factoría.
    common::RecordReadError readPositionRecord(const common::ConsolidatedRecord&, float);
    common::RecordReadError readVelocityRecord(const common::ConsolidatedRecord&, float);
    common::RecordReadError readCorrectionsRecord(const common::ConsolidatedRecord&, float);
    common::RecordReadError readTransponderRecord(const common::ConsolidatedRecord&, float);
    common::RecordReadError readOffsetFromCenterRecord(const common::ConsolidatedRecord&, float);
    common::RecordReadError readRotationAngleRecord(const common::ConsolidatedRecord&, float);
    common::RecordReadError readEarthOrientationRecord(const common::ConsolidatedRecord&, float);


private:

    // Generic private read method.
    common::RecordReadError readDataLine(const common::RecordLinePair &rpair, float version);

    // Private vectors for store the different data records.
    std::vector<PositionRecord> pos_records;
    std::vector<VelocityRecord> vel_records;
    std::vector<CorrectionsRecord> corr_records;
    std::vector<TransponderRecord> transp_records;
    std::vector<OffsetFromCenterRecord> offset_records;
    std::vector<RotationAngleRecord> rot_angle_records;
    std::vector<EarthOrientationRecord> earth_orientation_records;
};

// =====================================================================================================================

}}} // END NAMESPACES
// =====================================================================================================================
