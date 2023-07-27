# **********************************************************************************************************************
#    LibDPSLR (Degoras Project SLR Library): A libre base library for SLR related developments.                                                              *
#
#    Copyright (C) 2023 Degoras Project Team
#                       < Ángel Vera Herrera, avera@roa.es - angeldelaveracruz@gmail.com >
#                       < Jesús Relinque Madroñal >
#
#    This file is part of LibDPSLR.
#
#    Licensed under the European Union Public License (EUPL), Version 1.2 or subsequent versions of the EUPL license
#    as soon they will be approved by the European Commission (IDABC).
#
#    This project is free software: you can redistribute it and/or modify it under the terms of the EUPL license as
#    published by the IDABC, either Version 1.2 or, at your option, any later version.
#
#    This project is distributed in the hope that it will be useful. Unless required by applicable law or agreed to in
#    writing, it is distributed on an "AS IS" basis, WITHOUT ANY WARRANTY OR CONDITIONS OF ANY KIND; without even the
#    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the EUPL license to check specific
#    language governing permissions and limitations and more details.
#
#    You should use this project in compliance with the EUPL license. You should have received a copy of the license
#    along with this project. If not, see the license at < https://eupl.eu/ >.
# **********************************************************************************************************************

# **********************************************************************************************************************
# Created 05/05/2023
# Updated 26/06/2023
# **********************************************************************************************************************

# Cmake minimum.
cmake_minimum_required(VERSION 2.9)

if (NOT ${CMAKE_VERSION} VERSION_LESS "3.1.0")
    CMAKE_POLICY(SET CMP0054 NEW)
endif()

#Configure the build type.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Configure the version.
set(MAJOR_VERSION 2)
set(MINOR_VERSION 0)
set(PATCH_VERSION 0)
set(VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION})


# Project name.
project(LibDPSLR)

# Other configs.
set(SHORT_NAME DPSLR)
set(CMAKE_COLOR_MAKEFILE ON)

# Set diferent paths for build and deployment.
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++17 -pthread -fopenmp")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Set path to additional CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules ${CMAKE_MODULE_PATH})
include(CMakeMacros)

# Get the headers, sources and template files.
FILE(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/includes/LibDPSLR/*.h)
FILE(GLOB_RECURSE TEMPLTS ${CMAKE_SOURCE_DIR}/includes/LibDPSLR/*.tpp)
FILE(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/sources/*.cpp)

# Get the include aliases files.
macro_get_files_without_extension(ALIAS ${CMAKE_SOURCE_DIR}/includes/LibDPSLR/*)

# Create a custom group for the templates and aliases.
source_group("Header Files" FILES ${TEMPLTS})
source_group("Alias Files" FILES ${ALIAS})

# Include the header directories.
include_directories(${CMAKE_SOURCE_DIR}/includes)

# Add definitions and the library.
add_definitions(-DLIBDPSLR_LIBRARY)
add_library(LibDPSLR SHARED ${SOURCES} ${HEADERS} ${TEMPLTS} ${ALIAS})

# Enable testing.
enable_testing()

# Add the diferents subprojects directories.
add_subdirectory(testing)
# add_subdirectory(examples)

# **********************************************************************************************************************
