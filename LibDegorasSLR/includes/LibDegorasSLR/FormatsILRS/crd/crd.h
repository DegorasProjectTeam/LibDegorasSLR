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
   2011.1
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


// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
#include "LibDegorasSLR/FormatsILRS/crd/records/crd_header.h"
#include "LibDegorasSLR/FormatsILRS/crd/records/crd_configuration.h"
#include "LibDegorasSLR/FormatsILRS/crd/records/crd_data.h"
#include "LibDegorasSLR/FormatsILRS/common/consolidated_record.h"
#include "LibDegorasSLR/Helpers/filedir_helpers.h"
// =====================================================================================================================

// C++ INCLUDES
// =====================================================================================================================
#include <string>
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace ilrs{
namespace crd{
// =====================================================================================================================

class LIBDPSLR_EXPORT CRD
{
public:

    // CRD
    // -----------------------------------------------------------------------------------------------------------------
    /// Different extensions for the CRD format. Usually the "frd" and "npt" extensions are used.
    static constexpr std::array<const char*, 6> ExtensionsStr{"frd", "npt", "fr2", "np2", "qlk", "crd"};
    // -----------------------------------------------------------------------------------------------------------------

    // CRD DATA ENUMS
    // -----------------------------------------------------------------------------------------------------------------

    /// @enum CRDRecordsType
    /// This enum represents the differents CRD record types.
    enum class CRDRecordsType
    {
        EOS_RECORD,     ///< End of Session record (H8).
        EOF_RECORD,     ///< End of File record (H9).
        HEADER_RECORD,  ///< Header records (H1, H2, H3, H4, H5).
        CFG_RECORD,     ///< Configuration records (C0, C1, C2, C3, C4, C5, C6, C7).
        DATA_RECORD     ///< Data records (10, 11, 12, 20, 21, 30, 40, 41, 50).
    };

    /// @enum OpenOption
    /// This enum represents the possible options for reading CRD files. Used in the function @ref openCRDFile.
    enum class OpenOption
    {
        ONLY_HEADER,         ///< Only header records will be read. Use this for faster processing (i.e. searching files).
        ONLY_HEADER_AND_CFG, ///< Only header and configuration will be read.
        ALL_DATA             ///< All records will be read, including data records.
    };


    /// @enum TargetIdOption
    /// This enum represents the possible target identifiers that can be used for generating the CRD filename. It is
    /// used in the function @ref getStandardFilename.
    enum class TargetIdOption
    {
        TARGET_NAME,   ///< Target name (a very bad identifier for debris targets!).
        ILRS_ID,       ///< ILRS identifier (eg. 8606101).
        NORAD,         ///< NORAD identifier (eg. 16908 ).
        COSPAR,        ///< COSPAR identifier: 1986-061A
        SHORT_COSPAR   ///< SHORT COSPAR identifier: 86061A
    };

    /// @enum ReadFileError
    /// This enum represents the posible errors at CRD file reading.
    enum class ReadFileError
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
    enum class WriteFileError
    {
        NOT_ERROR,            ///< No error.
        FILE_ALREADY_EXIST,   ///< The file already exist and force option was not specified.
        VERSION_UNKNOWN       ///< Version missing (missing H1).
    };

    /// @enum ReadRecordResultEnum
    /// This enum represents the posible errors when reading each record.
    enum class ReadRecordResult
    {
        NOT_ERROR,           ///< No error.
        STREAM_NOT_OPEN,     ///< The file is not found.
        STREAM_EMPTY,        ///< The file is empty.
        UNDEFINED_RECORD     ///< The readead record is not recognized.
    };

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
    explicit CRD(const std::string& crd_filepath, OpenOption open_option);

    M_DEFINE_CTOR_DEF_COPY_MOVE_OP_COPY_MOVE_DTOR(CRD)

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
    const common::RecordReadErrorMultimap& getReadHeaderErrors() const;
    const common::RecordReadErrorMultimap& getReadCfgErrors() const;
    const common::RecordReadErrorMultimap& getReadDataErrors() const;
    ReadFileError getLastReadError() const;
    const Optional<common::ConsolidatedRecord>& getLastReadErrorRecord() const;

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
    std::string getStandardFilename(TargetIdOption id_option = TargetIdOption::TARGET_NAME) const;

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
    ReadFileError openCRDFile(const std::string& crd_filepath, OpenOption open_option);

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
    ReadFileError openCRDData();

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
    WriteFileError writeCRDFile(const std::string& crd_filepath, CRDData::DataGenerationOption data_opt,
                                bool force = false);


    // Use the Format Header version for write the data
    //bool writeCRDFile(const std::string& crd_filepath);
    //

    // Check integrity method.
    // TODO

private:

    // Private methods for reading records.
    // Return false if error.
    ReadRecordResult readRecord(helpers::files::DegorasInputFileStream&, common::ConsolidatedRecord&);

    // Empty.
    bool empty_;

    // Data, configuration and header storage.
    CRDHeader header;
    CRDConfiguration configuration;
    CRDData data;

    // Private members for store read errors.
    common::RecordReadErrorMultimap read_header_errors;
    common::RecordReadErrorMultimap read_cfg_errors;
    common::RecordReadErrorMultimap read_data_errors;
    Optional<common::ConsolidatedRecord> last_error_record_;
    ReadFileError last_read_error_;

    // File names and path.
    std::string crd_filename;
    std::string crd_fullpath;
};

// =====================================================================================================================

}}} // END NAMESPACES.
// =====================================================================================================================
