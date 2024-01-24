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
 * @file class_cpf.h
 *
 * @brief This file contains an abstraction of Consolidated Laser Target Prediction Format (CPF),
 * for version 1 and 2, currently.
 *
 * This library is an abstraction of Consolidated Laser Target Prediction Format (CPF), for version 1 and 2, currently.
 * This class permits opening an existing CPF file and editing, or creating new files with raw data.
 *
 * This implementation also permits some less restrictive functionalities as convenient file naming generation using
 * NORAD, mixed files generation, and others.
 *
 * Also, the library contains methods for checking the consistency and integrity of all the introduced data [TODO],
 * and for including comment records (lines "00") when you need for clarity or to indicate important things.
 *
 * The "CPF Version 1" specification can be found in: https://ilrs.gsfc.nasa.gov/docs/2006/cpf_1.01.pdf
 * The "CPF Version 2" specification can be found in: https://ilrs.gsfc.nasa.gov/docs/2018/cpf_2.00h-1.pdf
 *
 * @author    Degoras Project Team.
 * @version   2202.1
 * @date      03-02-2022
 * @copyright EUPL License.
 *
 * @par IMPORTANT NOTE:
 *
 * This CPF code is provided "as is" with no guarantees. The software has been tested with a limited amount of data,
 * and there may still be errors and incomplete implementation of standards.
 *
 * Some cpf do not start at midnight even though they indicate so. They do this to avoid interpolation errors, but
 * not all do. For this reason, when working internally with the CPF we do not usually take into account the hours
 * indicated in the headers.
 *
 * @todo Consistency and integrity checker functions (like in EUROLAS Data Center).
 * @todo Sorting the records or ensure the order.
 **********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdpslr_global.h"
#include "LibDegorasSLR/FormatsILRS/cpf/records/cpf_header.h"
#include "LibDegorasSLR/FormatsILRS/cpf/records/cpf_data.h"
#include "LibDegorasSLR/FormatsILRS/common/consolidated_record.h"
#include "LibDegorasSLR/Helpers/file_helpers.h"
#include "LibDegorasSLR/Mathematics/containers/interval.h"
// =====================================================================================================================

