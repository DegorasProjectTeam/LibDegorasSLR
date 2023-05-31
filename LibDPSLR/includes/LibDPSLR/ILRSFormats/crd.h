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

/** ********************************************************************************************************************
 * @file class_crd.h
 *
 * @brief This file contains an abstraction of Consolidated Data Format (CRD), for version 1 and 2, currently.
 *
 * This library is an abstraction of Consolidated Data Format (CRD), for version 1 and 2, currently. This class permits
 * opening an existing CRD file and editing, or creating new files with raw data. During the creation, the full rate data
 * and the normal point data will coexist in this abstraction, but due to ILRS specification, both types of data will
 * be written in different files (frd and npt). This implementation also permits some less restrictive functionalities
 * as convenient file naming generation using NORAD, mixed files generation, and others.
 *
 * About the data processing, the system contains methods for calculating the statistics data, the overall calibrations,
 * and the normal points [TODO]. Also the system permits calculating all this data by external methods, and including
 * this data using setters. For external calculations you can also use our mathematical and helper methods in the
 * namespaces "dpslr::utils" and "dpslr::math".
 *
 * Also, the library contains methods for checking the consistency and integrity of all the introduced data [TODO],
 * and for including comment records (lines "00") if necessary for clarity or to indicate important things.
 *
 * With the CRD class you can manage only CRD files with a single pass, with only one header and configuration block
 * and the data block for a single pass. This is the basic functionality for the normal work of the ILRS. For data
 * centers and others, you have the MultiCRD class, that can manage files with multiple passes and data [TODO].
 *
 * The "CRD Version 1" specification can be found in: https://ilrs.gsfc.nasa.gov/docs/2009/crd_v1.01.pdf
 * The "CRD Version 2" specification can be found in: https://ilrs.gsfc.nasa.gov/docs/2019/crd_v2.01.pdf
 *
 * @author    Degoras Project Team
 * @version   2011.1
 * @date      12-11-2020
 * @copyright EUPL License.
 *
 * @par IMPORTANT NOTE:
 *
 * This CRD code is provided "as is" with no guarantees. The software has been tested with a limited amount of data,
 * and there may still be errors and incomplete implementation of standards. Due to the own CRD format inconsistencies
 * (for both versions), we have made some implementation decisions which are not covered or completely defined by the
 * format specification document.
 *
 * We have compared several CRD files from different stations, so we can cover the most common situations for the SLR
 * and LLR stations for both versions. However, some stations could find some issues at reading their files, so we
 * recommend checking all the read and written data by comparing original files with the ones created by our library.
 * Also, some stations could find some issues at reading the files created by our library with their programs or
 * libraries, specially if those are based in the old fortran/c/c++ ILRS codes.
 *
 * We use experimental functionalities from C++14, like the "optional class". We cannot use C++17 or higher because we
 * need crosscompiling this library with gcc Linaro for our RGG working in a Cyclone V SoC. However, C++14 is enough.
 *
 * @par ABOUT LINES GENERATION ORDER:
 *
 * Our implementation does not order the lines by time points, except for the full rate records [TODO] and normal points
 * records. We prefer (for more clarity and visibility) putting, for example, all the calibrations lines together, and
 * the same with other lines, like pointing angle records [TODO], meteorological records, etc. So, the final order is:
 *
 * H1, H2, H3, H4, H5, C0, C1, C2, C3, C4, C5, C6, C7, [20], [41], [40RT], 40 overall, 50, [10] or [11], [30], H8, H9
 *
 * @par ABOUT ERROR HANDLING:
 *
 * There are three posibilities when reading a CRD file:
 * - The CRD file is read with no errors.
 * - The CRD file has some faulty records. In this case, the read operation gives a warning code, but the correct
 *   records are stored. In addition, the faulty records are stored at its corresponding struct (Header, Data or
 *   Cfg), so you can check them.
 * - The CRD file has some fatal errors, as lacking some required records or having an incorrect order. In this case,
 *   the CRD object is cleared, and the record responsible for the error is stored so you can check it.
 *
 * @par IMPLEMENTATION DECISIONS:
 *
 * In this implementation, after reviewing different CRD files from several stations, we have made some changes or
 * additions in order to be more flexible and avoid incoherencies or inconsistencies. This changes should not affect
 * the correct processing of the files and the data. These are:
 *
 * - For security, you can't generate the frd with the date you want. The real time clock will be used for the
 *   production date. If you open a CRD and edit the data, the original production date will also be updated.
 *
 * - The blank lines will be skipped while reading. The file will always be written without blank lines.
 *
 * - You can write and read float CRD format version numbers (for example 2.01). This may be useful for internal use or
 *   for the future (if Version 2 keeps updating for example).
 *
 * - For writing, all the floating point numeric data will be normalized based on their specification. However, except
 *   for the corresponding records for Version 1, all the data is written as free format, so the old columnar format
 *   can't be used. Numbers with greater precision than indicated by the specification will be truncated.
 *
 * - About the file extensions, we saw in EUROLAS that for Version 2 the extensions ".fr2" and ".np2" are being used,
 *   so, in order to make a distinction between file versions (including in our own file storage), we implement the use
 *   of these extensions.
 *
 * - Lines 60 are obsolete, and they are ignored for this implementation (also for the Version 1).
 *
 * - The comment line will be trunctated if number of characters > 80. If you need to write more characters, you can
 *   add more comment lines without problem. The limitation of 40 characters for the rest of the data is checked in the
 *   check integrity function [TODO]. The comment block will always precede the associated record line. Also, can be
 *   in the file blank comments ("00").
 *
 * - For optional values for Version 1, if we read a field as "na" or -1, the system will store the data without
 *   value (even if the first case does not fit the format).
 *
 * - In the Target Header, in the field "ilrsid", the library can read (for both versions) as target identifier the
 *   ILRS ID (8606101), the COSPAR (1986-061A) or the SHORT COSPAR (86061A) automatically. As habitual behavior, for
 *   file data writing, the system will always write the ILRS ID in the field. You can force the writing of other
 *   identifier ussing the struct modifiers.
 *
 * - In the Session Header, for the ending time data, the "na" values used for real time writting is not supported,
 *   since that does not make sense in our implementation. For real time, you can store the data in memory using
 *   the structs, and when the pass is over you can store the file.
 *
 * - We understand that the components fields in System Configuration Record are in order of appeareance. If a
 *   configuration is missing, it will not appear (it will never be written as "na" or blank space). When reading, all the
 *   components vaules are ignored, since the system configuration record is autogenerated using the rest of the
 *   configuration lines.
 *
 * - In Detector Configuration Record, the last 3 fields in v2 can be "na". However, for reading, if the last field
 *   value is "na" or 0, the other two will be stored without value, and printed as "na". If the last field value is 1,
 *   and the other values are 0.0, those fields will also be 0.0. The spatial filter field has been allowed to be
 *   optional, since in our station it is unknown.
 *
 * - In the Full Rate Record, for Version 1, if receive amplitude is 0 when reading a file, it will be stored without value
 *   and printed as 0. For Version 2, if receive amplitude is 0 or "na" when reading, it will be stored without value, and
 *   printed as "na".
 *
 * - In the Normal Point Record, for Version 1, the return rate field can be the SNR for LLR. We will store the value in
 *   all cases in the field "return_rate" for this version. The Version 2 does not have this problem as the SNR
 *   is stored separately.
 *
 * - In the Normal Point Record, for Version 2, we saw that several stations have the SNR field as optional, so we
 *   have implemented it as optional too. So, if SNR is "na" when reading a file, it will stored without value and printed
 *   as "na". We will consider the value 0 as a valid value, so it will be stored as 0 and printed as 0.
 *
 * - In the Normal Point Record, the rms, skew, kurtosis and peak will be implented as optionals.
 *   So, for v1 if the read value is -1, it will be stored without value, and printed as -1. For v2 if the read
 *   value is "na", it will be stored without value, and printed as "na". For v2 if the read value is 0 or -1,
 *   it will be considered a valid value, so it will be stored and printed as 0 or -1. Only rms, skew and kurtosis are
 *   explicitly mentioned as optional in the standard, but we will implement peak as well, since other stations use it.
 *
 * - In the Calibration Record, for Version 2, we saw that several stations have the return rate field as optional,
 *   so we have implemented this functionality. So, if the file reading return rate is "na", it will be stored without
 *   value and printed as "na". The value 0 is valid, so it will be stored as 0 and printed as 0.
 *
 * - In the Calibration Record, for Version 2, we saw that some stations perform the summatory of the data point used
 *   and recorded for the overall calibration (line 40), while other stations only copy the data of the
 *   precalibration or postcalibration. The latter does not make sense, so we have implemented the first one.
 *
 * - For the Overall Calibration Record generation, time tag will be calculated as the time in the middle between
 *   start time and end time in Session Header Record. In some stations, this time tag is calculated as the mean between
 *   pre and post calibration, though it is not what the specification states.
 *
 * - In Statistics Record, the skew, kurtosis and peak will be implemented as optionals. So, for v1 if the read value
 *   is -1, it will be stored without value, and printed as -1. For v2 if the read value is "na", it will be stored
 *   without value, and printed as "na". For v2 if the read value is 0 or -1, it will be considered a valid value,
 *   so it will be stored and printed as 0 or -1.
 *
 * @par LIBRARY USAGE DECISIONS IN SFEL (ROA) STATION
 *
 * For the library usage for our SFEL station, we have considered the discrepancies between the CRD format specification
 * and the debris or lunar tracking, and other problems associated to the format. So, for the library usage for our
 * station we have taken the following decisions:
 *
 * + For all versions:
 *
 *  - The name of the files that we will store in our own database will always use the NORAD as target identifier,
 *    instead of using the ILRS ID. This decision make the naming more universal and valid for debris target. However,
 *    the files that we will send to the ILRS datacenters will use the ILRS ID, following the convention.
 *
 *  - For all debris tracking, the generated filename will include a preamble, although officially the format does not
 *    allow it for Version 1 (after all, one day this version will be deprecated), like the network (different to
 *    "ILRS") or other [TODO].
 *
 * + For Version 1:
 *
 *  - ILRS satellite debris tracking with laser retro-reflectors (LRR) can't be completely determined in this version,
 *    so we will add a comment record after the header records saying "ILRS LRR SATELLITE DEBRIS TRACKING" [TODO].
 *
 *  - Non ILRS debris tracking (without LRR) can't be correctly defined. The SIC code will be "9999", and the ILRS ID
 *    will be created using the transformation helper function using the debris COSPAR ID (it can fail). The target
 *    class will be 0 (no LRR), although officially the format does not allow it. We will also add a comment record
 *    after the header records saying "NO LRR DEBRIS TRACKING" [TODO].
 *
 *  - Lunar trackings can't be completely determined in this version, so we will add a comment record after the header
 *    records saying "LLR TRACKING". The target class will be 1, and the NORAD "99999999" [TODO].
 *
 * + For Version 2:
 *
 *  - ILRS satellite debris tracking with laser retro-reflectors (LRR) can't be completely determined in this version,
 *    so we will add a comment record after the header records saying "ILRS LRR SATELLITE DEBRIS TRACKING". The target
 *    class will be 1 (passive LRR), and the station network will be different to "ILRS" [TODO].
 *
 *  - For non ILRS debris tracking (without LRR), the SIC code will be "na", and the ILRS ID will be replaced by the
 *    full COSPAR ID. The target class will be 0 . We will also add a comment record after the header records saying
 *    "NO LRR DEBRIS TRACKING" [TODO].
 *
 *  - For Lunar trackings we will add a comment record after the header records saying "LLR TRACKING". The target class
 *    will be 1, the target location will be 3 (lunar surface), and the NORAD "na" [TODO].
 *
 * @par CRD FILE CREATION PROCESS (FULL RATE + NORMAL POINT):
 *
 * To create and write CRD files using this library, the following steps should be followed:
 *
 *  1. Create a CRD instance specifying the version.
 *  2. Create the Header Records externally, and insert them.
 *  3. Create the Configuration Records externally, and insert them.
 *  4. Create the Meteorological Records externally, and insert them.
 *  5. Generate the interpolated Meteorological Records if neccesary:
 *     - Calculate externally and insert the structs.
 *  6. Create the detailed Calibration Records externally, and insert them.
 *  7. [Version 2] Create the Real Time Calibration Records externally if neccesary, and insert them.
 *  8. [Version 2] Generate the overall calibration record. You have two options:
 *     - Calculate externally and insert it.
 *     - Calculate and generate the struct using the internal methods.
 *  9. Create all Full Rate Records externally and insert them.
 * 10. Generate the Statistics Record using the full rate data (raw data) if neccesary. You have two options:
 *     - Calculate externally and insert the struct.
 *     - Calculate and generate the struct using the internal methods. [TODO]
 * 11. Generate the Normal Point Records if neccesary. You have two options:
 *     - Calculate externally and insert the structs (for example using "orbitnp.py").
 *     - Calculate and generate the struct using the internal methods. [TODO]
 * 12. Check the integrity of the data using the checking functions if neccesary [TODO].
 * 13. Write the ".frd" or ".npt" (for v1) or ".fr2" or ".np2" (for v2) files (or both) with all the necessary data.
 *     You have two options:
 *     - Get the lines and process or store externally.
 *     - Write the file with the internal methods.
 *
 * Note: In all momment you can edit the structs data, add comments, etc.
 *
 * @par CRD FILE OPENING AND EDITING PROCESS:
 *
 * To open and edit CRD files using this library, the following steps should be followed:
 *
 * 1. Create an empty CRD and open all the data, or directly create a CRD using the reading file constructor. You must
 *    pass the file path to the latter. The function will automatically store in memory all the structs with the
 *    corresponding data and it will generate the read errors if any.
 * 2. Edit the data you want.
 * 3. Remember to update the release number if necessary (data release in Session Header). If you do not edit the number,
 *    then, the autogenerated standard filename will be the same as the opened file, so the original file will be
 *    replaced if the new file is saved in the same directory.
 * 4. Check the integrity of the data using the check functions if necessary [TODO].
 * 5. Write the ".frd" or ".npt" (for v1) or ".fr2" or ".np2" (for v2) file with the corresponding data.
 *    You have two options:
 *    - Get the lines and process or store externally.
 *    - Write the file with the internal methods.
 *
 **********************************************************************************************************************/


