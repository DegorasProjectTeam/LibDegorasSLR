#
# Windows toolchain for MSYS2 MinGW prefixes.
# Requires environment variable:
#   MINGW_ROOT=/ucrt64 (or /mingw64, /clang64, etc.)

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

if(NOT DEFINED ENV{MINGW_ROOT} OR "$ENV{MINGW_ROOT}" STREQUAL "")
  message(FATAL_ERROR "[CMAKE] dp_windows_mingw.cmake: MINGW_ROOT is not set. Expected /ucrt64, /mingw64, /clang64, etc.")
endif()

set(_PFX "$ENV{MINGW_ROOT}")

# Compilers
set(CMAKE_C_COMPILER   "${_PFX}/bin/gcc.exe"  CACHE FILEPATH "MSYS2 GCC (C)" FORCE)
set(CMAKE_CXX_COMPILER "${_PFX}/bin/g++.exe"  CACHE FILEPATH "MSYS2 G++ (C++)" FORCE)

# Resource compiler (optional, but common for Windows builds)
if(EXISTS "${_PFX}/bin/windres.exe")
  set(CMAKE_RC_COMPILER "${_PFX}/bin/windres.exe" CACHE FILEPATH "MSYS2 windres" FORCE)
endif()

# Prefer Ninja from the same prefix if available
if(EXISTS "${_PFX}/bin/ninja.exe")
  set(CMAKE_MAKE_PROGRAM "${_PFX}/bin/ninja.exe" CACHE FILEPATH "Ninja from MSYS2 prefix" FORCE)
endif()

# Prefer config packages, and ensure prefix is searched first
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG ON CACHE BOOL "" FORCE)
list(PREPEND CMAKE_PREFIX_PATH "${_PFX}")

# Useful default
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "" FORCE)