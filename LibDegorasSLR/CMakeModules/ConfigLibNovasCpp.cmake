# **********************************************************************************************************************
# Updated 29/01/2024
# **********************************************************************************************************************

# **********************************************************************************************************************

# Macro to search for LibNovasCpp
MACRO(macro_configure_libnovascpp_default version version_mode extra_search_paths extra_search_patterns)

    # Log.
    message(STATUS "Configuring LibNovasCPP...")

    # Configure extra things if neccesary.
    # ...

    # Find the package.
    macro_find_package_default("LibNovasCpp" "${version}" ${version_mode} "${extra_search_paths}" "${extra_search_patterns}")

    # Logs.
    get_target_property(libnovascpp_includes LibNovasCpp::LibNovasCpp INTERFACE_INCLUDE_DIRECTORIES)
    get_target_property(libnovascpp_link_libs LibNovasCpp::LibNovasCpp INTERFACE_LINK_LIBRARIES)
    get_target_property(libnovascpp_location LibNovasCpp::LibNovasCpp LOCATION)
    get_target_property(libnovascpp_location_debug LibNovasCpp::LibNovasCpp IMPORTED_LOCATION_DEBUG)
    get_target_property(libnovascpp_location_release LibNovasCpp::LibNovasCpp IMPORTED_LOCATION_RELEASE)
    message(STATUS "  LibNovasCpp::LibNovasCpp information:")
    message(STATUS "    Library version: ${LibNovasCpp_VERSION}")
    message(STATUS "    Include directories: ${libnovascpp_includes}")
    message(STATUS "    Interface link libraries: ${libnovascpp_link_libs}")
    message(STATUS "    DLL location: ${libnovascpp_location}")
    message(STATUS "    DLL location debug: ${libnovascpp_location_debug}")
    message(STATUS "    DLL location release: ${libnovascpp_location_release}")

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