// =====================================================================================================================
// TODO:
// - Lines C4, C5, C7, 12, 21, 30, 42
// - Maybe use variants (without c++17) in the future for simplifying the implementation.
// - Normal points internal calculations.
// - Meteorological data internal interpolation.
// =====================================================================================================================


// =====================================================================================================================
#pragma once
// =====================================================================================================================


// ========== LOCAL INCLUDES ===========================================================================================
#include "libdpslr_global.h"
#include "helpers.h"
#include "common.h"
// =====================================================================================================================


// ========== CRD HEADER ===============================================================================================
class LIBDPSLR_EXPORT CRDHeader
{
public:

    // CRD HEADERS LINES
    // -----------------------------------------------------------------------------------------------------------------
    static const std::array<std::string, 5> HeaderLineString;   // String header records identifiers.
    static const std::array<int, 2> CRDVersions;                // Add new main versions here.
    // -----------------------------------------------------------------------------------------------------------------

    // CRD HEADERS ENUMS
    // -----------------------------------------------------------------------------------------------------------------

    //// @enum HeaderRecordEnum
    //// This enum represents the different types of Header Records.
    enum class HeaderRecordEnum
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
    enum class TimeScaleEnum
    {
        UTC_USNO     = 3,
        UTC_GPS      = 4,
        UTC_BIH_BIPM = 7,
        UTC_STATION  = 10  ///< Station Time Scales. Can be used only with  analysis standing committee (ASC) approval.
    };

