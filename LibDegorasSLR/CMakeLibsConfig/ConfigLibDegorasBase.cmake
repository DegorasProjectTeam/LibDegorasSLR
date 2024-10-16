# **********************************************************************************************************************
# Updated 11/03/2024
# **********************************************************************************************************************

# **********************************************************************************************************************

# Macro to search for LibDegorasBase
MACRO(macro_configure_libdegorasbase_default version version_mode extra_search_paths extra_search_patterns)

    # Log.
    message(STATUS "Configuring LibDegorasBase...")

    # Setup the find package config.
    set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)

    # Configure extra things if neccesary.
    # ...

    # Find the package.
    macro_find_package_default("LibDegorasBase" "${version}" "${version_mode}" "${extra_search_paths}" "${extra_search_patterns}")

    # Logs.
    get_target_property(LibDegorasBase_INCLUDES LibDegorasBase::LibDegorasBase INTERFACE_INCLUDE_DIRECTORIES)
    get_target_property(LibDegorasBase_LINK_LIBS LibDegorasBase::LibDegorasBase INTERFACE_LINK_LIBRARIES)
    get_target_property(LibDegorasBase_LOCATION LibDegorasBase::LibDegorasBase LOCATION)
    get_target_property(LibDegorasBase_LOCATION_DEBUG LibDegorasBase::LibDegorasBase IMPORTED_LOCATION_DEBUG)
    get_target_property(LibDegorasBase_LOCATION_RELEASE LibDegorasBase::LibDegorasBase IMPORTED_LOCATION_RELEASE)
    get_filename_component(LibDegorasBase_LOCATION_DIR "${LibDegorasBase_LOCATION}" DIRECTORY)
    message(STATUS "  LibDegorasBase::LibDegorasBase information:")
    message(STATUS "    Library version: ${LibDegorasBase_VERSION}")
    message(STATUS "    Configuration path: ${LibDegorasBase_DIR}")
    message(STATUS "    Include directories: ${LibDegorasBase_INCLUDES}")
    message(STATUS "    Interface link libraries: ${LibDegorasBase_LINK_LIBS}")
    message(STATUS "    DLL location: ${LibDegorasBase_LOCATION}")
    message(STATUS "    DLL location dir: ${LibDegorasBase_LOCATION_DIR}")
    message(STATUS "    DLL location debug: ${LibDegorasBase_LOCATION_DEBUG}")
    message(STATUS "    DLL location release: ${LibDegorasBase_LOCATION_RELEASE}")

ENDMACRO()

# **********************************************************************************************************************

# Macro to link target to LibDegorasBase
MACRO(macro_link_libdegorasbase_default target visibility)

    message(STATUS "Linking LibDegorasBase to target ${target} with ${visibility} visibility")

    target_link_libraries(${target} ${visibility} LibDegorasBase::LibDegorasBase)

    if(MODULES_GLOBAL_SHOW_EXTERNALS)

        # LibDegorasBase
        get_target_property(LibDegorasBase_INCLUDES LibDegorasBase::LibDegorasBase INTERFACE_INCLUDE_DIRECTORIES)
        file(GLOB_RECURSE EXTERNAL_HEADERS ${LibDegorasBase_INCLUDES}/*)
        target_sources(${target} ${visibility} ${EXTERNAL_HEADERS})

        # LibNovasCpp
        get_target_property(LibNovasCpp_INCLUDES LibNovasCpp::LibNovasCpp INTERFACE_INCLUDE_DIRECTORIES)
        file(GLOB_RECURSE EXTERNAL_HEADERS ${LibNovasCpp_INCLUDES}/*)
        target_sources(${target} ${visibility} ${EXTERNAL_HEADERS})

    endif()

ENDMACRO()

# **********************************************************************************************************************
