# **********************************************************************************************************************
# LIBDEGORASSLR TESTS CMAKELIST
# **********************************************************************************************************************

# ----------------------------------------------------------------------------------------------------------------------
# CONFIGURATION

# Set the installation example path.
set(GLOBAL_LIBDPSLR_TESTS_INSTALL_PATH ${MODULES_GLOBAL_INSTALL_BIN_PATH}/testing)

# Ignore paths for basic unit test:
set(IGNORED_PATHS "UtilitiesSLR/UnitTest_PredictorSLR")

# ----------------------------------------------------------------------------------------------------------------------
# ADD THE MODULES (SIMPLE AND COMPLEX TESTS)
macro_add_subdirs_recursive("${CMAKE_CURRENT_SOURCE_DIR}" "${IGNORED_PATHS}")

# **********************************************************************************************************************