    //// @enum SpacecraftEpochTimeScaleEnum
    //// This enum represents the different types of spacecraft epoch time scales.
    /// @note This enum is only used for transponders.
    enum class SpacecraftEpochTimeScaleEnum
    {
        NOT_USED        = 0,    ///< Not used.
        UTC             = 1,    ///< Coordinated Universal Time scale.
        SPACECRAFT_TIME = 2     ///< Spacecraft time scale.
    };

    //// @enum TargetClassEnum
    //// This enum represents the different types of tracking targets (space objects).
    /// @note Unfortunately there is no way to identify space debris with LRR using these identifiers.
    /// @note The enum value = 2 is missing because is deprecated.
    enum class TargetClassEnum
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
    enum class TargetLocationEnum
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


    enum class DataTypeEnum
    {
        FULL_RATE = 0,            // frd extension
        NORMAL_POINT = 1,         // npt extension
        SAMPLED_ENGINEERING = 2,  // qlk extension (TODO)
        MIXED_DATA = 3            // crd extension (for non ILRS purposes)
    };




    enum class RangeTypeEnum
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
    enum class DataQualityEnum
    {
        GOOD = 0,
        SUSPECT = 1,
        POOR_UNKNOWN = 2
    };

    enum class PredictionTypeEnum
    {
        OTHER = 0,
        CPF = 1,
        TLE = 2
    };
    // -----------------------------------------------------------------------------------------------------------------

    // CRD HEADERS STRUCTS
    // -----------------------------------------------------------------------------------------------------------------
    // H1 - Format Header (required)
    struct FormatHeader : dpslr::common::ConsolidatedRecordStruct
    {
        // Members.
        float crd_version;                            // CRD version. We store the subversion, for example 2.1
        dpslr::common::HRTimePoint crd_production_date;     // File production date. Will update if we call generate line.
        // Functions.
        std::string generateLine();                   // Also regenerate the crd_production_date.
    };

    // H2 - Station Header (required)
    struct StationHeader : dpslr::common::ConsolidatedRecordStruct
    {
        // Members.
        std::string official_name;          // SLR station official name.
        int cdp_pad_identifier;            // CDP Pad Identifier for SLR (4 digits).
        int cdp_system_number;             // CDP System Number for SLR (2 digits).
        int cdp_occupancy_sequence;        // CDP Occupancy Sequence Number for SLR (2 digits).
        TimeScaleEnum epoch_timescale;     // For us, 7 (UTC BIPM) TODO: Check
        std::string network;               // "ILRS" or others. Can be empty (print "na" in this case).         [For v2]
        // Functions.
        std::string generateLine(float version) const;
    };

    // H3 - Target Header (required)
    struct TargetHeader : dpslr::common::ConsolidatedRecordStruct
    {
        // Members.
        std::string name;                          // ILRS name. For debris usually is made up (SL16-04 eg).
        std::string ilrsid;                        // ILRS ID from COSPAR ID. Use the helper function to transform.
        std::string sic;                           // SIC code. Can be empty (v1: print "9999", v2: "na").
        std::string norad;                         // Catalog number. Can be empty (v1: print "99999999", v2: "na").
        SpacecraftEpochTimeScaleEnum sc_epoch_ts;  // Only for transponders.
        TargetClassEnum target_class;              // Usually 1 for LRR or 0 for debris without LRR.            [For v2]
        TargetLocationEnum location;               // Target location/dynamics.                                 [For v2]
        std::string cospar;                        // COSPAR ID. Autogenerated, could be wrong.               [Auxiliar]

        // Functions.
        std::string generateLine(float version) const;
    };

    // H4 - Session Header (required)
    struct SessionHeader : dpslr::common::ConsolidatedRecordStruct
    {
        // Members.
        DataTypeEnum data_type;             // Data type (full rate, normal point or sampled engineering).
        dpslr::common::HRTimePoint start_time;    // Object tracking session start time.
        dpslr::common::HRTimePoint end_time;      // Object tracking session end time.
        unsigned int data_release;          // Data release. Remember updating when you edit the CRD data.
        bool trop_correction_applied;       // Tropospheric refraction correction applied flag.
        bool com_correction_applied;        // Center of mass correction applied flag.
        bool rcv_amp_correction_applied;    // Receive amplitude correction applied flag.
        bool stat_delay_applied;            // Station system delay applied flag.
        bool spcraft_delay_applied;         // Spacecraft system delay applied flag.
        RangeTypeEnum range_type;           // The corrections must match with this flag.
        DataQualityEnum data_quality_alert; // Data quality alert indicator. Remember include coments "00" if any.
        // Functions.
        std::string generateLine(float version) const;
    };

    // H5 - Predicition Header (only for v2)
    struct PredictionHeader : dpslr::common::ConsolidatedRecordStruct
    {
        // Members.
        PredictionTypeEnum prediction_type;       // Can be CPF or TLE (forbbiden for ILRS).
        dpslr::common::HRTimePoint file_creation_time;  // CPF: MMDDHH              TLE: Field 8 (Epoch)
        std::string prediction_provider;          // CPF or TLE provider
        unsigned int sequence_number;             // CPF: daily*100+sub-daily TLE: Field 9 (Revolution number at epoch)
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
    const dpslr::common::optional<FormatHeader> &formatHeader() const;
    const dpslr::common::optional<StationHeader> &stationHeader() const;
    const dpslr::common::optional<TargetHeader> &targetHeader() const;
    const dpslr::common::optional<SessionHeader> &sessionHeader() const;
    const dpslr::common::optional<PredictionHeader> &predictionHeader() const;

    // Headers getters non const.
    dpslr::common::optional<FormatHeader> &formatHeader();
    dpslr::common::optional<StationHeader> &stationHeader();
    dpslr::common::optional<TargetHeader> &targetHeader();
    dpslr::common::optional<SessionHeader> &sessionHeader();
    dpslr::common::optional<PredictionHeader> &predictionHeader();

    // Headers setters.
    void setStationHeader(const StationHeader&);
    void setTargetHeader(const TargetHeader&);
    void setSessionHeader(const SessionHeader&);
    void setPredictionHeader(const PredictionHeader&);
    void setFormatHeader(const FormatHeader&);                  // It is not neccesary to use it. Usually autogenerated.

    // Generate CRD header lines.
    std::string generateHeaderLines(bool include_format_header);

    // Generic read methods.
    dpslr::common::RecordReadErrorMultimap readHeader(const dpslr::common::RecordLinesVector &rec_v);

    // Specific read methods.
    dpslr::common::RecordReadErrorEnum readFormatHeader(const dpslr::common::ConsolidatedRecordStruct&);      // H1
    dpslr::common::RecordReadErrorEnum readStationHeader(const dpslr::common::ConsolidatedRecordStruct&);     // H2
    dpslr::common::RecordReadErrorEnum readTargetHeader(const dpslr::common::ConsolidatedRecordStruct&);      // H3
    dpslr::common::RecordReadErrorEnum readSessionHeader(const dpslr::common::ConsolidatedRecordStruct&);     // H4
    dpslr::common::RecordReadErrorEnum readPredictionHeader(const dpslr::common::ConsolidatedRecordStruct&);  // H5


private:

    // Generic private read method.
    dpslr::common::RecordReadErrorEnum readHeaderLine(const dpslr::common::RecordLinePair &lpair);

    // H1, H2, H3, H4 (required), H5
    dpslr::common::optional<FormatHeader> format_header;
    dpslr::common::optional<StationHeader> station_header;
    dpslr::common::optional<TargetHeader> target_header;
    dpslr::common::optional<SessionHeader> session_header;
    dpslr::common::optional<PredictionHeader> prediction_header;
};
// =====================================================================================================================


// ========== CRD CONFIGURATION ========================================================================================
class LIBDPSLR_EXPORT CRDConfiguration
{
public:

