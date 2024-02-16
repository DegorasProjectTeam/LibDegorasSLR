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
 * @file file_helpers.cpp
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

// LIBDPSLR INCLUDES
// =====================================================================================================================
#include <LibDegorasSLR/Helpers/file_helpers.h>
// =====================================================================================================================

// DPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace helpers{
namespace files{
// =====================================================================================================================

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
