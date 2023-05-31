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
#include "LibDPSLR/libdpslr_global.h"
#include "LibDPSLR/Helpers/common_aliases.h"
#include "LibDPSLR/ILRSFormats/common/consolidated_types.h"
#include "LibDPSLR/ILRSFormats/common/consolidated_record.h"
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
        Optional<double> spatial_filter;   // arcsec
        std::string ext_signal_proc;                      // TODO: ?
        Optional<double> amp_gain;         // TODO: ? db?                             [For v2] [optional]
        Optional<double> amp_bandwidth;    // kHz.                                    [For v2] [optional]
        Optional<bool> used_amp;           // Used amplificator flag.                 [For v2] [optional]
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
    // TODO
    struct SoftwareConfiguration : common::ConsolidatedRecord
    {
        // Members.
        unsigned int detail;            // Always 0
        std::string cfg_id;             // Software configuration ID
        // Functions.
        std::string generateLine(float version) const;
    };

    // C6 - Meteorological Configuration
    struct MeteorologicalConfiguration : common::ConsolidatedRecord
    {
        // Members.
        unsigned detail;                          // Always 0
        std::string cfg_id;                       // Meteorological configuration ID
        Optional<std::string> press_manufacturer; // Pressure sensor manufacturer
        Optional<std::string> press_model;        // Pressure sensor model
        Optional<std::string> press_sn;           // Pressure sensor serial number
        Optional<std::string> temp_manufacturer;  // Temperature sensor manufacturer
        Optional<std::string> temp_model;         // Temperature sensor model
        Optional<std::string> temp_sn;            // Temperature sensor serial number
        Optional<std::string> humid_manufacturer; // Humidity sensor manufacturer
        Optional<std::string> humid_model;        // Humidity sensor model
        Optional<std::string> humid_sn;           // Humidity sensor serial number
        // Functions.
        std::string generateLine(float version) const;
    };

    // C7 - Calibration Configuration
    // TODO
    struct CalibrationConfiguration : common::ConsolidatedRecord
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
    const Optional<SystemConfiguration> &systemConfiguration() const;
    const Optional<LaserConfiguration> &laserConfiguration() const;
    const Optional<DetectorConfiguration> &detectorConfiguration() const;
    const Optional<TimingConfiguration> &timingConfiguration() const;
    const Optional<TransponderConfiguration> &transponderConfiguration() const;
    const Optional<SoftwareConfiguration> &softwareConfiguration() const;
    const Optional<MeteorologicalConfiguration> &meteorologicalConfiguration() const;
    const Optional<CalibrationConfiguration> &calibrationConfiguration() const;

    // Configuration no const getters.
    Optional<SystemConfiguration> &systemConfiguration();
    Optional<LaserConfiguration> &laserConfiguration();
    Optional<DetectorConfiguration> &detectorConfiguration();
    Optional<TimingConfiguration> &timingConfiguration();
    Optional<TransponderConfiguration> &transponderConfiguration();
    Optional<SoftwareConfiguration> &softwareConfiguration();
    Optional<MeteorologicalConfiguration> &meteorologicalConfiguration();
    Optional<CalibrationConfiguration> &calibrationConfiguration();

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
    Optional<SystemConfiguration> system_cfg;
    Optional<LaserConfiguration> laser_cfg;
    Optional<DetectorConfiguration> detector_cfg;
    Optional<TimingConfiguration> timing_cfg;
    Optional<TransponderConfiguration> transponder_cfg;
    Optional<SoftwareConfiguration> software_cfg;
    Optional<MeteorologicalConfiguration> meteorological_cfg;
    Optional<CalibrationConfiguration> calibration_cfg;
};

// =====================================================================================================================

}}} // END NAMESPACES
// =====================================================================================================================
