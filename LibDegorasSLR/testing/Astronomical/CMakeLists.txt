# **********************************************************************************************************************
# LIBDEGORASSLR UNIX TESTS CMAKELIST
# **********************************************************************************************************************

# Ignore paths for basic unit test:
set(IGNORED_PATHS "")

# Setup the basic module unit tests.
macro_setup_lib_basic_unit_tests("${CMAKE_CURRENT_SOURCE_DIR}"
                                 "${GLOBAL_LIBDPSLR_TESTS_INSTALL_PATH}"
                                 "${IGNORED_PATHS}")

# **********************************************************************************************************************