    // CRD CONFIGURATION LINES
    // -----------------------------------------------------------------------------------------------------------------
    static const std::array<const char*, 8> CfgLineString;
    // -----------------------------------------------------------------------------------------------------------------

    // CRD CONFIGURATION ENUMS
    // -----------------------------------------------------------------------------------------------------------------
    enum class ConfigurationLineEnum
    {
        SYSTEM_CFG,          // C0 (mandatory)
        LASER_CFG,           // C1 (mandatory)
        DETECTOR_CFG,        // C2 (mandatory)
        TIMING_CFG,          // C3 (mandatory)
        TRANSPONDER_CFG,     // C4 (optional)
        SOFTWARE_CFG,        // C5 (optional)
        METEOROLOGICAL_CFG,  // C6 (optional)
        CALIBRATION_CFG      // C7 (optional)
    };
    // ----------------------------------------------------------------------------------------------------------------

    // CRD CONFIGURATION STRUCTS
    // -----------------------------------------------------------------------------------------------------------------
    // C0 - System Configuration (required)
    struct SystemConfiguration : dpslr::common::ConsolidatedRecordStruct
    {
        // Members.
        unsigned int detail_type;
        double transmit_wavelength;  // Nanometers.
        std::string system_cfg_id;
        // A, B, C... dynamically generated using all IDs of other configuration structs.
    };

    // C1 - Laser Configuration Record (required)
    struct LaserConfiguration : dpslr::common::ConsolidatedRecordStruct
    {
        // Members.
        unsigned int detail;                     // Always 0
        std::string cfg_id;                      // Laser configuration id (NL317, PL2251)
        std::string type;                        // "Nd-Yag", ...
        double primary_wavelength;               // nm
        double fire_rate;                        // Hz
        double pulse_energy;                     // mj
        double pulse_width;                      // ps (FWHM)
        double beam_divergence;                  // arcs (full angle)
        unsigned int pulses_outgoing_semitrain;  // For multi-pulse systems. (TODO: ?)
        // Functions.
        std::string generateLine(float version) const;
    };

    // C2 - Detector Configuration (required)
    struct DetectorConfiguration : dpslr::common::ConsolidatedRecordStruct
    {
        // Members.
        unsigned int detail;                        // Always 0.
        std::string cfg_id;                         // Detector configuration ID
        std::string type;                           // SPAD, CSPAD, MCP, APD, ...
        double work_wavelength;                     // nm
        double efficiency;                          // %
        double voltage;                             // V
        double dark_count;                          // kHz
        std::string out_pulse_type;                 // ECL, TTL, photon-dependent, ...
        double out_pulse_width;                     // ps
        double spectral_filter;                     // nm
        double transm_spectral_filter;              // %
        dpslr::common::optional<double> spatial_filter;   // arcsec
        std::string ext_signal_proc;                      // TODO: ?
        dpslr::common::optional<double> amp_gain;         // TODO: ? db?                             [For v2] [optional]
        dpslr::common::optional<double> amp_bandwidth;    // kHz.                                    [For v2] [optional]
        dpslr::common::optional<bool> used_amp;           // Used amplificator flag.                 [For v2] [optional]
        // Functions.
        std::string generateLine(float version) const;
    };

    // C3 - Timing Configuration (required)
    struct TimingConfiguration : dpslr::common::ConsolidatedRecordStruct
    {
        // Members.
        unsigned int detail;            // Always 0.
        std::string cfg_id;             // Timing configuration ID
        std::string time_source;        // GPS, maser roa utc, ...
        std::string frequency_source;   // Rubidium, SYMMETRICOM, ...
        std::string timer;              // SR620, ETA033, ...
        std::string timer_serial;       // Serial
        double epoch_delay;             // Time origin delay (us)
        // Functions.
        std::string generateLine(float version) const;
    };

    // C4 - Trnasponder Configuration TODO
    struct TransponderConfiguration : dpslr::common::ConsolidatedRecordStruct
    {
        // Members.
        unsigned int detail;            // Always 0.
        std::string cfg_id;             // Transponder configuration ID
        // Functions.
        std::string generateLine(float version) const;
    };

    // C5 - Software Configuration TODO
    struct SoftwareConfiguration : dpslr::common::ConsolidatedRecordStruct
    {
        // Members.
        unsigned int detail;            // Always 0
        std::string cfg_id;             // Software configuration ID
        // Functions.
        std::string generateLine(float version) const;
    };

    // C6 - Meteorological Configuration
    struct MeteorologicalConfiguration : dpslr::common::ConsolidatedRecordStruct
    {
        // Members.
        unsigned int detail;            // Always 0
        std::string cfg_id;             // Meteorological configuration ID
        // TODO: fields optional?
        std::string press_manufacturer; // Pressure sensor manufacturer
        std::string press_model;        // Pressure sensor model
        std::string press_sn;           // Pressure sensor serial number
        std::string temp_manufacturer;  // Temperature sensor manufacturer
        std::string temp_model;         // Temperature sensor model
        std::string temp_sn;            // Temperature sensor serial number
        std::string humid_manufacturer; // Humidity sensor manufacturer
        std::string humid_model;        // Humidity sensor model
        std::string humid_sn;           // Humidity sensor serial number
        // Functions.
        std::string generateLine(float version) const;
    };

    // C7 - Calibration Configuration TODO
    struct CalibrationConfiguration : dpslr::common::ConsolidatedRecordStruct
    {
        // Members.
        unsigned int detail;            // Always 0
        std::string cfg_id;             // Calibration configuration ID
        //Functions.
        std::string generateLine(float version) const;
    };
    // -----------------------------------------------------------------------------------------------------------------

    // Constructor.
    CRDConfiguration() = default;

    // Copy and movement constructors and assign operators
    CRDConfiguration(const CRDConfiguration&) = default;
    CRDConfiguration(CRDConfiguration&&) = default;
    CRDConfiguration& operator = (const CRDConfiguration&) = default;
    CRDConfiguration& operator = (CRDConfiguration&&) = default;

    // Destructor.
    ~CRDConfiguration() = default;

    // Clear methods.
    void clearAll();
    void clearSystemConfiguration();
    void clearLaserConfiguration();
    void clearDetectorConfiguration();
    void clearTimingConfiguration();
    void clearTransponderConfiguration();
    void clearSoftwareConfiguration();
    void clearMeteorologicalConfiguration();
    void clearCalibrationConfiguration();

    // Configuration const getters.
    const dpslr::common::optional<SystemConfiguration> &systemConfiguration() const;
    const dpslr::common::optional<LaserConfiguration> &laserConfiguration() const;
    const dpslr::common::optional<DetectorConfiguration> &detectorConfiguration() const;
    const dpslr::common::optional<TimingConfiguration> &timingConfiguration() const;
    const dpslr::common::optional<TransponderConfiguration> &transponderConfiguration() const;
    const dpslr::common::optional<SoftwareConfiguration> &softwareConfiguration() const;
    const dpslr::common::optional<MeteorologicalConfiguration> &meteorologicalConfiguration() const;
    const dpslr::common::optional<CalibrationConfiguration> &calibrationConfiguration() const;

    // Configuration no const getters.
    dpslr::common::optional<SystemConfiguration> &systemConfiguration();
    dpslr::common::optional<LaserConfiguration> &laserConfiguration();
    dpslr::common::optional<DetectorConfiguration> &detectorConfiguration();
    dpslr::common::optional<TimingConfiguration> &timingConfiguration();
    dpslr::common::optional<TransponderConfiguration> &transponderConfiguration();
    dpslr::common::optional<SoftwareConfiguration> &softwareConfiguration();
    dpslr::common::optional<MeteorologicalConfiguration> &meteorologicalConfiguration();
    dpslr::common::optional<CalibrationConfiguration> &calibrationConfiguration();

