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
 * @file crd_configuration.cpp
 * @author Degoras Project Team.
 * @brief This file contains the implementation of the CRDConfiguration class that abstracts part of ILRS CRD format.
 * @copyright EUPL License
 2305.1
***********************************************************************************************************************/

// C++ INCLUDES
// =====================================================================================================================#include <fstream>
#include <iostream>
#include <iomanip>
#include <numeric>
#include <cstring>
#include <algorithm>
#include <array>
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include <LibDegorasSLR/FormatsILRS/crd/records/crd_configuration.h>
#include <LibDegorasSLR/FormatsILRS/common/consolidated_types.h>
// =====================================================================================================================

// LIBDPBASE INCLUDES
// =====================================================================================================================
#include <LibDegorasBase/Helpers/container_helpers.h>
#include <LibDegorasBase/Helpers/string_helpers.h>
// =====================================================================================================================

// =====================================================================================================================
using namespace dpslr::ilrs::common;
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace ilrs{
namespace crd{
// =====================================================================================================================

// CRD CONFIGURATION
// =====================================================================================================================

void CRDConfiguration::clearAll()
{
    this->clearSystemConfiguration();
    this->clearLaserConfiguration();
    this->clearDetectorConfiguration();
    this->clearTimingConfiguration();
    this->clearTransponderConfiguration();
    this->clearSoftwareConfiguration();
    this->clearMeteorologicalConfiguration();
    this->clearCalibrationTargetConfiguration();
}

void CRDConfiguration::clearSystemConfiguration() {this->system_cfg = {};}

void CRDConfiguration::clearLaserConfiguration(){this->laser_cfg = {};}

void CRDConfiguration::clearDetectorConfiguration() {this->detector_cfg = {};}

void CRDConfiguration::clearTimingConfiguration() {this->timing_cfg = {};}

void CRDConfiguration::clearTransponderConfiguration(){this->transponder_cfg = {};}

void CRDConfiguration::clearSoftwareConfiguration() {this->software_cfg = {};}

void CRDConfiguration::clearMeteorologicalConfiguration() {this->meteorological_cfg = {};}

void CRDConfiguration::clearCalibrationTargetConfiguration() {this->calibration_cfg = {};}

const dpbase::Optional<CRDConfiguration::SystemConfiguration>& CRDConfiguration::systemConfiguration() const
{return this->system_cfg;}

const dpbase::Optional<CRDConfiguration::LaserConfiguration>& CRDConfiguration::laserConfiguration() const
{return this->laser_cfg;}

const dpbase::Optional<CRDConfiguration::DetectorConfiguration>& CRDConfiguration::detectorConfiguration() const
{return this->detector_cfg;}

const dpbase::Optional<CRDConfiguration::TimingConfiguration>& CRDConfiguration::timingConfiguration() const
{return this->timing_cfg;}

const dpbase::Optional<CRDConfiguration::TransponderConfiguration>& CRDConfiguration::transponderConfiguration()
const {return this->transponder_cfg;}

const dpbase::Optional<CRDConfiguration::SoftwareConfiguration>& CRDConfiguration::softwareConfiguration() const
{return this->software_cfg;}

const dpbase::Optional<CRDConfiguration::MeteorologicalConfiguration>& CRDConfiguration::meteorologicalConfiguration() const
{return  this->meteorological_cfg;}

const dpbase::Optional<CRDConfiguration::CalibrationTargetConfiguration>&
CRDConfiguration::calibrationTargetConfiguration() const {return  this->calibration_cfg;}

dpbase::Optional<CRDConfiguration::SystemConfiguration>& CRDConfiguration::systemConfiguration()
{return this->system_cfg;}

dpbase::Optional<CRDConfiguration::LaserConfiguration>& CRDConfiguration::laserConfiguration()
{return this->laser_cfg;}

dpbase::Optional<CRDConfiguration::DetectorConfiguration>& CRDConfiguration::detectorConfiguration()
{return this->detector_cfg;}

dpbase::Optional<CRDConfiguration::TimingConfiguration>& CRDConfiguration::timingConfiguration()
{return this->timing_cfg;}

dpbase::Optional<CRDConfiguration::TransponderConfiguration>& CRDConfiguration::transponderConfiguration()
{return this->transponder_cfg;}

dpbase::Optional<CRDConfiguration::SoftwareConfiguration>& CRDConfiguration::softwareConfiguration()
{return this->software_cfg;}

dpbase::Optional<CRDConfiguration::MeteorologicalConfiguration>& CRDConfiguration::meteorologicalConfiguration()
{return this->meteorological_cfg;}

dpbase::Optional<CRDConfiguration::CalibrationTargetConfiguration>& CRDConfiguration::calibrationTargetConfiguration()
{return this->calibration_cfg;}

void CRDConfiguration::setSystemConfiguration(const CRDConfiguration::SystemConfiguration& sc) {this->system_cfg = sc;}

void CRDConfiguration::setLaserConfiguration(const CRDConfiguration::LaserConfiguration& lc) {this->laser_cfg = lc;}

void CRDConfiguration::setDetectorConfiguration(const CRDConfiguration::DetectorConfiguration& dc)
{this->detector_cfg = dc;}

void CRDConfiguration::setTimingConfiguration(const CRDConfiguration::TimingConfiguration& tc) {this->timing_cfg = tc;}

void CRDConfiguration::setTransponderConfiguration(const CRDConfiguration::TransponderConfiguration& tc)
{this->transponder_cfg = tc;}

void CRDConfiguration::setSoftwareConfiguration(const CRDConfiguration::SoftwareConfiguration& sc)
{this->software_cfg = sc;}

void CRDConfiguration::setMeteorologicalConfiguration(const CRDConfiguration::MeteorologicalConfiguration& mc)
{this->meteorological_cfg = mc;}

void CRDConfiguration::setCalibrationTargetConfiguration(const CRDConfiguration::CalibrationTargetConfiguration& cc)
{this->calibration_cfg = cc;}

std::string CRDConfiguration::generateConfigurationLines(float version) const
{
    // Header line
    std::stringstream cfg_lines;

    // For v1 and 2.
    if(version >= 1 && version <=3)
    {
        // Add the system cfg comment block.
        if(!this->system_cfg->comment_block.empty())
            cfg_lines << this->system_cfg->generateCommentBlock() << std::endl;

        cfg_lines << this->generateSystemCfgLine(version) << std::endl;

        if(this->laser_cfg)
        {
            // Add the laser cfg comment block.
            if(!this->laser_cfg->comment_block.empty())
                cfg_lines << this->laser_cfg->generateCommentBlock() << std::endl;

            cfg_lines << this->laser_cfg->generateLine(version) << std::endl;
        }

        if(this->detector_cfg)
        {
            // Add the detector cfg comment block.
            if(!this->detector_cfg->comment_block.empty())
                cfg_lines << this->detector_cfg->generateCommentBlock() << std::endl;

            cfg_lines << this->detector_cfg->generateLine(version) << std::endl;
        }

        if(this->timing_cfg)
        {
            // Add the timing cfg comment block.
            if(!this->timing_cfg->comment_block.empty())
                cfg_lines << this->timing_cfg->generateCommentBlock() << std::endl;

            cfg_lines << this->timing_cfg->generateLine(version) << std::endl;
        }

        if(this->transponder_cfg)
        {
            // Add the transponder cfg comment block.
            if(!this->transponder_cfg->comment_block.empty())
                cfg_lines << this->transponder_cfg->generateCommentBlock() << std::endl;

            cfg_lines << this->transponder_cfg->generateLine(version) << std::endl;
        }
    }

    // For v2 only.
    if(version >= 2 && version <=3)
    {
        if(this->software_cfg)
        {
            // Add the system cfg comment block.
            if(!this->software_cfg->comment_block.empty())
                cfg_lines << this->software_cfg->generateCommentBlock() << std::endl;

            cfg_lines << this->software_cfg->generateLine(version) << std::endl;
        }

        if(this->meteorological_cfg)
        {
            // Add the meteo cfg comment block.
            if(!this->meteorological_cfg->comment_block.empty())
                cfg_lines << this->meteorological_cfg->generateCommentBlock() << std::endl;

            cfg_lines << this->meteorological_cfg->generateLine(version) << std::endl;
        }

        if(this->calibration_cfg)
        {
            // Add the calibration cfg comment block.
            if(!this->calibration_cfg->comment_block.empty())
                cfg_lines << this->calibration_cfg->generateCommentBlock() << std::endl;

            cfg_lines << this->calibration_cfg->generateLine(version) << std::endl;
        }
    }

    return cfg_lines.str().substr(0, cfg_lines.str().find_last_of('\n'));
}

// Generic reading configuration function.
RecordReadErrorMultimap CRDConfiguration::readConfiguration(const RecordLinesVector& rec_v, float v)
{
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
        if(dpbase::helpers::containers::find(CfgIdStr, rec.getIdToken(), pos))
        {
            // Store the record type in a pair.
            rec_pair = {pos, rec};

            // Read the record to store the data in memory.
            error = this->readConfigurationLine(rec_pair, v);

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

// Specific function for reading C0.
RecordReadError CRDConfiguration::readSystemCFG(const ConsolidatedRecord& record, float)
{
    // Variables.
    RecordReadError result = RecordReadError::NOT_ERROR;

    // Tokens are: C0, DETAIL, TRANSMIT WAVE, CFG ID, A, B, C, D, E, F, G
    // Get the tokens.
    std::vector<std::string> tokens = record.tokens;

    // Delete the current data.
    this->clearSystemConfiguration();

    // Check if size is correct.
    if (tokens.size() < 7 || tokens.size() > 11)
        result = RecordReadError::BAD_SIZE;
    // Check if the record type is correct.
    else if (dpbase::helpers::strings::toUpper(tokens[0]) != CfgIdStr[static_cast<int>(ConfigurationLine::SYSTEM_CFG)])
        result = RecordReadError::BAD_TYPE;
    // All ok at this momment.
    else
        try
        {
            // New system configuration struct.
            SystemConfiguration sc;

            // Get the data.
            sc.detail_type = std::stoi(tokens[1]);
            sc.transmit_wavelength = std::stod(tokens[2]);
            sc.system_cfg_id = tokens[3];

            // Add the associated comments, the line number, and the tokens.
            sc.comment_block = record.comment_block;
            sc.line_number = record.line_number;
            sc.tokens = record.tokens;

            // Store the system configuration struct
            this->system_cfg = std::move(sc);

        } catch (...)
        {
            result = RecordReadError::CONVERSION_ERROR;
        }

    // Return the result.
    return result;
}

// Specific function for reading C1.
RecordReadError CRDConfiguration::readLaserCFG(
        const ConsolidatedRecord& record, float)
{
    // Variables.
    RecordReadError result = RecordReadError::NOT_ERROR;

    // Tokens are: C1, DETAIL, CFG ID, TYPE, PRIM_WAVE, FREQ, ENERGY, PULSE_WIDTH, DIVERGENCE, PULSES_SEMITRAIN
    // Get the tokens.
    std::vector<std::string> tokens = record.tokens;

    // Delete the current data.
    this->clearLaserConfiguration();

    // Check if size is correct.
    if (tokens.size() != 10)
        result = RecordReadError::BAD_SIZE;
    // Check if the record type is correct.
    else if (dpbase::helpers::strings::toUpper(tokens[0]) != CfgIdStr[static_cast<int>(ConfigurationLine::LASER_CFG)])
        result = RecordReadError::BAD_TYPE;
    // All ok at this momment.
    else
        try
        {
            // New laser configuration struct
            LaserConfiguration lc;

            // Get the data.
            lc.detail = std::stoi(tokens[1]);
            lc.cfg_id =tokens[2];
            lc.type = tokens[3];
            lc.primary_wavelength = std::stod(tokens[4]);
            lc.fire_rate = std::stod(tokens[5]);
            lc.pulse_energy = std::stod(tokens[6]);
            lc.pulse_width = std::stod(tokens[7]);
            lc.beam_divergence = std::stod(tokens[8]);
            lc.pulses_outgoing_semitrain = std::stoi(tokens[9]);

            // Add the associated comments, the line number, and the tokens.
            lc.comment_block = record.comment_block;
            lc.line_number = record.line_number;
            lc.tokens = record.tokens;

            // Store the laser configuration struct
            this->laser_cfg = std::move(lc);

        } catch (...)
        {
            result = RecordReadError::CONVERSION_ERROR;
        }

    // Return the result.
    return result;
}

// Specific function for reading C2.
RecordReadError CRDConfiguration::readDetectorCFG(const ConsolidatedRecord& record, float v)
{
    // Variables.
    RecordReadError result = RecordReadError::NOT_ERROR;

    // Get the tokens.
    std::vector<std::string> tokens = record.tokens;

    // Delete the current data.
    this->clearDetectorConfiguration();

    // Check the data size for each version.
    // For v1.
    if (v >= 1 && v < 2 && tokens.size() != 14)
        result = RecordReadError::BAD_SIZE;

    // For v2.
    else if(v >= 2 && v < 3 && tokens.size() != 17)
        result = RecordReadError::BAD_SIZE;

    // Check if the record type is correct.
    else if (dpbase::helpers::strings::toUpper(tokens[0]) != CfgIdStr[static_cast<int>(ConfigurationLine::DETECTOR_CFG)])
        result = RecordReadError::BAD_TYPE;

    // All ok at this momment.
    else
        try
        {
            // New detector configuration struct
            DetectorConfiguration dc;

            // Common for v1.
            // Get the data.
            dc.detail = std::stoi(tokens[1]);
            dc.cfg_id = tokens[2];
            dc.type = tokens[3];
            dc.work_wavelength = std::stod(tokens[4]);
            dc.efficiency = std::stod(tokens[5]);
            dc.voltage = std::stod(tokens[6]);
            dc.dark_count = std::stod(tokens[7]);
            dc.out_pulse_type = tokens[8];
            dc.out_pulse_width = std::stod(tokens[9]);
            dc.spectral_filter = std::stod(tokens[10]);
            dc.transm_spectral_filter = std::stod(tokens[11]);
            dc.ext_signal_proc = tokens[13];

            // For v1
            if (v >= 1 && v < 2)
            {
                // TODO: check integrity. It should be positive
                dc.spatial_filter = ("-1" == tokens[12] ? decltype(dc.spatial_filter)() : std::stod(tokens[12]));
            }
            // For v2.
            else if(v >= 2 && v < 3 )
            {
                dc.spatial_filter = ("na" == tokens[12] ? decltype(dc.spatial_filter)() : std::stod(tokens[12]));

                // Get the amplifier related data.
                // If unknown or not used, all to undefined.
                // If is used, read each field.
                if(tokens[16] == "na" || !dpbase::helpers::containers::BoolString(tokens[16]))
                {
                    dc.amp_gain = {};
                    dc.amp_bandwidth = {};
                    dc.used_amp = tokens[16] == "na" ? decltype(dc.used_amp)() : false;
                }
                else
                {
                    dc.amp_gain = (tokens[14] == "na" ? decltype(dc.amp_gain)() : std::stod(tokens[14]));
                    dc.amp_bandwidth = (tokens[15] == "na" ? decltype (dc.amp_bandwidth)() : std::stod(tokens[15]));
                    dc.used_amp = true;
                }
            }

            // Add the associated comments, the line number, and the tokens.
            dc.comment_block = record.comment_block;
            dc.line_number = record.line_number;
            dc.tokens = record.tokens;

            // Store the detector configuration struct
            this->detector_cfg = std::move(dc);

        } catch (...)
        {
            result = RecordReadError::CONVERSION_ERROR;
        }

    // Return the result.
    return result;
}

// Specific function for reading C3.
RecordReadError CRDConfiguration::readTimingCFG(
        const ConsolidatedRecord& record, float)
{
    // Variables.
    RecordReadError result = RecordReadError::NOT_ERROR;

    // Get the tokens.
    std::vector<std::string> tokens = record.tokens;

    // Delete the current data.
    this->clearTimingConfiguration();

    // Check if size is correct.
    if (tokens.size() != 8)
        result = RecordReadError::BAD_SIZE;
    // Check if the record type is correct.
    else if (dpbase::helpers::strings::toUpper(tokens[0]) != CfgIdStr[static_cast<int>(ConfigurationLine::TIMING_CFG)])
        result = RecordReadError::BAD_TYPE;
    
    // All ok at this momment.
    else
        try
        {
            // New format header struct.
            TimingConfiguration tc;

            // Get the data.
            tc.detail = std::stoi(tokens[1]);
            tc.cfg_id = tokens[2];
            tc.time_source = tokens[3];
            tc.frequency_source = tokens[4];
            tc.timer = tokens[5];
            tc.timer_serial = tokens[6];
            tc.epoch_delay = std::stod(tokens[7]);

            // Add the associated comments, the line number, and the tokens.
            tc.comment_block = record.comment_block;
            tc.line_number = record.line_number;
            tc.tokens = record.tokens;

            // Store the timing configuration struct
            this->timing_cfg = std::move(tc);

        } catch (...)
        {
            result = RecordReadError::CONVERSION_ERROR;
        }

    // Return the result.
    return result;
}

// Specific function for reading C4.
RecordReadError CRDConfiguration::readTransponderCFG(
        const ConsolidatedRecord&, float)
{
    // TODO
    return RecordReadError::NOT_IMPLEMENTED;
}

// Specific function for reading C5.
RecordReadError CRDConfiguration::readSoftwareCFG(
        const ConsolidatedRecord& record, float version)
{
    // Variables.
    RecordReadError result = RecordReadError::NOT_ERROR;

    // Get the tokens.
    std::vector<std::string> tokens = record.tokens;

    // Delete the current data.
    this->clearSoftwareConfiguration();

    // Check if size is correct.
    if (tokens.size() != 7)
        result = RecordReadError::BAD_SIZE;
    // Check if the record type is correct.
    else if (dpbase::helpers::strings::toUpper(tokens[0]) !=
             CfgIdStr[static_cast<int>(ConfigurationLine::SOFTWARE_CFG)])
        result = RecordReadError::BAD_TYPE;
    // Check if version is at least 2.
    else if (version < 2.f)
        result = RecordReadError::VERSION_MISMATCH;

    // All ok at this momment.
    else
        try
        {
            // New format header struct.
            SoftwareConfiguration swc;

            // Get the data.
            swc.detail = std::stoi(tokens[1]);
            swc.cfg_id = tokens[2];
            swc.tracking_sw = tokens[3];
            swc.tracking_sw_ver = tokens[4];
            swc.processing_sw = tokens[5];
            swc.processing_sw_ver = tokens[6];

            // Add the associated comments, the line number, and the tokens.
            swc.comment_block = record.comment_block;
            swc.line_number = record.line_number;
            swc.tokens = record.tokens;

            // Store the timing configuration struct
            this->software_cfg = std::move(swc);

        } catch (...)
        {
            result = RecordReadError::CONVERSION_ERROR;
        }

    // Return the result.
    return result;
}

// Specific function for reading C6.
RecordReadError CRDConfiguration::readMeteoCFG(
        const ConsolidatedRecord& record, float version)
{
    // Variables.
    RecordReadError result = RecordReadError::NOT_ERROR;

    // Get the tokens.
    std::vector<std::string> tokens = record.tokens;

    // Delete the current data.
    this->clearMeteorologicalConfiguration();

    // Check if size is correct.
    if (tokens.size() != 12)
        result = RecordReadError::BAD_SIZE;
    // Check if the record type is correct.
    else if (dpbase::helpers::strings::toUpper(tokens[0]) !=
             CfgIdStr[static_cast<int>(ConfigurationLine::METEOROLOGICAL_CFG)])
        result = RecordReadError::BAD_TYPE;
    // Check if version is at least 2.
    else if (version < 2.f)
        result = RecordReadError::VERSION_MISMATCH;

    // All ok at this momment.
    else
        try
        {
            // New format header struct.
            MeteorologicalConfiguration mc;

            // Get the data.
            mc.detail = std::stoi(tokens[1]);
            mc.cfg_id = tokens[2];
            mc.press_manufacturer = tokens[3];
            mc.press_model = tokens[4];
            mc.press_sn = tokens[5];
            mc.temp_manufacturer = tokens[6];
            mc.temp_model = tokens[7];
            mc.temp_sn = tokens[8];
            mc.humid_manufacturer = tokens[9];
            mc.humid_model = tokens[10];
            mc.humid_sn = tokens[11];

            // Add the associated comments, the line number, and the tokens.
            mc.comment_block = record.comment_block;
            mc.line_number = record.line_number;
            mc.tokens = record.tokens;

            // Store the timing configuration struct
            this->meteorological_cfg = std::move(mc);

        } catch (...)
        {
            result = RecordReadError::CONVERSION_ERROR;
        }

    // Return the result.
    return result;
}

// Specific function for reading C7.
RecordReadError CRDConfiguration::readCalibrationCFG(const ConsolidatedRecord &record, float version)
{
    // Variables.
    RecordReadError result = RecordReadError::NOT_ERROR;

    // Get the tokens.
    std::vector<std::string> tokens = record.tokens;

    // Delete the current data.
    this->clearCalibrationTargetConfiguration();

    // Check if size is correct.
    if (tokens.size() != 10)
        result = RecordReadError::BAD_SIZE;
    // Check if the record type is correct.
    else if (dpbase::helpers::strings::toUpper(tokens[0]) !=
             CfgIdStr[static_cast<int>(ConfigurationLine::CALIBRATION_CFG)])
        result = RecordReadError::BAD_TYPE;
    // Check if version is at least 2.
    else if (version < 2.f)
        result = RecordReadError::VERSION_MISMATCH;

    // All ok at this momment.
    else
        try
        {
            // New format header struct.
            CalibrationTargetConfiguration ctc;

            // Get the data.
            ctc.detail = std::stoi(tokens[1]);
            ctc.cfg_id = tokens[2];
            ctc.target_name = tokens[3];
            ctc.distance = std::stod(tokens[4]);
            if (tokens[5] != "na")
                ctc.error = std::stod(tokens[5]);
            if (tokens[6] != "na")
                ctc.delays = std::stod(tokens[6]);
            if (tokens[7] != "na")
                ctc.energy = std::stod(tokens[7]);
            ctc.processing_sw = tokens[8];
            ctc.processing_sw_ver = tokens[9];

            // Add the associated comments, the line number, and the tokens.
            ctc.comment_block = record.comment_block;
            ctc.line_number = record.line_number;
            ctc.tokens = record.tokens;

            // Store the timing configuration struct
            this->calibration_cfg = std::move(ctc);

        } catch (...)
        {
            result = RecordReadError::CONVERSION_ERROR;
        }

    // Return the result.
    return result;
}

// Generic function for reading single configuration line.
RecordReadError CRDConfiguration::readConfigurationLine(const RecordLinePair& lpair, float version)
{
    RecordReadError result = RecordReadError::NOT_IMPLEMENTED;

    switch (static_cast<ConfigurationLine>(lpair.first))
    {
    case ConfigurationLine::SYSTEM_CFG:
        result = this->readSystemCFG(lpair.second, version);
        break;

    case ConfigurationLine::LASER_CFG:
        result = this->readLaserCFG(lpair.second, version);
        break;

    case ConfigurationLine::DETECTOR_CFG:
        result = this->readDetectorCFG(lpair.second, version);
        break;

    case ConfigurationLine::TIMING_CFG:
        result = this->readTimingCFG(lpair.second, version);
        break;

    case ConfigurationLine::TRANSPONDER_CFG:
        result = this->readTransponderCFG(lpair.second, version);
        break;

    case ConfigurationLine::SOFTWARE_CFG:
        result = this->readSoftwareCFG(lpair.second, version);
        break;

    case ConfigurationLine::METEOROLOGICAL_CFG:
        result = this->readMeteoCFG(lpair.second, version);
        break;

    case ConfigurationLine::CALIBRATION_CFG:
        result = this->readCalibrationCFG(lpair.second, version);
        break;
    }

    return result;
}

// Helper function to generate A, B, C... of C0 line.
std::string CRDConfiguration::generateSystemCfgLine(float version) const
{
    // TODO check validity for mandatory lines
    if (!(this->system_cfg && this->laser_cfg && this->detector_cfg && this->timing_cfg))
        return "";

    // Base line
    std::stringstream line_c0;

    // For version 1 and 2.
    if (version >= 1 && version < 3)
    {
        line_c0 << "C0"
                << ' ' << this->system_cfg->detail_type
                << ' ' << dpbase::helpers::strings::numberToFixStr(this->system_cfg->transmit_wavelength, 10)
                << ' ' << this->system_cfg->system_cfg_id
                << ' ' << this->laser_cfg->cfg_id
                << ' ' << this->detector_cfg->cfg_id
                << ' ' << this->timing_cfg->cfg_id;

        if (transponder_cfg)
            line_c0 << ' ' << this->transponder_cfg->cfg_id;
    }

    // For version 2 only
    if (version >= 2 && version < 3)
    {
        // Non mandatory fields.
        if(this->software_cfg)
            line_c0 << ' ' << this->software_cfg->cfg_id;
        if(this->meteorological_cfg)
            line_c0 << ' ' << this->meteorological_cfg->cfg_id;
        if(this->calibration_cfg)
            line_c0 << ' ' << this->calibration_cfg->cfg_id;
    }

    // Return computed C0
    return line_c0.str();
}

std::string CRDConfiguration::LaserConfiguration::generateLine(float version) const
{
    // Base line
    std::stringstream line_c1;

    // For version 1 and 2.
    if (version >= 1 && version < 3)
    {
        line_c1 << "C1"
                << ' ' << this->detail
                << ' ' << this->cfg_id
                << ' ' << this->type
                << ' ' << dpbase::helpers::strings::numberToFixStr(this->primary_wavelength, 10)
                << ' ' << dpbase::helpers::strings::numberToFixStr(this->fire_rate, 10)
                << ' ' << dpbase::helpers::strings::numberToFixStr(this->pulse_energy, 10)
                << ' ' << dpbase::helpers::strings::numberToFixStr(this->pulse_width, 6)
                << ' ' << dpbase::helpers::strings::numberToFixStr(this->beam_divergence, 5)
                << ' ' << this->pulses_outgoing_semitrain;
    }

    // Return the C1
    return line_c1.str();
}

std::string CRDConfiguration::DetectorConfiguration::generateLine(float version) const
{
    // Base line
    std::stringstream line_c2;

    // For version 1 and 2.
    if (version >= 1 && version < 3)
    {
        line_c2 << "C2"
                << ' ' << this->detail
                << ' ' << this->cfg_id
                << ' ' << this->type
                << ' ' << dpbase::helpers::strings::numberToFixStr(this->work_wavelength,10)
                << ' ' << dpbase::helpers::strings::numberToFixStr(this->efficiency, 6)
                << ' ' << dpbase::helpers::strings::numberToFixStr(this->voltage, 5)
                << ' ' << dpbase::helpers::strings::numberToFixStr(this->dark_count, 5)
                << ' ' << this->out_pulse_type
                << ' ' << dpbase::helpers::strings::numberToFixStr(this->out_pulse_width, 5)
                << ' ' << dpbase::helpers::strings::numberToFixStr(this->spectral_filter, 5)
                << ' ' << dpbase::helpers::strings::numberToFixStr(this->transm_spectral_filter, 5);

        // Spatial filter is optional
        if (this->spatial_filter)
            line_c2 << ' ' << dpbase::helpers::strings::numberToFixStr(this->spatial_filter.value(), 5);
        else
            line_c2 << ' ' << (version < 2 ? "-1" : "na");

        line_c2 << ' ' << this->ext_signal_proc;
    }

    // For version 2.
    if (version >= 2 && version < 3)
    {
        // If used amp is No (0) or not defined, gain and bandwidth are
        // set to na and amp in use to 0 (No) or na (unknown).
        if(!this->used_amp.value_or(false))
        {
            line_c2 << " na na " << (this->used_amp ? "0" : "na");
        }
        // Otherwise, we will print each vaule as separated.
        else
        {
            line_c2 << ' ' << (!this->amp_gain ? "na" : dpbase::helpers::strings::numberToFixStr(this->amp_gain.value(), 6))
                    << ' ' << (!this->amp_bandwidth ? "na" : dpbase::helpers::strings::numberToFixStr(this->amp_bandwidth.value(), 6))
                    << " 1";
        }
    }

    // Return the C2
    return line_c2.str();
}

std::string CRDConfiguration::TimingConfiguration::generateLine(float version) const
{
    // Base line
    std::stringstream line_c3;

    // For version 1 and 2.
    if (version >= 1 && version < 3)
    {
        line_c3 << "C3"
                << ' ' << this->detail
                << ' ' << this->cfg_id
                << ' ' << this->time_source
                << ' ' << this->frequency_source
                << ' ' << this->timer
                << ' ' << this->timer_serial
                << ' ' << dpbase::helpers::strings::numberToFixStr(this->epoch_delay, 6);
    }

    // Return the C3
    return line_c3.str();
}

std::string CRDConfiguration::TransponderConfiguration::generateLine(float) const
{
    // TODO
    // Base line
    std::stringstream line_c4;

    // Return the C4
    return line_c4.str();
}

std::string CRDConfiguration::SoftwareConfiguration::generateLine(float version) const
{
    // TODO ADD THE "na" decision to the documentation.
    // Base line
    std::stringstream line_c5;

    // For version 2.
    if (version >= 2 && version < 3)
    {
        line_c5 << "C5"
                << ' ' << this->detail
                << ' ' << this->cfg_id
                << ' ' << (this->tracking_sw ? this->tracking_sw.value() : "na")
                << ' ' << (this->tracking_sw_ver ? this->tracking_sw_ver.value() : "na")
                << ' ' << (this->processing_sw ? this->processing_sw.value() : "na")
                << ' ' << (this->processing_sw_ver ? this->processing_sw_ver.value() : "na");
    }

    // Return the C5
    return line_c5.str();
}

std::string CRDConfiguration::MeteorologicalConfiguration::generateLine(float version) const
{
    // TODO ADD THE "na" decision to the documentation.
    // Base line
    std::stringstream line_c6;

    // Only version 2.
    if (version >= 2 && version < 3)
    {
        line_c6 << "C6"
                << ' ' << this->detail
                << ' ' << this->cfg_id
                << ' ' << (this->press_manufacturer ? this->press_manufacturer.value() : "na")
                << ' ' << (this->press_model ? this->press_model.value() : "na")
                << ' ' << (this->press_sn ? this->press_sn.value() : "na")
                << ' ' << (this->temp_manufacturer ? this->temp_manufacturer.value() : "na")
                << ' ' << (this->temp_model ? this->temp_model.value() : "na")
                << ' ' << (this->temp_sn ? this->temp_sn.value() : "na")
                << ' ' << (this->humid_manufacturer ? this->humid_manufacturer.value() : "na")
                << ' ' << (this->humid_model ? this->humid_model.value() : "na")
                << ' ' << (this->humid_sn ? this->humid_sn.value() : "na");
    }

    // Return the C6
    return line_c6.str();
}

std::string CRDConfiguration::CalibrationTargetConfiguration::generateLine(float version) const
{
    // TODO ADD THE "na" decision to the documentation.
    // Base line
    std::stringstream line_c7;

    // For version 2.
    if (version >= 2 && version < 3)
    {
        line_c7 << "C7"
                << ' ' << this->detail
                << ' ' << this->cfg_id
                << ' ' << this->target_name
                << ' ' << dpbase::helpers::strings::numberToStr(this->distance, 17, 5)
                << ' ' << (this->error  ? dpbase::helpers::strings::numberToStr(this->error.value(), 8, 2) : "na")
                << ' ' << (this->delays ? dpbase::helpers::strings::numberToStr(this->delays.value(), 12, 8) : "na")
                << ' ' << (this->energy ? dpbase::helpers::strings::numberToStr(this->energy.value(), 12, 2) : "na")
                << ' ' << (this->processing_sw ? this->processing_sw.value() : "na")
                << ' ' << (this->processing_sw_ver ? this->processing_sw_ver.value() : "na");
    }

    // Return the C7
    return line_c7.str();
}

// =====================================================================================================================

}}} // END NAMESPACES
// =====================================================================================================================
