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
 * @file crd_data.h
 * @author Degoras Project Team.
 * @brief This file contains the declaration of the CRDData class that abstracts the data of ILRS CRD format.
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
#include <vector>
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
#include "LibDegorasSLR/FormatsILRS/common/consolidated_types.h"
#include "LibDegorasSLR/FormatsILRS/common/consolidated_record.h"
// =====================================================================================================================

// LIBDPBASE INCLUDES
// =====================================================================================================================
#include "LibDegorasBase/Helpers/common_aliases_macros.h"
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace ilrs{
namespace crd{
// =====================================================================================================================

// CRD DATA
// =====================================================================================================================

class LIBDPSLR_EXPORT CRDData
{
public:

    // CRD DATA LINES
    // -----------------------------------------------------------------------------------------------------------------
    static const std::array<const char*, 10> DataLineString;
    // -----------------------------------------------------------------------------------------------------------------

    // CRD DATA ENUMS
    // -----------------------------------------------------------------------------------------------------------------

    /// @enum DataRecordType
    /// @brief Different data records of the CRD format.
    /// @todo Implementation of the following records is missing:
    ///       RANGE_SUPPLEMENT_RECORD
    ///       METEO_SUP_RECORD
    ///       POINTING_ANGLES_RECORD
    ///       CALIBRATION_SHOT_RECORD
    enum class DataRecordType
    {
        FULL_RATE_RECORD            = 0,  /// Line 10. Range record.
        NORMAL_POINT_RECORD         = 1,  /// Line 11. Normal point range record.
        RANGE_SUPPLEMENT_RECORD     = 2,  /// Line 12. Range supplement record.
        METEO_RECORD                = 3,  /// Line 20. Meteorological record.
        METEO_SUP_RECORD            = 4,  /// Line 21. Meteorological supplement record.
        POINTING_ANGLES_RECORD      = 5,  /// Line 30. Pointing angle record.
        CALIBRATION_OVERALL_RECORD  = 6,  /// Line 40. Calibration overall record.
        CALIBRATION_DETAIL_RECORD   = 7,  /// Line 41. Calibration detail record. For v2 only.
        CALIBRATION_SHOT_RECORD     = 8,  /// Line 42. Calibration shot record. For v2 only.
        STATISTICS_RECORD           = 9,  /// Line 50. Statistic record.
        COMPATIBILITY_RECORD        = 10, /// Line 60. Deprecated and not used.
    };

    enum class EpochEvent
    {
        SPACECRAFT_BOUNCE_TIME_2W = 1,
        GROUND_TRANSMIT_TIME_2W   = 2, // TODO other values
    };

    enum class FilterFlag
    {
        UNKNOWN = 0,
        NOISE_EXCLUDED_RETURN = 1,
        DATA   = 2,
    };

    enum class MeteoOrigin
    {
        MEASURED = 0,
        INTERPOLATED = 1,
    };

    enum class DataType
    {
        STATION_COMBINED = 0,
        STATION_TRANSMIT = 1,
        STATION_RECEIVE  = 2,
        TARGET_COMBINED  = 3,
        TARGET_TRANSMIT  = 4,
        TARGET_RECIEVE   = 5
    };

    enum class CalibrationType
    {
        CAL_UNDEFINED          = 0,
        NOMINAL                = 1,
        EXTERNAL               = 2,
        INTERNAL_V1_TELESCOPE  = 3,  // V1: Internal. V2: Internal Telescope
        BURST_V1_BUILDING      = 4,  // V1: Burst. V2: Internal Building
        OTHER_V1_BURST         = 5,  // V1: Other. V2: Burst calibrations
        CAL_OTHER              = 6   // V2 only: Other
    };

    enum class ShiftType
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
    enum class DataQuality
    {
        AUTO_QUALITY = -1,              // For calculating the quality using the internal methods. [TODO]
        UNDEFINED_QUALITY = 0,
        CLEAR_NO_NOISE = 1,
        CLEAR_SOME_NOISE = 2,
        CLEAR_SIGNIFICANT_NOISE = 3,
        UNCLEAR = 4,
        NOT_DATA = 5
    };

    enum class DataGenerationOption
    {
        FULL_RATE,
        NORMAL_POINT,
        BOTH_DATA
    };
    // -----------------------------------------------------------------------------------------------------------------

    // CRD DATA STRUCTS                                                          // TODO: Rest of the structs and lines.
    // -----------------------------------------------------------------------------------------------------------------
    // 10 - FULL RATE DATA
    struct FullRateRecord : common::ConsolidatedRecord
    {
        // Members.
        long double time_tag;        // Second of day (12 decimals). 100 ns precission for SLR/LLR. 1 ps for others.
        long double time_flight;     // Can be none, one-, two-way, etc. It can be corrected. Seconds with 12 decimals.
        std::string system_cfg_id;                       // System configuration ID.
        EpochEvent epoch_event;                      // Indicates the time event reference.
        FilterFlag filter_flag;                      // Indicates the filter type of this record.
        unsigned int detector_channel;                   // 0 for na or all, 1-4 for quadrant, 1-n for many channels.
        unsigned int stop_number;                        // For multiple stop numbers.
        dpbase::Optional<unsigned> receive_amp;   // Linear scale value.                               [optional]
        dpbase::Optional<unsigned> transmit_amp;  // Linear scale value.                      [For v2] [optional]
        // Functions.
        std::string generateLine(float version) const;
    };

    // 11 - NORMAL POINT DATA
    struct NormalPointRecord : common::ConsolidatedRecord
    {
        // Members.
        long double time_tag;                     // Second of day (12 decimals). Clock correction should be applied.
        long double time_flight;                  // Should be can be corrected for calibration delay.
        std::string system_cfg_id;                // System configuration ID.
        EpochEvent epoch_event;               // Indicates the time event reference.
        double window_length;                     // Seconds.
        unsigned int raw_ranges;                  // Raw ranges compressed into the normal point.
        dpbase::Optional<double> bin_rms;      // From mean of raw minus the trend function (ps).      [optional]
        dpbase::Optional<double> bin_skew;     // From mean of raw minus the trend function.           [optional]
        dpbase::Optional<double> bin_kurtosis; // From mean of raw minus the trend function.           [optional]
        dpbase::Optional<double> bin_peak;     // Bin peak - mean value (ps).    [non-standard explicit optional]
        double return_rate;                       // %
        unsigned int detector_channel;            // 0 for na or all, 1-4 for quadrant, 1-n for many channels.
        dpbase::Optional<double> snr;          // S:N Signal to noise ratio.                      [For v2] [optional]
        // Functions.
        std::string generateLine(float version) const;
    };

    // 20 - METEOROLOGICAL DATA
    struct MeteorologicalRecord : common::ConsolidatedRecord
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
        MeteoOrigin values_origin;        // 0 - measured, 1 - interpolated
        // Functions.
        std::string generateLine(float version) const;
    };

    // 40 and 41 - CALIBRATION DATA
    struct CalibrationRecord : common::ConsolidatedRecord
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
        DataType data_type;         // Usually station combined for SLR/LLR.
        std::string system_cfg_id;      // System configuration ID.
        dpbase::Optional<int> data_recorded;      // Number of data recorded.                          [optional]
        dpbase::Optional<int> data_used;          // Number of data point used.                        [optional]
        dpbase::Optional<double> target_dist_1w;  // Meters, nominal, one way.                         [optional]
        double calibration_delay;       // Calibration system delay (ps), 1 decimal.
        double delay_shift;             // Calibration stability (ps), 1 decimal. 0 for line 41
        double rms;                     // RMS (ps)
        double skew;                    // Skew from the mean
        double kurtosis;                // Kurtosis from the mean
        double peak;                    // peak Mean value (ps)
        CalibrationType cal_type;   // Calibration type.
        ShiftType shift_type;       // Usually pre to post or minimum to maximum.
        unsigned int detector_channel;  // 0 for na or all, 1-4 for quadrant, 1-n for many channels.
        CalibrationSpan span;           // Span of the records.                                      [For v2] [optional]
        dpbase::Optional<double> return_rate; // %                                            [For v2] [optional]
        bool is_overall = false;        // True: The struct is an overall cal (line 40). False: is detail cal.

        // Functions.
        std::string generateLine(float version) const;   // Generate lines 40-41
    };

    // 50 - STATISTICS DATA
    struct StatisticsRecord : common::ConsolidatedRecord
    {
        std::string system_cfg_id;
        double rms;
        dpbase::Optional<double>  skew;
        dpbase::Optional<double>  kurtosis;
        dpbase::Optional<double>  peak;
        DataQuality quality;

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
    common::FlightTimeData fullRateFlightTimeData() const;
    common::FlightTimeData normalPointFlightTimeData() const;

    const std::vector<FullRateRecord> &fullRateRecords() const;
    const std::vector<NormalPointRecord> &normalPointRecords() const;
    const std::vector<MeteorologicalRecord> &meteorologicalRecords() const;
    const std::vector<CalibrationRecord> &realTimeCalibrationRecord() const;
    const std::vector<CalibrationRecord> &calibrationRecords() const;
    const dpbase::Optional<CalibrationRecord> &calibrationOverallRecord() const;
    const dpbase::Optional<StatisticsRecord> &statisticsRecord() const;

    // Non-const getters
    std::vector<FullRateRecord> &fullRateRecords();
    std::vector<NormalPointRecord> &normalPointRecords();
    std::vector<MeteorologicalRecord> &meteorologicalRecords();
    std::vector<CalibrationRecord> &realTimeCalibrationRecord();
    std::vector<CalibrationRecord> &calibrationRecords();
    dpbase::Optional<CalibrationRecord> &calibrationOverallRecord();
    dpbase::Optional<StatisticsRecord> &statisticsRecord();

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
    std::string generateDataLines(float version, DataGenerationOption option) const;

    // Generate specific CRD lines. Data must be sorted.
    std::string generateFullRateLines(float version) const;       // Line 10              TODO: Sort function in future.
    std::string generateNormalPointLines(float version) const;    // Line 11              TODO: Sort function in future.
    std::string generateMeteoLines(float version) const;          // Line 20              TODO: Sort function in future.
    std::string generateCalibrationLines(float version) const;    // Lines 40 41          TODO: Sort function in future.
    std::string generateStatisticsLine(float version) const;      // Line 50

    // Generic read methods.
    // Calls to clearAll()
    common::RecordReadErrorMultimap readData(const common::RecordLinesVector &rec_v, float version);

    // Specific read methods.
    // The read data-line functions append the data to the proper vector. Do nothing if error.
    // The read data functions clear the pointers (struct) and create a new one (or nullptr if error).
    common::RecordReadError readFRDataLine(const common::ConsolidatedRecord&, float v);       // Line 10
    common::RecordReadError readNPDataLine(const common::ConsolidatedRecord&, float v);       // Line 11
    common::RecordReadError readMeteoDataLine(const common::ConsolidatedRecord&, float v);    // Line 20
    common::RecordReadError readCalDataLine(const common::ConsolidatedRecord&, float v);  // Lines 40 41
    common::RecordReadError readStatisticsData(const common::ConsolidatedRecord&, float v);   // Line 50


    //TODO
    // Integrity Checker.
    //// Si v2 y no overall, no puede haber delay shift.
    // Funcion para interpolar datos de meteorologia si es necesario.
    // Funcion para calcular puntos normales.


private:

    // Generic private read method.
    common::RecordReadError readDataLine(const common::RecordLinePair &rpair, float version);

    // Private vectors for store the different data records.
    std::vector<FullRateRecord> fullrate_records;              // Full rate record.
    std::vector<NormalPointRecord> normalpoint_records;        // Normal point record vector.
    std::vector<MeteorologicalRecord> meteo_records;           // Meteo records vector.
    std::vector<CalibrationRecord> rt_cal_records;             // Real time calibrations (for v2).
    std::vector<CalibrationRecord> cal_records;                // Calibration records (line 40 for v1, 41 for v2)

    // Calibration overall record (for v2) and statistics record storage.
    dpbase::Optional<CalibrationRecord> cal_overall_record;
    dpbase::Optional<StatisticsRecord> stat_record;
};

// =====================================================================================================================

}}} // END NAMESPACES
// =====================================================================================================================