// C++ INCLUDES
// =====================================================================================================================
#include <string>
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace ilrs{
namespace cpf{
// =====================================================================================================================

class LIBDPSLR_EXPORT CPF
{
public:

    // CPF DATA ENUMS
    // -----------------------------------------------------------------------------------------------------------------

    /// @enum CPFRecordsTypeEnum
    /// This enum represents the differents CPF record types.
    enum class CPFRecordsType
    {
        EOH_RECORD = 1,  ///< End of Header record (H9).
        EOE_RECORD = 2,  ///< End of Ephemeris record (99).
        HEADER_RECORD,   ///< Header records (H1, H2, H3, H4, H5).
        DATA_RECORD      ///< Data records (10, 20, 30, 40, 50, 60, 70).
    };

    /// @enum OpenOptionEnum
    /// This enum represents the possible options for reading CPF files. Used in the function @ref openCPFFile.
    enum class OpenOptionEnum
    {
        ONLY_HEADER,    ///< Only header records will be read. Use this for faster processing (i.e. searching files).
        ALL_DATA        ///< All records will be read, including data records.
    };

    /// @enum TargetIdOptionEnum
    /// This enum represents the possible target identifiers that can be used for generating the CPF filename. It is
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
    /// This enum represents the posible errors at CPF file reading.
    enum class ReadFileErrorEnum
    {
        NOT_ERROR,            ///< No error.
        RECORDS_LOAD_WARNING, ///< Issues in multiple types of records. Use the getters for get specific ones.
        HEADER_LOAD_WARNING,  ///< Issues in header records. Use @ref getReadHeaderErrors for get specific ones.
        DATA_LOAD_WARNING,    ///< Issues in data records. Use @ref getReadDataErrors for get specific ones.
        UNDEFINED_RECORD,     ///< Read an undefined record. The wrong record can be get using @ref getLastErrorStruct.
        ORDER_ERROR,          ///< Bad record order in the file. The record can be get using @ref getLastErrorStruct.
        MULTIPLE_EOH,         ///< Multiple end of session (H9). The record can be get using @ref getLastErrorStruct.
        CONTENT_AFTER_EOE,    ///< Content after the end of file (99) record. 99 must be the last line always.
        EOH_NOT_FOUND,        ///< The end of session token (H9) is not found.
        EOE_NOT_FOUND,        ///< The end of file token (99) is not found.
        FILE_NOT_FOUND,       ///< The CPF file is not found.
        FILE_EMPTY,           ///< The CPF file is empty.
        FILE_TRUNCATED,       ///< The file is truncated. The end of file was reached before completing the data read.
        NO_HEADER_FOUND,      ///< No header records were found.
        NO_DATA_FOUND,        ///< No data records were found.
        VERSION_UNKNOWN       ///< Header records loaded, but version missing (maybe missing H1).
    };

    /// @enum WriteFileErrorEnum
    /// This enum represents the posible errors at CPF file writing.
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

    // Constructors.
    /**
     * @brief Empty constructor to create an empty CPF. It can be opened later with @ref openCPFFile (this function
     * will update the version). Otherwise, you can add/modify records using the getters to generate your custom CPF.
     * @param version
     */
    explicit CPF(float version = 2.0);

    /**
     * @brief Constructor to create a CPF by reading a file. Remember to check the possible errors.
     * @param cpf_filepath
     * @param open_data
     */
    explicit CPF(const std::string& cpf_filepath, OpenOptionEnum open_option);

    // Default destructor
    ~CPF() = default;

    // Copy and movement constructors and assign operators
    CPF(const CPF&) = default;
    CPF(CPF&&) = default;
    CPF& operator = (const CPF&) = default;
    CPF& operator = (CPF&&) = default;


    // Clear methods.
    /**
     * @brief Clears all data in CPF, including data, header, error structs and filename.
     */
    void clearCPF();
    /**
     * @brief Clears data and header in CPF, leaving it empty. Error structs and filename are kept.
     */
    void clearCPFContents();
    /**
     * @brief Clears the CPF header.
     */
    void clearCPFHeader();
    /**
     * @brief Clears the CPF data.
     */
    void clearCPFData();


    // CPF header and data getter methods for records edition as modifiers.
    CPFHeader &getHeader();
    CPFData &getData();

    // CPF header, cfg and data const getter methods.
    const CPFHeader &getHeader() const;
    const CPFData &getData() const;

    // Error getters.
    const common::RecordReadErrorMultimap& getReadHeaderErrors() const;
    const common::RecordReadErrorMultimap& getReadDataErrors() const;
    ReadFileErrorEnum getLastReadError() const;
    const Optional<common::ConsolidatedRecord>& getLastReadErrorRecord() const;

    // Filename getters.
    const std::string& getSourceFilename() const;
    const std::string& getSourceFilepath() const;

    /**
     * @brief Gets the available time window for the CPF. If is empty, all values are set to zero.
     *
     * This function retrieves the available time window from the CPF data. If the CPF is empty, the start and end
     * times are set to zero. Otherwise, the start and end times are obtained from the position records.
     *
     * @param mjd_start The output parameter to store the Modified Julian Date of the start time.
     * @param secs_start The output parameter to store the seconds of the day of the start time with decimals.
     * @param mjd_end The output parameter to store the Modified Julian Date of the end time.
     * @param secs_end The output parameter to store the seconds of the day of the end time with decimals.
     */
    void getAvailableTimeWindow(long long& mjd_start, long double &secs_start,
                                long long& mjd_end, long double& secs_end) const;

    /**
     * @brief Gets the available time window for the CPF as an Interval (will be invalid if the CPF is empty).
     *
     * This function retrieves the available time window from the CPF data. If the CPF is empty, the returned interval
     * will be invalid. Otherwise, the maximum and minimum of the interval are obtained from the position records.
     *
     * @return The available time interval as an instance of math::Interval<long double>.
     */
    math::Interval<long double> getAvailableTimeInterval() const;

    /**
     * @brief Checks if CPF is empty. A CPF will be empty if both header and data are empty. This can happen if an error
     * occurs at CPF reading, or if user explicitly clears data and header.
     *
     * @return true if CPF is empty, false otherwise.
     *
     * @todo Update the empty status while writing a CPF.
     */
    bool isEmpty() const;

    bool hasData() const;


    /**
     * @brief Custom getter that generates the filename using the CPF naming convention.
     *
     * This function generates a ILRS CPF standard convention filename. The convention is:
     * - For v1:
     *    targetname_cpf_yymmdd_nnnv.src (src is source, nnn sequence number and v sub-daily sequence number)
     *
     * - For v2:
     *    targetname_cpf_yymmdd_nnnvv.src (src is source, nnn sequence number and vv sub-daily sequence number)
     *
     * The targetname is the official name defined by ILRS. It can be changed using @p id_option
     *
     * @pre The Basic Info 1 Header (H1) must be defined.
     *      The Basic Info 2 Header (H2) must be defined.
     *
     * @param[in] id_option Determines which identifier will be used for the file name. Default is name.
     *
     * @return A string that contains the generated standard filename. If error, it returns an empty string.
     *
     * @note Changing the ID that appears in the file name is interesting, since different stations can use different
     *       target names, especially when tracking debris. Also, using other more standard IDs allows faster and
     *       simpler file search systems.
     */
    std::string getStandardFilename(TargetIdOptionEnum id_option = TargetIdOptionEnum::TARGET_NAME) const;

    /**
     * @brief Open a CPF file.
     *
     * This function opens an ILRS CPF file, and store the records in memory. Using the open
     * options, you can open the header records only, or all the records. This can be useful for fast opening while
     * searching certain files. This call doesn't modify the content of the file.
     *
     * @post This call will clear all the structs with the previous data, calling to clearCPF().
     *       This call will clear the read error vectors with the previous errors.
     *       This call will open the file, store the records in memory, and close the file.
     *
     * @param[in] cpf_filepath String with the complete path where the CPF file is stored.
     * @param[in] open_option Determines which structures will be read and stored.
     *
     * @return A @ref ReadFileErrorEnum value that contains the possible error.
     */
    ReadFileErrorEnum openCPFFile(const std::string& cpf_filepath, OpenOptionEnum open_option);

    /**
     * @brief Open data records of a previously opened CPF file.
     *
     * This function opens all records (including the data records) of an ILRS CPF file, and store
     * the records in memory. This function will call to @ref openCPFFile internally.
     *
     * @pre A CPF file MUST have been opened with @ref CPF(const std::string&, OpenOptionEnum) or the @ref openCPFFile,
     *      since this function will use the previously stored file path.
     *
     * @post This call will clear all the structs with the previous data, calling to clearCPF().
     *       This call will clear the read error vectors with the previous errors.
     *       This call will open the file, store the records in memory, and close the file.
     *
     * @return A @ref ReadFileErrorEnum value that contains the possible error.
     */
    ReadFileErrorEnum openCPFData();

    /**
     * @brief Writes a CPF file.
     *
     * This function writes the records stored in memory into an ILRS CPF file.
     *
     * @post
     *
     * @param[in] cpf_filepath String with the complete path where the CPF file will be written.
     * @param[in] force Boolean that indicates if the writing can be forced even if the file already exists.

     * @return A @ref WriteFileErrorEnum value that contains the possible error.
     */
    WriteFileErrorEnum writeCPFFile(const std::string& cpf_filepath, bool force = false);

    // Check integrity method.
    // TODO

private:

    // Private methods for reading records.
    // Return false if error.
    ReadRecordResultEnum readRecord(helpers::files::InputFileStream&, common::ConsolidatedRecord&);

    // Empty.
    bool empty_;

    // Data, configuration and header storage.
    CPFHeader header_;
    CPFData data_;

    // Private members for store read errors.
    common::RecordReadErrorMultimap read_header_errors_;
    common::RecordReadErrorMultimap read_data_errors_;
    ReadFileErrorEnum last_read_error_;
    Optional<common::ConsolidatedRecord> last_error_record_;

    // File names and path.
    std::string cpf_filename_;
    std::string cpf_fullpath_;
};
// =====================================================================================================================

}}} // END NAMESPACES.
// =====================================================================================================================
