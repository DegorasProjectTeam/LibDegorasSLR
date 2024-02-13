# **********************************************************************************************************************
# Updated 13/02/2024
# **********************************************************************************************************************

# **********************************************************************************************************************

# Macro to search for LibDegorasSLR
MACRO(macro_configure_libdegorasslr_default version version_mode extra_search_paths extra_search_patterns)

    # Log.
    message(STATUS "Configuring LibDegorasSLR...")

    # Configure extra things if neccesary.
    # ...

    # Find the package.
    macro_find_package_default("LibDegorasSLR" "${version}" "${version_mode}" "${extra_search_paths}" "${extra_search_patterns}")

    # Logs.
    get_target_property(LibDegorasSLR_INCLUDES LibDegorasSLR::LibDegorasSLR INTERFACE_INCLUDE_DIRECTORIES)
    get_target_property(LibDegorasSLR_LINK_LIBS LibDegorasSLR::LibDegorasSLR INTERFACE_LINK_LIBRARIES)
    get_target_property(LibDegorasSLR_LOCATION LibDegorasSLR::LibDegorasSLR LOCATION)
    get_target_property(LibDegorasSLR_LOCATION_DEBUG LibDegorasSLR::LibDegorasSLR IMPORTED_LOCATION_DEBUG)
    get_target_property(LibDegorasSLR_LOCATION_RELEASE LibDegorasSLR::LibDegorasSLR IMPORTED_LOCATION_RELEASE)
    message(STATUS "  LibDegorasSLR::LibDegorasSLR information:")
    message(STATUS "    Library version: ${LibDegorasSLR_VERSION}")
    message(STATUS "    Configuration path: ${LibDegorasSLR_DIR}")
    message(STATUS "    Include directories: ${LibDegorasSLR_INCLUDES}")
    message(STATUS "    Interface link libraries: ${LibDegorasSLR_LINK_LIBS}")
    message(STATUS "    DLL location: ${LibDegorasSLR_LOCATION}")
    message(STATUS "    DLL location debug: ${LibDegorasSLR_LOCATION_DEBUG}")
    message(STATUS "    DLL location release: ${LibDegorasSLR_LOCATION_RELEASE}")

ENDMACRO()

# **********************************************************************************************************************

# Macro to link target to LibDegorasSLR
MACRO(macro_link_libdegorasslr_default target visibility)

    message(STATUS "Linking LibDegorasSLR to target ${target} with ${visibility} visibility")

    target_link_libraries(${target} ${visibility} LibDegorasSLR::LibDegorasSLR)

    if(MODULES_GLOBAL_SHOW_EXTERNALS)
        get_target_property(LibDegorasSLR_INCLUDES LibDegorasSLR::LibDegorasSLR INTERFACE_INCLUDE_DIRECTORIES)
        file(GLOB_RECURSE EXTERNAL_HEADERS ${LibDegorasSLR_INCLUDES}/*)
        target_sources(${target} ${visibility} ${EXTERNAL_HEADERS})
    endif()

ENDMACRO()

# **********************************************************************************************************************
