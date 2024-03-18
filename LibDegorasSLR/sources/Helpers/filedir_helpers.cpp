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
 * @file filedir_helpers.cpp
 * @brief This file contains the implementation of several helper tools related with files and directories.
 * @author Degoras Project Team
 * @copyright EUPL License
 * @version 2305.1
***********************************************************************************************************************/

// =====================================================================================================================
#if  defined(WINDOWS) ||  defined(_MSC_VER)
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif
// =====================================================================================================================

// C++ INCLUDES
// =====================================================================================================================
#include <string>
#include <fstream>
// =====================================================================================================================

// LIBRARY INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/Helpers/filedir_helpers.h"
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace helpers{
namespace files{
// =====================================================================================================================

std::string windowsPathToUnix(const std::string &windows_path, bool rm_drive_letter)
{
    std::string unix_path = windows_path;
    std::replace(unix_path.begin(), unix_path.end(), '\\', '/');
    if(rm_drive_letter)
    {
        size_t colon_pos = unix_path.find(':');
        if (colon_pos != std::string::npos)
            unix_path = unix_path.substr(colon_pos + 1);
    }
    return unix_path;
}

std::string unixPathToWindows(const std::string &unix_path, const std::string &drive_letter)
{
    std::string windows_path = unix_path;
    std::replace(windows_path.begin(), windows_path.end(), '/', '\\');
    if (!drive_letter.empty() && windows_path[0] == '\\')
        windows_path = drive_letter + ":" + windows_path;
    return windows_path;
}

std::string normalizePath(const std::string &path)
{
    // Ensure the path finish withoud '/' or '\\'.
    std::string formated = path;
    if (!formated.empty())
    {
        while (formated.back() == '/' || formated.back() == '\\')
            formated.pop_back();
    }
    return formated;
}




bool createDirectory(const std::string &path)
{
    std::string command;
#ifdef _WIN32
    command = "mkdir \"" + path + "\"";
#else
    command = "mkdir -p \"" + path + "\"";
#endif
    if (system(command.c_str()) == 0)
        return true;
    else
        return false;
}









bool directoryExists(const std::string &path)
{
    struct stat info;
    if (stat(path.c_str(), &info) != 0)
        return false;
    return (info.st_mode & S_IFDIR) != 0;
}

bool fileExists(const std::string &path)
{
    struct stat info;
    return stat(path.c_str(), &info) == 0;
}

std::string getFileName(const std::string &filepath)
{
    // Find the last occurrence of directory separator character.
    size_t l_sep = filepath.find_last_of("/\\");

    if (l_sep != std::string::npos) {
        // Extract and return the substring after the separator.
        return filepath.substr(l_sep + 1);
    }

    // If no separator is found, return the entire input path as the filename.
    return filepath;
}

std::string getCurrentDir()
{
    char buff[FILENAME_MAX];
    GetCurrentDir( buff, FILENAME_MAX );
    std::string current_working_dir(buff);
    for (size_t i = 0; i < current_working_dir.size(); ++i)
        if (current_working_dir[i] == '\\')
            current_working_dir[i] = '/';
    return current_working_dir;
}




DegorasInputFileStream::DegorasInputFileStream(const std::string& path):
    std::ifstream(path),
    current_line_number_(0)
{
    // Open the file.
    if (fileExists(path))
    {
        this->file_path_ = path;
        this->file_name_ = files::getFileName(path);
    }
}

std::istream& DegorasInputFileStream::getline(std::string& line)
{
    this->current_line_number_++;
    return std::getline(*this, line);
}

unsigned DegorasInputFileStream::getCurrentLineNumber() const {return this->current_line_number_;}

const std::string& DegorasInputFileStream::getFilePath() const {return this->file_path_;}

bool DegorasInputFileStream::isEmpty()
{
    // Return the result.
    return (this->peek() == std::ifstream::traits_type::eof());
}

DegorasInputFileStream::~DegorasInputFileStream()
{
    if(this->is_open())
        this->close();
}







}}} // END NAMESPACES
// =====================================================================================================================