    // Configuration setters.
    void setSystemConfiguration(const SystemConfiguration&);
    void setLaserConfiguration(const LaserConfiguration&);
    void setDetectorConfiguration(const DetectorConfiguration&);
    void setTimingConfiguration(const TimingConfiguration&);
    void setTransponderConfiguration(const TransponderConfiguration&);
    void setSoftwareConfiguration(const SoftwareConfiguration&);
    void setMeteorologicalConfiguration(const MeteorologicalConfiguration&);
    void setCalibrationConfiguration(const CalibrationConfiguration&);

    // Generate CRD configuration lines.
    std::string generateConfigurationLines(float version) const;

    // Generic read methods.
    dpslr::common::RecordReadErrorMultimap readConfiguration(const dpslr::common::RecordLinesVector&, float version);

    // Specific read methods.
    dpslr::common::RecordReadErrorEnum readSystemCFG(const dpslr::common::ConsolidatedRecordStruct&, float version);       // C0
    dpslr::common::RecordReadErrorEnum readLaserCFG(const dpslr::common::ConsolidatedRecordStruct&, float version);        // C1
    dpslr::common::RecordReadErrorEnum readDetectorCFG(const dpslr::common::ConsolidatedRecordStruct&, float version);     // C2
    dpslr::common::RecordReadErrorEnum readTimingCFG(const dpslr::common::ConsolidatedRecordStruct&, float version);       // C3
    dpslr::common::RecordReadErrorEnum readTransponderCFG(const dpslr::common::ConsolidatedRecordStruct&, float version);  // C4
    dpslr::common::RecordReadErrorEnum readSoftwareCFG(const dpslr::common::ConsolidatedRecordStruct&, float version);     // C5
    dpslr::common::RecordReadErrorEnum readMeteoCFG(const dpslr::common::ConsolidatedRecordStruct&, float version);        // C6
    dpslr::common::RecordReadErrorEnum readCalibrationCFG(const dpslr::common::ConsolidatedRecordStruct&, float version);  // C7

private:

    // Generic private read method.
    dpslr::common::RecordReadErrorEnum readConfigurationLine(const dpslr::common::RecordLinePair &rpair, float version);

    // Private method to generate A, B, C... of C0 line.
    std::string generateSystemCfgLine(float version) const;

    // Storage CRD configuration data. ( C0 to C7 )
    dpslr::common::optional<SystemConfiguration> system_cfg;
    dpslr::common::optional<LaserConfiguration> laser_cfg;
    dpslr::common::optional<DetectorConfiguration> detector_cfg;
    dpslr::common::optional<TimingConfiguration> timing_cfg;
    dpslr::common::optional<TransponderConfiguration> transponder_cfg;
    dpslr::common::optional<SoftwareConfiguration> software_cfg;
    dpslr::common::optional<MeteorologicalConfiguration> meteorological_cfg;
    dpslr::common::optional<CalibrationConfiguration> calibration_cfg;
};
// =====================================================================================================================


// ========== CRD Data =================================================================================================
class LIBDPSLR_EXPORT CRDData
{
public:

    // CRD DATA LINES
    // -----------------------------------------------------------------------------------------------------------------
    static const std::array<const char*, 10> DataLineString;
    // -----------------------------------------------------------------------------------------------------------------

    // CRD DATA ENUMS
    // -----------------------------------------------------------------------------------------------------------------
    enum class DataRecordTypeEnum
    {
        FULL_RATE_RECORD            = 0,
        NORMAL_POINT_RECORD         = 1,
        RANGE_SUPPLEMENT_RECORD     = 2, // TODO
        METEO_RECORD                = 3,
        METEO_SUP_RECORD            = 4, // TODO
        POINTING_ANGLES_RECORD      = 5, // TODO
        CALIBRATION_OVERALL_RECORD  = 6,
        CALIBRATION_DETAIL_RECORD   = 7, // For v2 only
        STATISTICS_RECORD           = 8,
        COMPATIBILITY_RECORD        = 9, // Deprecated. Only legacy CRD
    };

    enum class EpochEventEnum
    {
        SPACECRAFT_BOUNCE_TIME_2W = 1,
        GROUND_TRANSMIT_TIME_2W   = 2, // TODO other values
    };

    enum class FilterFlagEnum
    {
        UNKNOWN = 0,
        NOISE_EXCLUDED_RETURN = 1,
        DATA   = 2,
    };

    enum class MeteoOriginEnum
    {
        MEASURED = 0,
        INTERPOLATED = 1,
    };

    enum class DataTypeEnum
    {
        STATION_COMBINED = 0,
        STATION_TRANSMIT = 1,
        STATION_RECEIVE  = 2,
        TARGET_COMBINED  = 3,
        TARGET_TRANSMIT  = 4,
        TARGET_RECIEVE   = 5
    };

    enum class CalibrationTypeEnum
    {
        CAL_UNDEFINED          = 0,
        NOMINAL                = 1,
        EXTERNAL               = 2,
        INTERNAL_V1_TELESCOPE  = 3,  // V1: Internal. V2: Internal Telescope
        BURST_V1_BUILDING      = 4,  // V1: Burst. V2: Internal Building
        OTHER_V1_BURST         = 5,  // V1: Other. V2: Burst calibrations
        CAL_OTHER              = 6   // V2 only: Other
    };

    enum class ShiftTypeEnum
    {
        SHIFT_UNDEFINED     = 0,
        SHIFT_NOMINAL       = 1,
        PRE_TO_POST         = 2,
        MIN_TO_MAX          = 3,
        SHIFT_OTHER         = 4
    };

    enum class CalibrationSpan   // For V2 only.
    {
        NOT_APPLICABLE     = 0,    // For example if calibration type is "nominal". For engineering.
        PRE_CALIBRATION    = 1,    // Pre only
        POST_CALIBRATION   = 2,    // Post only
        COMBINED           = 3,    // Pre + Post                                    Not used in line 41
        REAL_TIME          = 4,    // Data taken while ranging to a satellite.      Not used in line 41
        ONE_OF_MULTIPLE    = 5
    };

    // Assessment data quality enum.
    // Used as specific quality indicator determined after processing and filtering the data.
    // A more generic indicator can be fount in the session record.
    enum class DataQualityEnum
    {
        AUTO_QUALITY = -1,              // For calculating the quality using the internal methods. [TODO]
        UNDEFINED_QUALITY = 0,
        CLEAR_NO_NOISE = 1,
        CLEAR_SOME_NOISE = 2,
        CLEAR_SIGNIFICANT_NOISE = 3,
        UNCLEAR = 4,
        NOT_DATA = 5
    };

    enum class DataGenerationOptionEnum
    {
        FULL_RATE,
        NORMAL_POINT,
        BOTH_DATA
    };
    // -----------------------------------------------------------------------------------------------------------------

    // CRD DATA STRUCTS                                                          // TODO: Rest of the structs and lines.
    // -----------------------------------------------------------------------------------------------------------------
    // 10 - FULL RATE DATA
    struct FullRateRecord : dpslr::common::ConsolidatedRecordStruct
    {
        // Members.
        long double time_tag;        // Second of day (12 decimals). 100 ns precission for SLR/LLR. 1 ps for others.
        long double time_flight;     // Can be none, one-, two-way, etc. It can be corrected. Seconds with 12 decimals.
        std::string system_cfg_id;                       // System configuration ID.
        EpochEventEnum epoch_event;                      // Indicates the time event reference.
        FilterFlagEnum filter_flag;                      // Indicates the filter type of this record.
        unsigned int detector_channel;                   // 0 for na or all, 1-4 for quadrant, 1-n for many channels.
        unsigned int stop_number;                        // For multiple stop numbers.
        dpslr::common::optional<unsigned int> receive_amp;   // Linear scale value.                               [optional]
        dpslr::common::optional<unsigned int> transmit_amp;  // Linear scale value.                      [For v2] [optional]
        // Functions.
        std::string generateLine(float version) const;
    };

