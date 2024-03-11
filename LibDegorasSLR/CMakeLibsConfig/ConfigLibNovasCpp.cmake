# **********************************************************************************************************************
# Updated 13/02/2024
# **********************************************************************************************************************

# **********************************************************************************************************************

# Macro to search for LibNovasCpp
MACRO(macro_configure_libnovascpp_default version version_mode extra_search_paths extra_search_patterns)

    # Log.
    message(STATUS "Configuring LibNovasCpp...")

    # Setup the find package config.
    set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)

    # Configure extra things if neccesary.
    # ...

    # Find the package.
    macro_find_package_default("LibNovasCpp" "${version}" "${version_mode}" "${extra_search_paths}" "${extra_search_patterns}")

    # Set direct access to the library useful properties.
    get_target_property(LibNovasCpp_INCLUDES LibNovasCpp::LibNovasCpp INTERFACE_INCLUDE_DIRECTORIES)
    get_target_property(LibNovasCpp_LINK_LIBS LibNovasCpp::LibNovasCpp INTERFACE_LINK_LIBRARIES)
    get_target_property(LibNovasCpp_LOCATION LibNovasCpp::LibNovasCpp LOCATION)
    get_target_property(LibNovasCpp_LOCATION_DEBUG LibNovasCpp::LibNovasCpp IMPORTED_LOCATION_DEBUG)
    get_target_property(LibNovasCpp_LOCATION_RELEASE LibNovasCpp::LibNovasCpp IMPORTED_LOCATION_RELEASE)
    get_filename_component(LibNovasCpp_LOCATION_DIR "${LibNovasCpp_LOCATION}" DIRECTORY)

    # Logs.
    message(STATUS "  LibNovasCpp::LibNovasCpp information:")
    message(STATUS "    Library version: ${LibNovasCpp_VERSION}")
    message(STATUS "    Configuration path: ${LibNovasCpp_DIR}")
    message(STATUS "    Include directories: ${LibNovasCpp_INCLUDES}")
    message(STATUS "    Interface link libraries: ${LibNovasCpp_LINK_LIBS}")
    message(STATUS "    Lib location dir: ${LibNovasCpp_LOCATION_DIR}")
    message(STATUS "    Lib location: ${LibNovasCpp_LOCATION}")
    message(STATUS "    Lib location debug: ${LibNovasCpp_LOCATION_DEBUG}")
    message(STATUS "    Lib location release: ${LibNovasCpp_LOCATION_RELEASE}")

ENDMACRO()

# **********************************************************************************************************************

# Macro to link target to LibNovasCpp
MACRO(macro_link_libnovascpp_default target)

    message(STATUS "Linking LibNovasCPP to target ${target}")

    target_link_libraries(${target} PUBLIC LibNovasCpp::LibNovasCpp)

    if(MODULES_GLOBAL_SHOW_EXTERNALS)
        get_target_property(libnovascpp_includes LibNovasCpp::LibNovasCpp INTERFACE_INCLUDE_DIRECTORIES)
        file(GLOB_RECURSE EXTERNAL_HEADERS ${libnovascpp_includes}/*)
        target_sources(${target} PUBLIC ${EXTERNAL_HEADERS})
    endif()

ENDMACRO()

# **********************************************************************************************************************
