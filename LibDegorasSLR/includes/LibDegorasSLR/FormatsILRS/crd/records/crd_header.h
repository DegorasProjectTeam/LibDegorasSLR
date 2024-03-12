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
 * @file crd_header.h
 * @author Degoras Project Team.
 * @brief This file contains the declaration of the CRDHeader class that abstracts the header of ILRS CRD format.
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
#include "LibDegorasSLR/libdegorasslr_global.h"
#include "LibDegorasSLR/Helpers/common_aliases_macros.h"
#include "LibDegorasSLR/Timing/time_utils.h"
#include "LibDegorasSLR/FormatsILRS/common/consolidated_types.h"
#include "LibDegorasSLR/FormatsILRS/common/consolidated_record.h"
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace ilrs{
namespace crd{
// =====================================================================================================================

// CRD HEADER
// =====================================================================================================================

class LIBDPSLR_EXPORT CRDHeader
{
public:

    // CRD CONSTANTS
    // -----------------------------------------------------------------------------------------------------------------
    static const std::array<unsigned, 2> CRDVersions;         /// CRD main versions.
    // -----------------------------------------------------------------------------------------------------------------

    // CRD HEADERS ENUMS
    // -----------------------------------------------------------------------------------------------------------------

    //// @enum HeaderRecordEnum
    //// This enum represents the different types of Header Records.
    enum class HeaderRecord
    {
        FORMAT_HEADER,       ///< Format Header. Line H1 (mandatory)
        STATION_HEADER,      ///< Station Header. Line H2 (mandatory)
        TARGET_HEADER,       ///< Target Header. Line H3 (mandatory)
        SESSION_HEADER,      ///< Session (pass) Header. Line H4 (mandatory)
        PREDICTION_HEADER    ///< Prediction Header. Line H5 (optional). New in v2.
    };

    //// @enum TimeScaleEnum
    //// This enum represents the different types of time scales that can be used in a station.
    /// @note The values 1-2, 5-6, 8-9 are reserved for compatibility with earlier data using obsolete time scales.
    /// @note For Version 1, the value 7 is for BIH; for Version 2, this value is for BIPM.
    enum class TimeScale
    {
        UTC_USNO     = 3,
        UTC_GPS      = 4,
        UTC_BIH_BIPM = 7,
        UTC_STATION  = 10  ///< Station Time Scales. Can be used only with  analysis standing committee (ASC) approval.
    };

    //// @enum SpacecraftEpochTimeScaleEnum
    //// This enum represents the different types of spacecraft epoch time scales.
    /// @note This enum is only used for transponders.
    enum class SpacecraftEpochTimeScale
    {
        NOT_USED        = 0,    ///< Not used.
        UTC             = 1,    ///< Coordinated Universal Time scale.
        SPACECRAFT_TIME = 2     ///< Spacecraft time scale.
    };

    //// @enum TargetClassEnum
    //// This enum represents the different types of tracking targets (space objects).
    /// @note Unfortunately there is no way to identify space debris with LRR using these identifiers.
    /// @note The enum value = 2 is missing because is deprecated.
    enum class TargetClass
    {
        NO_LRR            = 0,    ///< The object does not have laser retroreflectors, like rocket debris. New in v2.
        PASSIVE_LRR       = 1,    ///< The object has laser retroreflectors.
        SYNCH_TRANSPONDER = 3,    ///< The object use a synchronous transponder.
        ASYNC_TRANSPONDER = 4,    ///< The object use an asynchronous transponder.
        OTHER_TARGET      = 5     ///< Other types of unclassified objects. New in v2.
    };

    //// @enum TargetLocationEnum
    //// This enum represents the different locations of tracking targets.
    /// @note This enum is new in CRD Version 2.
    /// @note The -1 value is not allowed in the specification, and it will be written and readed as "na".
    enum class TargetLocation
    {
        UNKNOWN_LOCATION = -1,   ///< The location is unknown (for use when tracking a transponder using a v1 CPF).
        OTHER_LOCATION   = 0,    ///< Another location, different from the following.
        EARTH_ORBIT      = 1,    ///< Earth orbit location.
        LUNAR_ORBIT      = 2,    ///< Lunar orbit location.
        LUNAR_SURFACE    = 3,    ///< Lunar surface location. For LLR.
        MARS_ORBIT       = 4,    ///< Mars orbit location.
        MARS_SURFACE     = 5,    ///< Mars surface location.
        VENUS_ORBIT      = 6,    ///< Venus orbit location.
        MERCURY_ORBIT    = 7,    ///< Mercury orbit location.
        ASTEROID_ORBIT   = 8,    ///< Asteroid orbit location.
        ASTEROID_SURFACE = 9,    ///< Asteroid surface location.
        SOLAR_ORBIT      = 10    ///< Solar orbit location (includes fly-by).
    };

    enum class DataType
    {
        FULL_RATE = 0,            // frd extension
        NORMAL_POINT = 1,         // npt extension
        SAMPLED_ENGINEERING = 2,  // qlk extension (TODO)
        MIXED_DATA = 3            // crd extension (for non ILRS purposes)
    };

    enum class RangeType
    {
        NO_RANGES = 0,
        ONE_WAY = 1,
        TWO_WAY = 2,
        RECEIVE_TIMES = 3,
        MIXED_RANGE = 4
    };

    // Generic data quality enum.
    // Used as generic quality indicator.
    // A more specific indicator can be fount in the statistic record.
    enum class DataQuality
    {
        GOOD = 0,
        SUSPECT = 1,
        POOR_UNKNOWN = 2
    };

    enum class PredictionType
    {
        OTHER = 0,
        CPF = 1,
        TLE = 2
    };
    // -----------------------------------------------------------------------------------------------------------------

    // CRD HEADERS STRUCTS
    // -----------------------------------------------------------------------------------------------------------------
    // H1 - Format Header (required)
    struct FormatHeader : common::ConsolidatedRecord
    {
        // Members.
        float crd_version;                           /// CRD version. We store the subversion, for example 2.1
        timing::types::HRTimePointStd crd_production_date;  /// File production date. Will update if we call generate line.
        // Functions.
        std::string generateLine();                  // Also regenerate the crd_production_date.
    };

    // H2 - Station Header (required)
    struct StationHeader : common::ConsolidatedRecord
    {
        // Members.
        std::string official_name;         /// SLR station official name.
        int cdp_pad_identifier;            /// CDP Pad Identifier for SLR (4 digits).
        int cdp_system_number;             /// CDP System Number for SLR (2 digits).
        int cdp_occupancy_sequence;        /// CDP Occupancy Sequence Number for SLR (2 digits).
        TimeScale epoch_timescale;     /// For us, 7 (UTC BIPM) TODO: Check
        std::string network;               /// "ILRS" or others. Can be empty (print "na" in this case).        [For v2]
        // Functions.
        std::string generateLine(float version) const;
    };

    // H3 - Target Header (required)
    struct TargetHeader : common::ConsolidatedRecord
    {
        // Members.
        std::string name;                          /// ILRS name. For debris usually is made up (SL16-04 eg).
        std::string ilrsid;                        /// ILRS ID from COSPAR ID. Use the helper function to transform.
        std::string sic;                           /// SIC code. Can be empty (v1: print "9999", v2: "na").
        std::string norad;                         /// Catalog number. Can be empty (v1: print "99999999", v2: "na").
        SpacecraftEpochTimeScale sc_epoch_ts;  /// Only for transponders.
        TargetClass target_class;              /// Usually 1 for LRR or 0 for debris without LRR.           [For v2]
        TargetLocation location;               /// Target location/dynamics.                                [For v2]
        std::string cospar;                        /// COSPAR ID. Autogenerated, could be wrong generated.

        // Functions.
        std::string generateLine(float version) const;
    };

    // H4 - Session Header (required)
    struct SessionHeader : common::ConsolidatedRecord
    {
        // Members.
        DataType data_type;             /// Data type (full rate, normal point or sampled engineering).
        timing::types::HRTimePointStd start_time;  /// Object tracking session start time.
        timing::types::HRTimePointStd end_time;    /// Object tracking session end time.
        unsigned data_release;              /// Data release. Remember updating when you edit the CRD data.
        bool trop_correction_applied;       /// Tropospheric refraction correction applied flag.
        bool com_correction_applied;        /// Center of mass correction applied flag.
        bool rcv_amp_correction_applied;    /// Receive amplitude correction applied flag.
        bool stat_delay_applied;            /// Station system delay applied flag.
        bool spcraft_delay_applied;         /// Spacecraft system delay applied flag.
        RangeType range_type;           /// The corrections must match with this flag.
        DataQuality data_quality_alert; /// Data quality alert indicator. Remember include coments "00" if any.
        // Functions.
        std::string generateLine(float version) const;
    };

    // H5 - Predicition Header (only for v2)
    struct PredictionHeader : common::ConsolidatedRecord
    {
        // Members.
        PredictionType prediction_type;                   /// Can be CPF or TLE (forbbiden for ILRS).
        timing::types::HRTimePointStd file_creation_time; /// CPF: MMDDHH ; TLE: Field 8 (Epoch).
        std::string prediction_provider;                  /// CPF or TLE provider
        unsigned sequence_number;                         /// CPF: daily*100+sub-daily TLE: Field 9 (epoch revolution).
        // Functions.
        std::string generateLine(float version) const;
    };
    // -----------------------------------------------------------------------------------------------------------------

    // Constructor.
    CRDHeader(float crd_version);
    CRDHeader();

    // Copy and movement constructors and assign operators
    CRDHeader(const CRDHeader&) = default;
    CRDHeader(CRDHeader&&) = default;
    CRDHeader& operator = (const CRDHeader&) = default;
    CRDHeader& operator = (CRDHeader&&) = default;

    // Destructor.
    ~CRDHeader() = default;

    // Clear methods.
    void clearAll();
    void clearFormatHeader();
    void clearStationHeader();
    void clearTargetHeader();
    void clearSessionHeader();
    void clearPredictionHeader();

    // Headers getters.
    const Optional<FormatHeader>& formatHeader() const;
    const Optional<StationHeader>& stationHeader() const;
    const Optional<TargetHeader>& targetHeader() const;
    const Optional<SessionHeader>& sessionHeader() const;
    const Optional<PredictionHeader>& predictionHeader() const;

    // Headers getters non const.
    Optional<FormatHeader>& formatHeader();
    Optional<StationHeader>& stationHeader();
    Optional<TargetHeader>& targetHeader();
    Optional<SessionHeader>& sessionHeader();
    Optional<PredictionHeader>& predictionHeader();

    // Headers setters.
    void setStationHeader(const StationHeader&);
    void setTargetHeader(const TargetHeader&);
    void setSessionHeader(const SessionHeader&);
    void setPredictionHeader(const PredictionHeader&);
    void setFormatHeader(const FormatHeader&);              // It is not neccesary to use it, usually is autogenerated.

    // Generate CRD header lines.
    std::string generateHeaderLines(bool include_format_header);

    // Generic read methods.
    common::RecordReadErrorMultimap readHeader(const common::RecordLinesVector& rec_v);

    // Specific read methods.
    // TODO Mover esto y aplicar patrón factoría.
    common::RecordReadError readFormatHeader(const common::ConsolidatedRecord&);      // H1
    common::RecordReadError readStationHeader(const common::ConsolidatedRecord&);     // H2
    common::RecordReadError readTargetHeader(const common::ConsolidatedRecord&);      // H3
    common::RecordReadError readSessionHeader(const common::ConsolidatedRecord&);     // H4
    common::RecordReadError readPredictionHeader(const common::ConsolidatedRecord&);  // H5


private:

    // Generic private read method.
    common::RecordReadError readHeaderLine(const common::RecordLinePair& lpair);

    // H1, H2, H3, H4 (required), H5
    Optional<FormatHeader> format_header;
    Optional<StationHeader> station_header;
    Optional<TargetHeader> target_header;
    Optional<SessionHeader> session_header;
    Optional<PredictionHeader> prediction_header;
};
// =====================================================================================================================

}}} // END NAMESPACES
// =====================================================================================================================