    // 11 - NORMAL POINT DATA
    struct NormalPointRecord : dpslr::common::ConsolidatedRecordStruct
    {
        // Members.
        long double time_tag;                     // Second of day (12 decimals). Clock correction should be applied.
        long double time_flight;                  // Should be can be corrected for calibration delay.
        std::string system_cfg_id;                // System configuration ID.
        EpochEventEnum epoch_event;               // Indicates the time event reference.
        double window_length;                     // Seconds.
        unsigned int raw_ranges;                  // Raw ranges compressed into the normal point.
        dpslr::common::optional<double> bin_rms;      // From mean of raw minus the trend function (ps).      [optional]
        dpslr::common::optional<double> bin_skew;     // From mean of raw minus the trend function.           [optional]
        dpslr::common::optional<double> bin_kurtosis; // From mean of raw minus the trend function.           [optional]
        dpslr::common::optional<double> bin_peak;     // Bin peak - mean value (ps).    [non-standard explicit optional]
        double return_rate;                       // %
        unsigned int detector_channel;            // 0 for na or all, 1-4 for quadrant, 1-n for many channels.
        dpslr::common::optional<double> snr;          // S:N Signal to noise ratio.                      [For v2] [optional]
        // Functions.
        std::string generateLine(float version) const;
    };

    // 20 - METEOROLOGICAL DATA
    struct MeteorologicalRecord : dpslr::common::ConsolidatedRecordStruct
    {
        // Must be added if:
        // -> Pressure change 0.1 mbar
        // Temperature 0.1 K
        // Humidity 5%

        // Members.
        long double time_tag;                 // Second of day (12 decimals). 1 ms precission.
        double surface_pressure;              // milibar
        double surface_temperature;           // kelvin
        double surface_relative_humidity;     // %
        MeteoOriginEnum values_origin;        // 0 - measured, 1 - interpolated
        // Functions.
        std::string generateLine(float version) const;
    };

    // 40 and 41 - CALIBRATION DATA
    struct CalibrationRecord : dpslr::common::ConsolidatedRecordStruct
    {
        // This struct covers both lines 40 and 41. The cases are:
        //   - For v1:
        //      - Pre and post will be lines 40. RT calibrations do not exist.
        //   - For v2:
        //      - RT calibrations will always be lines 40.
        //      - Pre and post (or other detail calibrations) will be lines 41.
        //      - We will use all lines 41 to compute ONE line 40 (overall)  with the field span as 1, 2 or 3.
        // So, for v2 there can be multiple lines 40 with span = RealTime, but there can only be ONE
        // line 40 with the field span as 1, 2 or 3.
        //
        // For overall calibration (v2, line 40), remember:
        //  - time_tag            = middle of pass time.
        //  - data_type           = must be the same for all cal_records data.
        //  - system_cfg_id       = global cfg id.
        //  - data_recorded       = sum(cal_records.data_recorded)                               [Empty if missing data]
        //  - data_used           = sum(cal_records.data_used)                                   [Empty if missing data]
        //  - target_dist_1w      = must be the same for all cal_records data.                   [Empty if missing data]
        //  - calibration_delay   = sum(cal_records.calibration_delay)/size(cal_records)
        //  - delay_shift         = post - pre OR max(cal_rec.cal_delay) - min(cal_rec.cal_delay) OR 0
        //  - rms                 = sum(cal_records.rms)/size(cal_records)
        //  - skew                = sum(cal_records.skew)/size(cal_records)
        //  - kurtosis            = sum(cal_records.kurtosis)/size(cal_records)
        //  - peak                = sum(cal_records.peak)/size(cal_records)
        //  - cal_type            = must be the same for all cal_records data.
        //  - shift_type          = Pre/Post, minimum to max, not used, other
        //  - detector_channel    = must be the same for all cal_records data.
        //  - span                = Pre/Post if size(cal_records) = 1, Combined if size(cal_records) > 1
        //  - return_rate         = sum(cal_records.return_rate)/size(cal_records)               [Empty if missing data]
        //  - is_overall          = true.

        // Members.
        long double time_tag;           // Second of day with clock correction. For v2, middle of pass for line 40.
        DataTypeEnum data_type;         // Usually station combined for SLR/LLR.
        std::string system_cfg_id;      // System configuration ID.
        dpslr::common::optional<int> data_recorded;      // Number of data recorded.                          [optional]
        dpslr::common::optional<int> data_used;          // Number of data point used.                        [optional]
        dpslr::common::optional<double> target_dist_1w;  // Meters, nominal, one way.                         [optional]
        double calibration_delay;       // Calibration system delay (ps), 1 decimal.
        double delay_shift;             // Calibration stability (ps), 1 decimal. 0 for line 41
        double rms;                     // RMS (ps)
        double skew;                    // Skew from the mean
        double kurtosis;                // Kurtosis from the mean
        double peak;                    // peak Mean value (ps)
        CalibrationTypeEnum cal_type;   // Calibration type.
        ShiftTypeEnum shift_type;       // Usually pre to post or minimum to maximum.
        unsigned int detector_channel;  // 0 for na or all, 1-4 for quadrant, 1-n for many channels.
        CalibrationSpan span;           // Span of the records.                                      [For v2] [optional]
        dpslr::common::optional<double> return_rate; // %                                            [For v2] [optional]
        bool is_overall = false;        // True: The struct is an overall cal (line 40). False: is detail cal.

        // Functions.
        std::string generateLine(float version) const;   // Generate lines 40-41
    };

    // 50 - STATISTICS DATA
    struct StatisticsRecord : dpslr::common::ConsolidatedRecordStruct
    {
        std::string system_cfg_id;
        double rms;
        dpslr::common::optional<double>  skew;
        dpslr::common::optional<double>  kurtosis;
        dpslr::common::optional<double>  peak;
        DataQualityEnum quality;

        std::string generateLine(float version) const;
    };

    // Constructor.
    CRDData() = default;

    // Copy and movement constructors and assign operators
    CRDData(const CRDData&) = default;
    CRDData(CRDData&&) = default;
    CRDData& operator = (const CRDData&) = default;
    CRDData& operator = (CRDData&&) = default;

    // Destructor.
    ~CRDData() = default;

    // Clear methods.
    void clearAll();
    void clearFullRateRecords();
    void clearNormalPointRecords();
    void clearMeteorologicalRecords();
    void clearCalibrationRecords();
    void clearRTCalibrationRecords();
    void clearOverallCalibrationRecord();
    void clearStatisticsRecord();

    // Data getters.
    dpslr::common::FlightTimeData fullRateFlightTimeData() const;
    dpslr::common::FlightTimeData normalPointFlightTimeData() const;

    const std::vector<FullRateRecord> &fullRateRecords() const;
    const std::vector<NormalPointRecord> &normalPointRecords() const;
    const std::vector<MeteorologicalRecord> &meteorologicalRecords() const;
    const std::vector<CalibrationRecord> &realTimeCalibrationRecord() const;
    const std::vector<CalibrationRecord> &calibrationRecords() const;
    const dpslr::common::optional<CalibrationRecord> &calibrationOverallRecord() const;
    const dpslr::common::optional<StatisticsRecord> &statisticsRecord() const;

    // Non-const getters
    std::vector<FullRateRecord> &fullRateRecords();
    std::vector<NormalPointRecord> &normalPointRecords();
    std::vector<MeteorologicalRecord> &meteorologicalRecords();
    std::vector<CalibrationRecord> &realTimeCalibrationRecord();
    std::vector<CalibrationRecord> &calibrationRecords();
    dpslr::common::optional<CalibrationRecord> &calibrationOverallRecord();
    dpslr::common::optional<StatisticsRecord> &statisticsRecord();

