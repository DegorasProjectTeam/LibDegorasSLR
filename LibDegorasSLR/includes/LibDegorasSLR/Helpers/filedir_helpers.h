/***********************************************************************************************************************
 *   LibDegorasSLR (Degoras Project SLR Library).                                                                      *
 *                                                                                                                     *
 *   A modern and efficient C++ base library for Satellite Laser Ranging (SLR) software and real-time hardware         *
 *   related developments. Developed as a free software under the context of Degoras Project for the Spanish Navy      *
 *   Observatory SLR station (SFEL) in San Fernando and, of course, for any other station that wants to use it!        *
 *                                                                                                                     *
 *   Copyright (C) 2024 Degoras Project Team                                                                           *
 *                      < Ángel Vera Herrera, avera@roa.es - angeldelaveracruz@gmail.com >                             *
 *                      < Jesús Relinque Madroñal >                                                                    *
 *                                                                                                                     *
 *   This file is part of LibDegorasSLR.                                                                               *
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
 * @file filedir_helpers.h
 * @brief This file contains several helper tools related with files and directories.
 * @author Degoras Project Team
 * @copyright EUPL License
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// C++ INCLUDES
// =====================================================================================================================
#include <algorithm>
#include <string>
#include <fstream>
#include <sys/stat.h>
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace helpers{
namespace files{
// =====================================================================================================================

// Directories and files related helper functions.
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Converts a Windows-style path to a Unix-style and can also remove the drive letter from the beginning.
 * @param windows_path The Windows-style path to be converted.
 * @param rm_drive_letter If true, removes the drive letter from the path (if exists). Defaults to false.
 * @return std::string The converted Unix-style path.
 */
LIBDPSLR_EXPORT std::string windowsPathToUnix(const std::string& windows_path, bool rm_drive_letter = false);

/**
 * @brief Converts a Unix-style path to a Windows-style. It allows the addition of a drive letter to be prepended.
 * @param unix_path The Unix-style path to be converted.
 * @param drive_letter The drive letter to be prepended (if empty, no letter is added).
 * @return std::string The converted Windows-style path.
 */
LIBDPSLR_EXPORT std::string unixPathToWindows(const std::string& unix_path, const std::string& drive_letter = "");

/**
 * @brief Normalizes a filesystem path by removing trailing slashes or backslashes from the path. The normalization
 * makes the path suitable for further manipulation or concatenation with filenames or subdirectory names.
 * @param path The filesystem path to be normalized.
 * @return std::string A copy of the input path, modified to remove any trailing directory separators.
 */

LIBDPSLR_EXPORT std::string normalizePath(const std::string& path);



LIBDPSLR_EXPORT bool createDirectory(const std::string& path);

LIBDPSLR_EXPORT bool directoryExists(const std::string& path);

LIBDPSLR_EXPORT bool fileExists(const std::string &path);

LIBDPSLR_EXPORT std::string getCurrentDir();

LIBDPSLR_EXPORT std::string getFileName(const std::string& filepath);

// Helper class for counting file line numbres of a file.
class LIBDPSLR_EXPORT DegorasInputFileStream : public std::ifstream
{
public:

    DegorasInputFileStream(const std::string& path);

    std::istream& getline(std::string& line);

    // Observer methods
    unsigned getCurrentLineNumber() const;

    const std::string& getFilePath() const;
    bool isEmpty();

    virtual ~DegorasInputFileStream() override;

private:

    std::string file_path_;
    std::string file_name_;
    unsigned current_line_number_;
};



}}} // END NAMESPACES
// =====================================================================================================================
