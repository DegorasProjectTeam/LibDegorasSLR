# **********************************************************************************************************************
# Updated 23/01/2024
# **********************************************************************************************************************

# ----------------------------------------------------------------------------------------------------------------------
# ComplexTest_GeneratePassesCPF configuration

# Test name.
set(TEST_NAME "ComplexTest_GeneratePassesCPF")

# Set the installation path.
set(TEST_INSTALL_PATH ${GLOBAL_LIBDPSLR_TESTS_INSTALL_PATH}/${TEST_NAME})

# Configure names and build path.
set(APP_BUILD_FOLDER ${CMAKE_BINARY_DIR}/bin/Testing/${TEST_NAME})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${APP_BUILD_FOLDER})

# Log.
message(STATUS "Setup complex test: ${TEST_NAME}")

# Set the sources.
set(SOURCES "ComplexTest_GeneratePassesCPF.cpp")

# Set resources.
set(RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/test_data/39380_cpf_230309_5681.tjr)
source_group("Resource Files" FILES ${RESOURCES})

# Include the external resources.
if(MODULES_GLOBAL_SHOW_EXTERNALS)
    file(GLOB_RECURSE EXTERN ${CMAKE_SOURCE_DIR}/includes/*.h)
endif()

# Setup the launcher.
macro_setup_launcher("${TEST_NAME}"
                     "${MODULES_GLOBAL_LIBS_OPTIMIZED}"
                     "${MODULES_GLOBAL_LIBS_DEBUG}"
                     "${SOURCES}" "${EXTERN}")

# Include directories for the target.
target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/includes)

# Install the launcher.
macro_install_launcher(${TEST_NAME} ${TEST_INSTALL_PATH})

# Install runtime artifacts.
macro_install_runtime_artifacts(${TEST_NAME}
                                ${MODULES_GLOBAL_MAIN_DEP_SET_NAME}
                                ${TEST_INSTALL_PATH})

#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test_data/39380_cpf_230309_5681.tjr
#     DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_data)


