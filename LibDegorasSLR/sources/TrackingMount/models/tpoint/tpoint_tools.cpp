/***********************************************************************************************************************
 *   AMELAS_SFELMountController: [...].
 *
 *   Copyright (C) 2023 ROA Team (Royal Institute and Observatory of the Spanish Navy)
 *                      < Ángel Vera Herrera, avera@roa.es - angeldelaveracruz@gmail.com >
 *                      < Jesús Relinque Madroñal >
 *                      AVS AMELAS Team
 *                      <>
 *
 *   This file is part of AMELAS_SFELMountController.
 *
 *   Licensed under [...]
 **********************************************************************************************************************/

/** ********************************************************************************************************************
 * @file tpoint_tools.cpp
 * @brief This file contains the implementation of elements related to TPoint usage.
 * @author Degoras Project Team
 * @author AVS AMELAS Team
 * @copyright EUPL License
 * @version 2309.5
***********************************************************************************************************************/

// C++ INCLUDES
// =====================================================================================================================
#include <cmath>
#include <regex>
#include <fstream>
// =====================================================================================================================

// PROJECT INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/TrackingMount/models/tpoint/tpoint_tools.h"
// =====================================================================================================================

// LIBDPBASE INCLUDES
// =====================================================================================================================
#include "LibDegorasBase/Mathematics/utils/math_utils.h"
#include "LibDegorasBase/Mathematics/units/unit_conversions.h"
// =====================================================================================================================

using dpslr::astro::types::AltAzPos;
using dpslr::astro::types::AltAzCorrection;

// LIBDEGORASSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace mount{
namespace models{

const std::regex regex_term_harmonic(R"(H([AESV])([CS])([AE])(\d?\d?)(([CS])([AE])(\d?\d?))?)");

TPointCoefficientId fromString(const std::string &term_name)
{
    TPointCoefficientId id = TPointCoefficientId::INVALID_ID;
    if (!term_name.empty())
    {
        if (std::regex_match(term_name, regex_term_harmonic))
            id = TPointCoefficientId::H;
        else if (term_name == "IE")
            id = TPointCoefficientId::IE;
        else if (term_name == "IA")
            id = TPointCoefficientId::IA;
        else if (term_name == "CA")
            id = TPointCoefficientId::CA;
        else if (term_name == "AN")
            id = TPointCoefficientId::AN;
        else if (term_name == "AW")
            id = TPointCoefficientId::AW;
        else if (term_name == "NPAE")
            id = TPointCoefficientId::NPAE;
        else if (term_name == "TF")
            id = TPointCoefficientId::TF;
        else if (term_name.size() >= 2 && term_name[0] == 'T' && term_name[1] == 'X')
            id = TPointCoefficientId::TX;
    }

    return id;
}


inline double arcsec_to_deg(const double &arcsec)
{
    return arcsec / 3600.0;
}


AltAzCorrection computeSingleCoefficient(const TPointSingleCoefficient& coef, const AltAzPos& pos)
{
    double coefValue = arcsec_to_deg(coef.value);
    double azimuth = dpbase::math::units::degToRad(pos.az);
    double elevation = dpbase::math::units::degToRad(pos.el);

    const double PI_HALF = M_PI / 2;
    const double THREE_PI_HALF = 3 * M_PI / 2;

    AltAzCorrection offsets = {0.0, 0.0};

    switch (coef.id)
    {
    case TPointCoefficientId::IE:
        offsets.el = coefValue;
        break;

    case TPointCoefficientId::IA:
        offsets.az = -coefValue;
        break;

    case TPointCoefficientId::CA:

        if (dpbase::math::compareFloating(elevation, PI_HALF) &&
            dpbase::math::compareFloating(elevation, THREE_PI_HALF))
        {
            offsets.az = -coefValue / cos(elevation);
        }
        break;

    case TPointCoefficientId::AN:
        if (dpbase::math::compareFloating(elevation, PI_HALF) &&
            dpbase::math::compareFloating(elevation, THREE_PI_HALF))
        {
            offsets.az = -coefValue * sin(azimuth) * tan(elevation);
        }
        offsets.el = -coefValue * cos(azimuth);
        break;

    case TPointCoefficientId::AW:
        if (dpbase::math::compareFloating(elevation, PI_HALF) &&
            dpbase::math::compareFloating(elevation, THREE_PI_HALF))
        {
            offsets.az = -coefValue * cos(azimuth) * tan(elevation);
        }
        offsets.el = coefValue * sin(azimuth);
        break;

    case TPointCoefficientId::NPAE:
        if (dpbase::math::compareFloating(elevation, PI_HALF) &&
            dpbase::math::compareFloating(elevation, THREE_PI_HALF))
        {
            offsets.az = -coefValue * tan(elevation);
        }
        break;

    case TPointCoefficientId::TF:
        offsets.el = -coefValue * sin(PI_HALF - elevation); // The correction is from zenith, so change sign.
        break;

    case TPointCoefficientId::TX:
        if (coef.name == "TX")
        {
            offsets.el = -coefValue * tan(PI_HALF - elevation); // The correction is from zenith, so change sign.
        }
        else if (coef.name == "TXL")
        {
            offsets.el = -coefValue / tan(elevation);
        }
        else if (coef.name == "TX5")
        {
            if (elevation > dpbase::math::units::degToRad(5.))
                offsets.el = -coefValue * tan(PI_HALF - elevation); // The correction is from zenith, so change sign.
        }
        else if (coef.name == "TX10")
        {
            if (elevation > dpbase::math::units::degToRad(10.))
                offsets.el = -coefValue * tan(PI_HALF - elevation); // The correction is from zenith, so change sign.
        }
        else if (coef.name == "TX15")
        {
            if (elevation > dpbase::math::units::degToRad(15.))
                offsets.el = -coefValue * tan(PI_HALF - elevation); // The correction is from zenith, so change sign.
        }
        break;


    case TPointCoefficientId::H:
    {
        // Parse harmonic term
        double first_freq = 1., second_freq = 1.;
        double first_variable, second_variable;
        double first_correction, second_correction = 1.;

        std::smatch matches;
        std::regex_match(coef.name, matches, regex_term_harmonic);

        if (matches.size() >= 5)
        {
            // Get frequency of first variable correction
            if (matches[4].length() > 0)
            {
                try
                {
                    first_freq = std::stod(matches[4]);
                } catch (...)
                {
                    first_freq = 1.;
                }
            }
            // Get first variable
            if ('A' == matches[3])
            {
                first_variable = azimuth;
            }
            else
            {
                first_variable = elevation;
            }

            first_variable *= first_freq;

            // Get first correction, applying correct function, variable and frequency.
            if ('C' == matches[2])
            {
                first_correction = std::cos(first_variable);
            }
            else
                first_correction = std::sin(first_variable);

            // Get second correction, if it exists
            if (matches.size() >= 9 && matches[5].length() > 0)
            {
                // Get frequency of second variable correction
                if (matches[8].length() > 0)
                {
                    try
                    {
                        second_freq = std::stod(matches[8]);
                    } catch (...)
                    {
                        second_freq = 1.;
                    }
                }
                // Get second variable
                if ('A' == matches[7])
                {
                    second_variable = azimuth;
                }
                else
                {
                    second_variable = elevation;
                }

                second_variable *= second_freq;

                // Get second correction, applying correct function, variable and frequency.
                if ('C' == matches[6])
                {
                    second_correction = std::cos(second_variable);
                }
                else
                    second_correction = std::sin(second_variable);
            }


            // Check the variable to apply correction to
            // Azimuth
            if ('A' == matches[1])
            {
                offsets.az = coefValue * first_correction * second_correction;
            }
            // Elevation
            else if ('E' == matches[1])
            {
                offsets.el = coefValue * first_correction * second_correction;
            }
            // Left to right (approx. equal to Az * cos Elev)
            else if ('S' == matches[1])
            {
                offsets.az = coefValue * first_correction * second_correction / std::cos(elevation);
            }
            // Az/El nonperpendicularity
            else if ('V' == matches[1])
            {
                // TODO:
            }
        }

        break;
    }

    default:
        break;
    }

    return offsets;
}

AltAzPos computeParallelCoefficients(const TPointParallelCoefficients& coefs, const AltAzPos& pos)
{
    AltAzPos accumulatedOffsets = {0.0L, 0.0L};
    for (const auto& coef : coefs)
    {
        auto computedOffset = computeSingleCoefficient(coef, pos);
        accumulatedOffsets.az += computedOffset.az;
        accumulatedOffsets.el += computedOffset.el;
    }
    return accumulatedOffsets;
}

AltAzPos computeCorrectedByTPointPosition(const TPointCoefficients& coefs, const AltAzPos& pos)
{
    AltAzPos modifiedPosition = pos;
    for (const auto& parallelCoefs : coefs)
    {
        auto computedOffsets = computeParallelCoefficients(parallelCoefs, modifiedPosition);
        modifiedPosition.az -= computedOffsets.az;
        modifiedPosition.el -= computedOffsets.el;
    }
    return modifiedPosition;
}

TPointParseError parseTPointModelFile(const std::string &path, TPointCoefficients &coefs)
{
    TPointParseError error = TPointParseError::NOT_ERROR;

    // Open and parse TPoint model file
    std::ifstream model_file(path);
    if (!model_file.is_open())
    {
        error = TPointParseError::CANNOT_OPEN_FILE;
    }
    else
    {

        std::string line;
        // Get caption and parameters lines and discard.
        std::getline(model_file, line);
        std::getline(model_file, line);

        // Read all terms
        while(std::getline(model_file, line) && line != "END")
        {
            char chain, fix;
            char term[9];
            double value, sigma;

            int read = std::sscanf(line.c_str(), "%c%c%8s%10lf%12lf", &chain, &fix, term, &value, &sigma);

            if (read != 4 && read != 5)
            {
                error = TPointParseError::BAD_PARAMETERS_LINE;
                break;
            }
            TPointSingleCoefficient coef;
            coef.name = term;
            coef.name = dpbase::helpers::strings::trim(coef.name);
            coef.id = fromString(coef.name);

            // Fill this coef data.
            if (coef.id == TPointCoefficientId::INVALID_ID)
            {
                error = TPointParseError::UNSUPPORTED_ID;
                break;
            }

            coef.value = value;
            if (read == 5)
                coef.sigma = sigma;
            else
                coef.sigma = 0.;

            // If coef is chained, push in its parallel group, otherwise create a new group
            if (chain == '&')
                coefs.back().push_back(std::move(coef));
            else
                coefs.push_back({std::move(coef)});
        }
    }

    return error;
}

// =====================================================================================================================

}}} // END NAMESPACES.
// =====================================================================================================================
