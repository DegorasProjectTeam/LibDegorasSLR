# **********************************************************************************************************************
# LIBDEGORASSLR EXAMPLE CMAKELIST
# **********************************************************************************************************************

# ----------------------------------------------------------------------------------------------------------------------
# CONFIGURATION

# Config.
set(MODULE_NAME Astronomical)
set(EXAMPLE_NAME PredictorStarNovas)
# --
set(EXAMPLE_DIR ${CMAKE_SOURCE_DIR}/examples/${MODULE_NAME}/${EXAMPLE_NAME})
set(EXAMPLE_FULL_NAME ${MODULE_NAME}_${EXAMPLE_NAME})
set(EXAMPLE_INSTALL_PATH ${GLOBAL_LIBDPSLR_EXAMPLES_INSTALL_PATH}/${EXAMPLE_FULL_NAME})
set(EXAMPLE_SOURCES ${EXAMPLE_DIR}/${EXAMPLE_FULL_NAME}.cpp)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Examples/${EXAMPLE_FULL_NAME})

# Add a custom target for Python scripts to make them visible in IDEs
file(GLOB PYTHON_SCRIPTS "${EXAMPLE_DIR}/python_scripts/*.py")
source_group("PythonScripts" FILES ${PYTHON_SCRIPTS})

# Setup the launcher.
macro_setup_launcher("${EXAMPLE_FULL_NAME}"
                     "${MODULES_GLOBAL_LIBS_OPTIMIZED}"
                     "${MODULES_GLOBAL_LIBS_DEBUG}"
                     "${EXAMPLE_SOURCES}" "${PYTHON_SCRIPTS}")

# Add to the build step the necessary input files for the example.
#add_custom_command(TARGET "${EXAMPLE_FULL_NAME}" PRE_BUILD
#                   COMMAND ${CMAKE_COMMAND} -E copy_directory
#                   "${EXAMPLE_DIR}/inputs/" "$<TARGET_FILE_DIR:${EXAMPLE_FULL_NAME}>/inputs")

# Add to the build step the necessary python scripts.
add_custom_command(TARGET "${EXAMPLE_FULL_NAME}" PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   "${EXAMPLE_DIR}/python_scripts/" "$<TARGET_FILE_DIR:${EXAMPLE_FULL_NAME}>/python_scripts")

# ----------------------------------------------------------------------------------------------------------------------
# INSTALLATION PROCESS

# Prepare the external library search folders.
set(ext_libs_loc "${MODULES_GLOBAL_INSTALL_LIB_PATH}"
                 "${MODULES_GLOBAL_INSTALL_BIN_PATH}")

# Install the launcher.
macro_install_launcher("${EXAMPLE_FULL_NAME}"
                       "${EXAMPLE_INSTALL_PATH}")

# Install runtime artifacts.
macro_install_runtime_artifacts("${EXAMPLE_FULL_NAME}"
                                "${MODULES_GLOBAL_MAIN_DEP_SET_NAME}"
                                "${EXAMPLE_INSTALL_PATH}")

# Install the runtime dependencies.
macro_install_runtime_deps("${EXAMPLE_FULL_NAME}"
                            "${MODULES_GLOBAL_MAIN_DEP_SET_NAME}"
                            "${ext_libs_loc}"
                            "${EXAMPLE_INSTALL_PATH}"
                            "" "")

# Install example input data.
#install(DIRECTORY "${EXAMPLE_DIR}/inputs/"
#        DESTINATION "${EXAMPLE_INSTALL_PATH}/inputs"
#        FILES_MATCHING PATTERN "*")

# Install auxiliar python scripts.
install(DIRECTORY "${EXAMPLE_DIR}/python_scripts/"
        DESTINATION "${EXAMPLE_INSTALL_PATH}/python_scripts"
        FILES_MATCHING PATTERN "*.py")

# ----------------------------------------------------------------------------------------------------------------------

# **********************************************************************************************************************