    // Records individual setter methods.
    void addFullRateRecord(const FullRateRecord&);
    void addNormalPointRecord(const NormalPointRecord&);                           // For external calculations.
    void addMeteorologicalRecord(const MeteorologicalRecord&);
    void addCalibrationRecord(const CalibrationRecord&);
    void addRealTimeCalibrationRecord(const CRDData::CalibrationRecord &);
    void setOverallCalibrationRecord(const CalibrationRecord&);                    // For external calculations.
    void setStatisticsRecord(const StatisticsRecord&);                             // For external calculations.

    // Records multiple setter methods
    void setFullRateRecords(const std::vector<FullRateRecord>&);
    void setNormalPointRecords(const std::vector<NormalPointRecord>&);             // For external calculations.
    void setMeteorologicalRecords(const std::vector<MeteorologicalRecord>&);
    void setCalibrationRecords(const std::vector<CalibrationRecord>&);
    void setRealTimeCalibrationRecords(const std::vector<CalibrationRecord>&);

    // Generate all CRD data lines.
    std::string generateDataLines(float version, DataGenerationOptionEnum option) const;

    // Generate specific CRD lines. Data must be sorted.
    std::string generateFullRateLines(float version) const;       // Line 10              TODO: Sort function in future.
    std::string generateNormalPointLines(float version) const;    // Line 11              TODO: Sort function in future.
    std::string generateMeteoLines(float version) const;          // Line 20              TODO: Sort function in future.
    std::string generateCalibrationLines(float version) const;    // Lines 40 41          TODO: Sort function in future.
    std::string generateStatisticsLine(float version) const;      // Line 50

    // Generic read methods.
    // Calls to clearAll()
    dpslr::common::RecordReadErrorMultimap readData(const dpslr::common::RecordLinesVector &rec_v, float version);

    // Specific read methods.
    // The read data-line functions append the data to the proper vector. Do nothing if error.
    // The read data functions clear the pointers (struct) and create a new one (or nullptr if error).
    dpslr::common::RecordReadErrorEnum readFRDataLine(const dpslr::common::ConsolidatedRecordStruct&, float v);       // Line 10
    dpslr::common::RecordReadErrorEnum readNPDataLine(const dpslr::common::ConsolidatedRecordStruct&, float v);       // Line 11
    dpslr::common::RecordReadErrorEnum readMeteoDataLine(const dpslr::common::ConsolidatedRecordStruct&, float v);    // Line 20
    dpslr::common::RecordReadErrorEnum readCalDataLine(const dpslr::common::ConsolidatedRecordStruct&, float v);  // Lines 40 41
    dpslr::common::RecordReadErrorEnum readStatisticsData(const dpslr::common::ConsolidatedRecordStruct&, float v);   // Line 50


    //TODO
    // Integrity Checker.
    //// Si v2 y no overall, no puede haber delay shift.
    // Funcion para interpolar datos de meteorologia si es necesario.
    // Funcion para calcular puntos normales.


private:

    // Generic private read method.
    dpslr::common::RecordReadErrorEnum readDataLine(const dpslr::common::RecordLinePair &rpair, float version);

    // Private vectors for store the different data records.
    std::vector<FullRateRecord> fullrate_records;              // Full rate record.
    std::vector<NormalPointRecord> normalpoint_records;        // Normal point record vector.
    std::vector<MeteorologicalRecord> meteo_records;           // Meteo records vector.
    std::vector<CalibrationRecord> rt_cal_records;             // Real time calibrations (for v2).
    std::vector<CalibrationRecord> cal_records;                // Calibration records (line 40 for v1, 41 for v2)

    // Calibration overall record (for v2) and statistics record storage.
    dpslr::common::optional<CalibrationRecord> cal_overall_record;
    dpslr::common::optional<StatisticsRecord> stat_record;
};


// ========== CRD CLASS ================================================================================================

class LIBDPSLR_EXPORT CRD
{
public:

    // CRD FILE EXTENSIONS
    // -----------------------------------------------------------------------------------------------------------------
    static const std::array<const char*, 6> ExtensionsString;
    static const std::array<const char*, 2> EndRecordsString;
    // -----------------------------------------------------------------------------------------------------------------

    // CRD DATA ENUMS
    // -----------------------------------------------------------------------------------------------------------------

    /// @enum CRDRecordsTypeEnum
    /// This enum represents the differents CRD record types.
    enum class CRDRecordsTypeEnum
    {
        EOS_RECORD,     ///< End of Session record (H8).
        EOF_RECORD,     ///< End of File record (H9).
        HEADER_RECORD,  ///< Header records (H1, H2, H3, H4, H5).
        CFG_RECORD,     ///< Configuration records (C0, C1, C2, C3, C4, C5, C6, C7).
        DATA_RECORD     ///< Data records (10, 11, 12, 20, 21, 30, 40, 41, 50).
    };

    /// @enum OpenOptionEnum
    /// This enum represents the possible options for reading CRD files. Used in the function @ref openCRDFile.
    enum class OpenOptionEnum
    {
        ONLY_HEADER,         ///< Only header records will be read. Use this for faster processing (i.e. searching files).
        ONLY_HEADER_AND_CFG, ///< Only header and configuration will be read.
        ALL_DATA             ///< All records will be read, including data records.
    };


    /// @enum TargetIdOptionEnum
    /// This enum represents the possible target identifiers that can be used for generating the CRD filename. It is
    /// used in the function @ref getStandardFilename.
    enum class TargetIdOptionEnum
    {
        TARGET_NAME,   ///< Target name (a very bad identifier for debris targets!).
        ILRS_ID,       ///< ILRS identifier (eg. 8606101).
        NORAD,         ///< NORAD identifier (eg. 16908 ).
        COSPAR,        ///< COSPAR identifier: 1986-061A
        SHORT_COSPAR   ///< SHORT COSPAR identifier: 86061A
    };

    /// @enum ReadFileErrorEnum
    /// This enum represents the posible errors at CRD file reading.
    enum class ReadFileErrorEnum
    {
        NOT_ERROR,            ///< No error.
        RECORDS_LOAD_WARNING, ///< Issues in multiple types of records. Use the getters for get specific ones.
        HEADER_LOAD_WARNING,  ///< Issues in header records. Use @ref getReadHeaderErrors for get specific ones.
        CFG_LOAD_WARNING,     ///< Issues in configuration records. Use @ref getReadCfgErrors for get specific ones.
        DATA_LOAD_WARNING,    ///< Issues in data records. Use @ref getReadDataErrors for get specific ones.
        UNDEFINED_RECORD,     ///< Read an undefined record. The wrong record can be get using @ref getLastErrorStruct.
        ORDER_ERROR,          ///< Bad record order in the file. The record can be get using @ref getLastErrorStruct.
        MULTIPLE_EOS,         ///< Multiple end of session (H8). The record can be get using @ref getLastErrorStruct.
        CONTENT_AFTER_EOF,    ///< Content after the end of file (H9) record. H9 must be the last line always.
        EOS_NOT_FOUND,        ///< The end of session token (H8) is not found.
        EOF_NOT_FOUND,        ///< The end of file token (H9) is not found.
        FILE_NOT_FOUND,       ///< The CRD file is not found.
        FILE_EMPTY,           ///< The CRD file is empty.
        FILE_TRUNCATED,       ///< The file is truncated. The end of file was reached before completing the data read.
        NO_HEADER_FOUND,      ///< No header records were found.
        NO_CFG_FOUND,         ///< No configuration records were found.
        NO_DATA_FOUND,        ///< No data records were found.
        VERSION_UNKNOWN       ///< Header records loaded, but version missing (maybe missing H1).
    };

    /// @enum WriteFileErrorEnum
    /// This enum represents the posible errors at CRD file writing.
    enum class WriteFileErrorEnum
    {
        NOT_ERROR,            ///< No error.
        FILE_ALREADY_EXIST,   ///< The file already exist and force option was not specified.
        VERSION_UNKNOWN       ///< Version missing (missing H1).
    };

