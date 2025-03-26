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
 * @file crd_configuration.h
 * @author Degoras Project Team.
 * @brief This file contains the declaration of the CRDConfiguration class that abstracts part of the ILRS CRD format.
 * @copyright EUPL License
 2305.1
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
// =====================================================================================================================
#include <string>
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

// CRD CONFIGURATION
// =====================================================================================================================

class LIBDPSLR_EXPORT CRDConfiguration
{
public:

    // CRD CONFIGURATION ENUMS
    // -----------------------------------------------------------------------------------------------------------------

    enum class ConfigurationLine
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
    struct SystemConfiguration : common::ConsolidatedRecord
    {
        // Members.
        unsigned int detail_type;
        double transmit_wavelength;  // Nanometers.
        std::string system_cfg_id;
        // A, B, C... dynamically generated using all IDs of other configuration structs.
    };

    // C1 - Laser Configuration Record (required)
    struct LaserConfiguration : common::ConsolidatedRecord
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
    struct DetectorConfiguration : common::ConsolidatedRecord
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
        dpbase::Optional<double> spatial_filter;   // arcsec
        std::string ext_signal_proc;                      // TODO: ?
        dpbase::Optional<double> amp_gain;         // TODO: ? db?                             [For v2] [optional]
        dpbase::Optional<double> amp_bandwidth;    // kHz.                                    [For v2] [optional]
        dpbase::Optional<bool> used_amp;           // Used amplificator flag.                 [For v2] [optional]
        // Functions.
        std::string generateLine(float version) const;
    };

    // C3 - Timing Configuration (required)
    struct TimingConfiguration : common::ConsolidatedRecord
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

    // C4 - Trnasponder Configuration
    // TODO
    struct TransponderConfiguration : common::ConsolidatedRecord
    {
        // Members.
        unsigned int detail;            // Always 0.
        std::string cfg_id;             // Transponder configuration ID
        // Functions.
        std::string generateLine(float version) const;
    };

    // C5 - Software Configuration
    struct SoftwareConfiguration : common::ConsolidatedRecord
    {
        // Members.
        unsigned int detail;                              // Always 0
        std::string cfg_id;                               // Software configuration ID
        dpbase::Optional<std::string> tracking_sw;        // Tracking software. Can be more than one, comma separated
        dpbase::Optional<std::string> tracking_sw_ver;    // Version of tracking software
        dpbase::Optional<std::string> processing_sw;      // Processing software. Can be more than one, comma separated
        dpbase::Optional<std::string> processing_sw_ver;  // Version of processing software
        // Functions.
        std::string generateLine(float version) const;
    };

    // C6 - Meteorological Configuration
    struct MeteorologicalConfiguration : common::ConsolidatedRecord
    {
        // Members.
        unsigned detail;                                  // Always 0
        std::string cfg_id;                               // Meteorological configuration ID
        dpbase::Optional<std::string> press_manufacturer; // Pressure sensor manufacturer
        dpbase::Optional<std::string> press_model;        // Pressure sensor model
        dpbase::Optional<std::string> press_sn;           // Pressure sensor serial number
        dpbase::Optional<std::string> temp_manufacturer;  // Temperature sensor manufacturer
        dpbase::Optional<std::string> temp_model;         // Temperature sensor model
        dpbase::Optional<std::string> temp_sn;            // Temperature sensor serial number
        dpbase::Optional<std::string> humid_manufacturer; // Humidity sensor manufacturer
        dpbase::Optional<std::string> humid_model;        // Humidity sensor model
        dpbase::Optional<std::string> humid_sn;           // Humidity sensor serial number
        // Functions.
        std::string generateLine(float version) const;
    };

    // C7 - Calibration Target Configuration
    struct CalibrationTargetConfiguration : common::ConsolidatedRecord
    {
        // Members.
        unsigned int detail;                              // Always 0
        std::string cfg_id;                               // Calibration configuration ID
        std::string target_name;                          // Name of target used for calibration
        double distance;                                  // Surveyed distance to target (m, one way)
        dpbase::Optional<double> error;                   // Surveyed distance error (mm)
        dpbase::Optional<double> delays;                  // Sum of constant delays not measured (m, one way)
        dpbase::Optional<double> energy;                  // Pulse energy (mJ)
        dpbase::Optional<std::string> processing_sw;      // Processing software. Can be more than one, comma separated.
        dpbase::Optional<std::string> processing_sw_ver;  // Version of processing software.
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
    void clearCalibrationTargetConfiguration();

    // Configuration const getters.
    const dpbase::Optional<SystemConfiguration> &systemConfiguration() const;
    const dpbase::Optional<LaserConfiguration> &laserConfiguration() const;
    const dpbase::Optional<DetectorConfiguration> &detectorConfiguration() const;
    const dpbase::Optional<TimingConfiguration> &timingConfiguration() const;
    const dpbase::Optional<TransponderConfiguration> &transponderConfiguration() const;
    const dpbase::Optional<SoftwareConfiguration> &softwareConfiguration() const;
    const dpbase::Optional<MeteorologicalConfiguration> &meteorologicalConfiguration() const;
    const dpbase::Optional<CalibrationTargetConfiguration> &calibrationTargetConfiguration() const;

    // Configuration no const getters.
    dpbase::Optional<SystemConfiguration> &systemConfiguration();
    dpbase::Optional<LaserConfiguration> &laserConfiguration();
    dpbase::Optional<DetectorConfiguration> &detectorConfiguration();
    dpbase::Optional<TimingConfiguration> &timingConfiguration();
    dpbase::Optional<TransponderConfiguration> &transponderConfiguration();
    dpbase::Optional<SoftwareConfiguration> &softwareConfiguration();
    dpbase::Optional<MeteorologicalConfiguration> &meteorologicalConfiguration();
    dpbase::Optional<CalibrationTargetConfiguration> &calibrationTargetConfiguration();

    // Configuration setters.
    void setSystemConfiguration(const SystemConfiguration&);
    void setLaserConfiguration(const LaserConfiguration&);
    void setDetectorConfiguration(const DetectorConfiguration&);
    void setTimingConfiguration(const TimingConfiguration&);
    void setTransponderConfiguration(const TransponderConfiguration&);
    void setSoftwareConfiguration(const SoftwareConfiguration&);
    void setMeteorologicalConfiguration(const MeteorologicalConfiguration&);
    void setCalibrationTargetConfiguration(const CalibrationTargetConfiguration&);

    // Generate CRD configuration lines.
    std::string generateConfigurationLines(float version) const;

    // Generic read methods.
    common::RecordReadErrorMultimap readConfiguration(const common::RecordLinesVector&, float version);

    // Specific read methods.
    common::RecordReadError readSystemCFG(const common::ConsolidatedRecord&, float version);       // C0
    common::RecordReadError readLaserCFG(const common::ConsolidatedRecord&, float version);        // C1
    common::RecordReadError readDetectorCFG(const common::ConsolidatedRecord&, float version);     // C2
    common::RecordReadError readTimingCFG(const common::ConsolidatedRecord&, float version);       // C3
    common::RecordReadError readTransponderCFG(const common::ConsolidatedRecord&, float version);  // C4
    common::RecordReadError readSoftwareCFG(const common::ConsolidatedRecord&, float version);     // C5
    common::RecordReadError readMeteoCFG(const common::ConsolidatedRecord&, float version);        // C6
    common::RecordReadError readCalibrationCFG(const common::ConsolidatedRecord&, float version);  // C7

private:

    // Generic private read method.
    common::RecordReadError readConfigurationLine(const common::RecordLinePair& rpair, float version);

    // Private method to generate A, B, C... of C0 line.
    std::string generateSystemCfgLine(float version) const;

    // Storage CRD configuration data. ( C0 to C7 )
    dpbase::Optional<SystemConfiguration> system_cfg;
    dpbase::Optional<LaserConfiguration> laser_cfg;
    dpbase::Optional<DetectorConfiguration> detector_cfg;
    dpbase::Optional<TimingConfiguration> timing_cfg;
    dpbase::Optional<TransponderConfiguration> transponder_cfg;
    dpbase::Optional<SoftwareConfiguration> software_cfg;
    dpbase::Optional<MeteorologicalConfiguration> meteorological_cfg;
    dpbase::Optional<CalibrationTargetConfiguration> calibration_cfg;
};

// =====================================================================================================================

}}} // END NAMESPACES
// =====================================================================================================================
