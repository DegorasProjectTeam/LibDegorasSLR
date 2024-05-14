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
 * @file cpf_header.h
 * @author Degoras Project Team.
 * @brief This file contains the declaration of the CPFHeader class that abstracts the header of ILRS CPF format.
 * @copyright EUPL License
 2305.1
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
// =====================================================================================================================
#include <array>
#include <string>
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
#include "LibDegorasSLR/Timing/types/base_time_types.h"
#include "LibDegorasSLR/Helpers/common_aliases_macros.h"
#include "LibDegorasSLR/FormatsILRS/common/consolidated_types.h"
#include "LibDegorasSLR/FormatsILRS/common/consolidated_record.h"
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace ilrs{
namespace cpf{
// =====================================================================================================================

// CPF HEADER
// =====================================================================================================================

/**
 * @brief This class manages the CPF header records (H1, H2, H3, H4, H4).
 */
class LIBDPSLR_EXPORT CPFHeader
{
public:

    // CPF CONSTANTS
    // -----------------------------------------------------------------------------------------------------------------
    static const std::array<unsigned, 2> CPFVersions;                /// CPF main versions.
    // -----------------------------------------------------------------------------------------------------------------

    // CPF HEADERS ENUMS
    // -----------------------------------------------------------------------------------------------------------------

    /// @enum HeaderRecordEnum
    /// This enum represents the different types of Header Records.
    enum class HeaderRecordEnum
    {
        BASIC_INFO_1_HEADER,         ///< Basic iformation 1 Header. Line H1 (mandatory).
        BASIC_INFO_2_HEADER,         ///< Basic information 2 Header. Line H2 (mandatory).
        EXPECTED_ACCURACY_HEADER,    ///< Expected accuracy Header. Line H3 (optional).
        TRANSPONDER_INFO_HEADER,     ///< Transponder information Header. Line H4 (optional).
        COM_CORRECTION_HEADER        ///< Spherical satellite center of mass correction Header. Line H5 (optional).
    };

    /// @enum TargetClassEnum
    /// This enum represents the different classes of targets.
    enum class TargetClassEnum
    {
        NO_LRR,              ///< No retroreflector (including debris).                                    [only for v2]
        PASSIVE_LRR,         ///< Passive retroreflector.
        PASSIVE_LRR_LUNAR,   ///< Passive lunar retroreflector. In v2, use instead PASSIVE_LRR.       [deprecated in v2]
        SYNC_TRANSPONDER,    ///< Synchronous transponder.
        ASYNC_TRANSPONDER    ///< Asynchronous transponder.
    };

    /// @enum ReferenceFrameEnum
    /// This enum represents the reference frame used for the coordinates system.
    enum class ReferenceFrameEnum
    {
        GEOCENTRIC_BODY_FIXED,             ///< Geocentric true body-fixed (default).
        GEOCENTRIC_SPACE_FIXED_TOD,        ///< Geocentric space-fixed (i.e, Inertial) (True-of-Date).
        GEOCENTRIC_SPACE_FIXED_MODJ2000    ///< Geocentric space-fixed (Mean-of-Date J2000).
    };

    /// @enum RotAngleTypeEnum
    /// This enum represents the reference frame used for the coordinates system.
    enum class RotAngleTypeEnum
    {
        NOT_APPLICABLE,            ///< Geocentric true body-fixed (default).
        LUNAR_EULER_ANGLES,        ///< Lunar Euler angles: (phi, theta, psi).
        RA_DEC                     ///< North pole Right Ascension and Declination, and angle to prime meridian.
    };

    /// @enum TargetDynamicsEnum
    /// This enum represents the target location/dynamics
    enum class TargetDynamicsEnum
    {
        OTHER,                  ///< Other.
        EARTH_ORBIT,            ///< Earth orbit.
        LUNAR_ORBIT,            ///< Lunar orbit.
        LUNAR_SURFACE,          ///< Lunar surface.
        MARS_ORBIT,             ///< Mars orbit.
        MARS_SURFACE,           ///< Mars surface.
        VENUS_ORBIT,            ///< Venus orbit.
        MERCURY_ORBIT,          ///< Mercury orbit.
        ASTEROID_ORBIT,         ///< Asteroid orbit.
        ASTEROID_SURFACE,       ///< Asteroid surface.
        SOLAR_TRANSFER_ORBIT    ///< Solar orbit/transfer orbit (includes fly-by).
    };

    // -----------------------------------------------------------------------------------------------------------------

    // CPF HEADERS STRUCTS
    // -----------------------------------------------------------------------------------------------------------------
    /**
     * @brief H1 - Basic Information 1 Header (required).
     */
    struct BasicInfo1Header : common::ConsolidatedRecord
    {
        // Members and functions.

        float cpf_version;                                  ///< CPF version. We store the subversion, for example 2.1
        std::string cpf_source;                             ///< Ephemeris source.
        timing::types::HRTimePointStd cpf_production_date;  ///< File production date (update if we generate the line).
        std::string target_name;                            ///< Target name from official ILRS list.
        std::string cpf_notes;                              ///< Notes with NO SPACES.
        int cpf_sequence_number;                            ///< Ephemeris sequence number.
        int cpf_subsequence_number;                         ///< Ephemeris sub-daily sequence number.           [For v2]

        /**
         * @brief Generate the line for this record. Also regenerate the cpf_production_date.
         * @return The generated line as string.
         * @todo Implementation pending.
         */
        std::string generateLine();
    };

    /**
     * @brief H2 - Basic Information 2 Header (required).
     */
    struct BasicInfo2Header : common::ConsolidatedRecord
    {
        // Members and functions.

        std::string id;                                 ///< It is always ILRS ID, based on COSPAR ID.
        Optional<std::string> sic;                      ///< SIC provided by ILRS. Set to -1 if target has no SIC.
        std::string norad;                              ///< NORAD ID.
        timing::types::HRTimePointStd start_time;       ///< Ephemeris start time.
        timing::types::HRTimePointStd end_time;         ///< Ephemeris end time.
        std::chrono::seconds total_seconds;             ///< TODO: what is this? non-standard.
        std::chrono::seconds time_between_entries;      ///< Time between two table entries in seconds. 0 if variable.
        bool tiv_compatible;                            ///< Compatible with TIVs.
        TargetClassEnum target_class;                   ///< Target class.
        ReferenceFrameEnum reference_frame;             ///< Reference frame.
        RotAngleTypeEnum rot_angle_type;                ///< Rotation angles type.
        bool com_applied;                               ///< Center of mass correction. True applied, false not applied.
        TargetDynamicsEnum target_dynamics;             ///< Target location/dynamics.                     [only for v2]

        /**
         * @brief Generate the line for this record.
         * @param version Version of the CPF.
         * @return The generated line as string.
         * @todo Implementation pending.
         */
        std::string generateLine(float version) const;
    };

    /**
     * @brief H3 - Expected accuracy (optional).
     * @todo Implementation pending.
     */
    struct ExpectedAccuracyHeader : common::ConsolidatedRecord
    {
        // Members and functions.

        /**
         * @brief Generate the line for this record.
         * @param version Version of the CPF.
         * @return The generated line as string.
         * @todo Implementation pending.
         */
        std::string generateLine(float version) const;
    };

    /**
     * @brief H4 - Transponder information (optional).
     * @todo Implementation pending.
     */
    struct TransponderInfoHeader : common::ConsolidatedRecord
    {
        // Members and functions.

        /**
         * @brief Generate the line for this record.
         * @param version Version of the CPF.
         * @return The generated line as string.
         * @todo Implementation pending.
         */
        std::string generateLine(float version) const;
    };

    /**
     * @brief H5 - Spherical satellite center of mass correction Header (optional).
     */
    struct CoMCorrectionHeader : common::ConsolidatedRecord
    {
        // Members and functions.

        double com_correction;  ///< Approximate center of mass to reflector offset in meters (positive).

        /**
         * @brief Generate the line for this record.
         * @param version Version of the CPF.
         * @return The generated line as string.
         * @todo Implementation pending.
         */
        std::string generateLine(float version) const;
    };
    // -----------------------------------------------------------------------------------------------------------------

    // Constructor.
    /**
     * @brief CPFHeader constructor filling specific version.
     * @param cpf_version
     */
    explicit CPFHeader(float cpf_version);

    M_DEFINE_CTOR_DEF_COPY_MOVE_OP_COPY_MOVE_DTOR(CPFHeader)

    // Clear methods.
    /**
     * @brief Clears all registers.
     */
    void clearAll();
    /**
     * @brief Clears the Basic Info 1 Header.
     */
    void clearBasicInfo1Header();
    /**
     * @brief Clears the Basic Info 2 Header.
     */
    void clearBasicInfo2Header();
    /**
     * @brief Clears the Expected Accuracy Header.
     */
    void clearExpectedAccuracyHeader();
    /**
     * @brief Clears the Transponder Information Header.
     */
    void clearTransponderInfoHeader();
    /**
     * @brief Clears the Center of Mass Correction Header.
     */
    void clearCoMCorrectionHeader();

    // Headers getters.
    const Optional<BasicInfo1Header> &basicInfo1Header() const;
    const Optional<BasicInfo2Header> &basicInfo2Header() const;
    const Optional<ExpectedAccuracyHeader> &expectedAccuracyHeader() const;
    const Optional<TransponderInfoHeader> &transponderInfoHeader() const;
    const Optional<CoMCorrectionHeader> &coMCorrectionHeader() const;

    // Headers getters non const.
    Optional<BasicInfo1Header> &basicInfo1Header();
    Optional<BasicInfo2Header> &basicInfo2Header();
    Optional<ExpectedAccuracyHeader> &expectedAccuracyHeader();
    Optional<TransponderInfoHeader> &transponderInfoHeader();
    Optional<CoMCorrectionHeader> &comCorrectionHeader();

    // Headers setters.
    void setBasicInfo1Header(const BasicInfo1Header&);
    void setBasicInfo2Header(const BasicInfo2Header&);
    void setExpectedAccuracyHeader(const ExpectedAccuracyHeader&);
    void setTransponderInfoHeader(const TransponderInfoHeader&);
    void setCoMCorrection(const CoMCorrectionHeader&);

    /**
     * @brief Generate CPF header lines.
     * @return The generated lines.
     */
    std::string generateHeaderLines();

    // Generic read methods.
    /**
     * @brief Read header records.
     * @param rec_v Vector with header records.
     * @return The errors generated at reading.
     */
    common::RecordReadErrorMultimap readHeader(const common::RecordLinesVector &rec_v);

    // Specific read methods.
    /**
     * @brief Read Basic Info 1 Header H1 from record.
     * @param rec The record to read from.
     * @return The error generated at reading.
     */
    common::RecordReadError readBasicInfo1Header(const common::ConsolidatedRecord &rec);

    /**
     * @brief Read Basic Info 2 Header H2 from record.
     * @param rec The record to read from.
     * @return The error generated at reading.
     */
    common::RecordReadError readBasicInfo2Header(const common::ConsolidatedRecord &rec);

    /**
     * @brief Read Expected Accuracy H3 from record.
     * @param rec The record to read from.
     * @return The error generated at reading.
     */
    common::RecordReadError readExpectedAccuracyHeader(const common::ConsolidatedRecord &rec);

    /**
     * @brief Read Transponder Info H4 from record.
     * @param rec The record to read from.
     * @return The error generated at reading.
     */
    common::RecordReadError readTransponderInfoHeader(const common::ConsolidatedRecord &rec);

    /**
     * @brief Read Center of Mass Correction H5 from record.
     * @param rec The record to read from.
     * @return The error generated at reading.
     */
    common::RecordReadError readCoMCorrectionHeader(const common::ConsolidatedRecord &rec);


private:

    // Generic private read method.
    common::RecordReadError readHeaderLine(const common::RecordLinePair &lpair);

    // H1 (required), H2 (required), H3, H4, H5
    Optional<BasicInfo1Header> basic_info1_header;
    Optional<BasicInfo2Header> basic_info2_header;
    Optional<ExpectedAccuracyHeader> exp_accuracy_header;
    Optional<TransponderInfoHeader> transp_info_header;
    Optional<CoMCorrectionHeader> com_corr_header;
};
// =====================================================================================================================

}}} // END NAMESPACES
// =====================================================================================================================
