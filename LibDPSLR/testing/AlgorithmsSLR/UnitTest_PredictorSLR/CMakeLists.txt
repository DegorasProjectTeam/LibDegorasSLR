include(ExternalData)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++17 -fopenmp")
set(APP_NAME "UnitTest_PredictorSLR")
set(SOURCES "UnitTest_PredictorSLR.cpp")
set(LIBRARIES LibDPSLR)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/test/AlgorithmsSLR/${APP_NAME})

include_directories(${CMAKE_SOURCE_DIR}/includes ${CMAKE_SOURCE_DIR})

if (WIN32)
    link_directories(${CMAKE_CURRENT_BINARY_DIR})
endif (WIN32)

if (UNIX)
    link_directories(${CMAKE_CURRENT_BINARY_DIR}/../lib)
endif (UNIX)

macro_setup_launcher(${APP_NAME})

#source_group("Resource Files" FILES ${RESOURCES})

add_dependencies(${APP_NAME} LibDPSLR)

add_test(NAME ${APP_NAME} COMMAND ${APP_NAME})