    /// @enum ReadRecordResultEnum
    /// This enum represents the posible errors when reading each record.
    enum class ReadRecordResultEnum
    {
        NOT_ERROR,           ///< No error.
        STREAM_NOT_OPEN,     ///< The file is not found.
        STREAM_EMPTY,        ///< The file is empty.
        UNDEFINED_RECORD     ///< The readead record is not recognized.
    };


    // -----------------------------------------------------------------------------------------------------------------

    // CRD CONVENIENT TYPE ALIAS
    // -----------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------

    // Constructors.
    /**
     * @brief Empty constructor to create an empty CRD. It can be opened later with @ref openCRDFile (this function
     * will update the version). Otherwise, you can add/modify records using the getters to generate your custom CRD.
     * @param version
     */
    explicit CRD(float version = 2.0);

    /**
     * @brief Constructor to create a CRD by reading a file. Remember to check the possible errors.
     * @param crd_filepath
     * @param open_data
     */
    explicit CRD(const std::string& crd_filepath, OpenOptionEnum open_option);

    // Default destructor
    ~CRD() = default;

    // Copy and movement constructors and assign operators
    CRD(const CRD&) = default;
    CRD(CRD&&) = default;
    CRD& operator = (const CRD&) = default;
    CRD& operator = (CRD&&) = default;

    // Clear methods.
    /**
     * @brief Clears all data in CRD, including data, configuration, header, error structs and filename.
     */
    void clearCRD();
    /**
     * @brief Clears data, configuration and header in CRD, leaving it empty. Error structs and filename are kept.
     */
    void clearCRDContents();
    /**
     * @brief Clears the CRD header.
     */
    void clearCRDHeader();
    /**
     * @brief Clears the CRD configuration.
     */
    void clearCRDConfiguration();
    /**
     * @brief Clears the CRD data.
     */
    void clearCRDData();

    // CRD header, cfg and data getter methods for records edition as modifiers.
    CRDHeader &getHeader();
    CRDConfiguration &getConfiguration();
    CRDData &getData();

    // CRD header, cfg and data const getter methods.
    const CRDHeader &getHeader() const;
    const CRDConfiguration &getConfiguration() const;
    const CRDData &getData() const;

    // Error getters.
    const dpslr::common::RecordReadErrorMultimap& getReadHeaderErrors() const;
    const dpslr::common::RecordReadErrorMultimap& getReadCfgErrors() const;
    const dpslr::common::RecordReadErrorMultimap& getReadDataErrors() const;
    ReadFileErrorEnum getLastReadError() const;
    const dpslr::common::optional<dpslr::common::ConsolidatedRecordStruct>& getLastReadErrorRecord() const;

    // Filename getters.
    const std::string& getSourceFilename() const;
    const std::string& getSourceFilepath() const;

    // Observers
    /**
     * @brief Checks if CPF is empty. A CPF will be empty if both header and data are empty. This can happen if an error
     * occurs at CPF reading, or if user explicitly clears data and header.
     * This method is only valid if CPF is used for reading. For writing, the user is responsible to track the emptiness
     * status of the CPF.
     * @return true if CPF is empty, false otherwise.
     */
    bool empty() const;

    // File Methods
    /**
     * @brief Custom getter that generates the filename using the CRD naming convention.
     *
     * This function generates a ILRS CRD standard convention filename. The convention is:
     *  - ILRS tracking:
     *    ssss_satname_crd_yyyymmdd_hh[MM]_rr.typ ("ssss" is the CDP Pad, and the "rr" the release number
     *  - For non ILRS tracking (only for CRD version 2):
     *    networkname_ssss_satname_crd_yyyymmdd_hh[MM]_rr.typ
     *
     * @pre The Format Header (H1) must be defined.
     *      The Station Header (H2) must be defined.
     *      The Session Header (H4) must be defined.
     *      The Target Header (H3) must be defined.
     *
     * @post For CRD version 2, if station network != "ILRS", the track will be treated as a debris/other tracking.
     *
     * @param[in] id_option Determines which identifier will be used for the file name. Default is name.
     *
     * @return A string that contains the generated standard filename. If error, it returns an empty string.
     *
     * @note Changing the ID that appears in the file name is interesting, because different stations can use different
     *       target names, especially when tracking debris. Also, using other more standard IDs allows faster and
     *       simpler file search systems.
     */
    std::string getStandardFilename(TargetIdOptionEnum id_option = TargetIdOptionEnum::TARGET_NAME) const;

    /**
     * @brief Open a CRD file.
     *
     * This function opens an ILRS Consolidated Ranging Data file, and store the records in memory. Using the open
     * options, you can open the header records only, the header and configuration records, or all the records. This
     * can be useful for fast opening while searching certain files. This call doesn't modify the content of the file.
     *
     * @post This call will clear all the structs with the previous data, calling to clearCRD().
     *       This call will clear the read error vectors with the previous errors.
     *       This call will open the file, store the records in memory, and close the file.
     *
     * @param[in] crd_filepath String with the complete path where the CRD file is stored.
     * @param[in] open_option Determines which structures will be read and stored.
     *
     * @return A @ref ReadFileErrorEnum value that contains the possible error.
     */
    ReadFileErrorEnum openCRDFile(const std::string& crd_filepath, OpenOptionEnum open_option);

    /**
     * @brief Open all records of a previously opened CRD file.
     *
     * This function opens all records (including the data records) of an ILRS Consolidated Ranging Data file, and store
     * the records in memory. This function will call to @ref openCRDFile internally. Must be called only if you
     * previously open a CRD file with @ref CRD(const std::string&, OpenOptionEnum) or the @ref openCRDFile.
     *
     * @post This call will clear all the structs with the previous data, calling to clearCRD().
     *       This call will clear the read error vectors with the previous errors.
     *       This call will open the file, store the records in memory, and close the file.
     *
     * @return A @ref ReadFileErrorEnum value that contains the possible error.
     */
    ReadFileErrorEnum openCRDData();

    /**
     * @brief Writes a CRD file.
     *
     * This function writes the records stored in memory into an ILRS Consolidated Ranging Data file.
     *
     * @post
     *
     * @param[in] crd_filepath String with the complete path where the CRD file will be written.
     * @param[in] data_opt A @ref DataGenerationOptionEnum which specifies the data that will be written.
     * @param[in] force Boolean that indicates if the writing can be forced even if the file already exists.

     * @return A @ref WriteFileErrorEnum value that contains the possible error.
     */
    WriteFileErrorEnum writeCRDFile(const std::string& crd_filepath, CRDData::DataGenerationOptionEnum data_opt,
                                   bool force = false);




    // Use the Format Header version for write the data
    //bool writeCRDFile(const std::string& crd_filepath);
    //

    // Check integrity method.
    // TODO

private:

    // Private methods for reading records.
    // Return false if error.
    ReadRecordResultEnum readRecord(dpslr::helpers::InputFileStream&, dpslr::common::ConsolidatedRecordStruct&);

    // Empty.
    bool empty_;

    // Data, configuration and header storage.
    CRDHeader header;
    CRDConfiguration configuration;
    CRDData data;

    // Private members for store read errors.
    dpslr::common::RecordReadErrorMultimap read_header_errors;
    dpslr::common::RecordReadErrorMultimap read_cfg_errors;
    dpslr::common::RecordReadErrorMultimap read_data_errors;
    dpslr::common::optional<dpslr::common::ConsolidatedRecordStruct> last_error_record_;
    ReadFileErrorEnum last_read_error_;

    // File names and path.
    std::string crd_filename;
    std::string crd_fullpath;
};
// =====================================================================================================================


// ========== EXTERNAL OPERATORS =======================================================================================
//bool LIBDPSLR_EXPORT operator <(const CRD& a, const CRD& b); TODO
//bool LIBDPSLR_EXPORT operator >(const CRD& a, const CRD& b);
// =====================================================================================================================
