# **********************************************************************************************************************
# Updated 19/01/2024
# **********************************************************************************************************************

# **********************************************************************************************************************

MACRO(macro_setup_base_project project_name project_version project_build_mode)

    # Check version.
    if (NOT ${CMAKE_VERSION} VERSION_LESS "3.1.0")
        cmake_policy(SET CMP0054 NEW)
    endif()

    # Project name.
    project(${project_name} VERSION ${project_version})

    # Log.
    message(STATUS "Preparing project: ${project_name} - ${project_version}")

    #Configure the build type.
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE ${project_build_mode})
    endif()

    # Output directories.
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

    # Check system architectrue.
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(SYS_ARCH "64 bits")
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(SYS_ARCH "32 bits")
    endif()

    # Logs.
    if(WIN32)
        set(OS_NAME "Windows")
    elseif(APPLE)
        set(OS_NAME "Apple")
    elseif(UNIX AND NOT APPLE)
        set(OS_NAME "Linux/Unix")
    else()
        set(OS_NAME "Unknown")
    endif()
    message(STATUS "  Project Name: ${CMAKE_PROJECT_NAME}")
    message(STATUS "  Project Version: ${CMAKE_PROJECT_VERSION}")
    message(STATUS "  C++ Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
    message(STATUS "  Operating System: ${OS_NAME}")
    message(STATUS "  Processor Architecture: ${CMAKE_SYSTEM_PROCESSOR}")
    message(STATUS "  System Architecture: ${SYS_ARCH}")
    message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
    message(STATUS "  Build Archive: ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
    message(STATUS "  Build Library: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
    message(STATUS "  Build Runtime: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    message(STATUS "  Force installation path: ${MODULES_GLOBAL_FORCE_INSTALL_DIR}")
    message(STATUS "  Install external dependencies: ${MODULES_GLOBAL_INSTALL_EXT_DEPS}")
    message(STATUS "  Install runtime artifacts: ${MODULES_GLOBAL_INSTALL_RUNTIME_ARTIFACTS}")

ENDMACRO()

# **********************************************************************************************************************
