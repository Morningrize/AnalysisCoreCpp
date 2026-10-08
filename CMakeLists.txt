cmake_minimum_required(VERSION 3.14)
project(AnalysisCoreCpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CMakePackageConfigHelpers)

option(BUILD_TESTS "Build tests" OFF)
option(ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)

set(FETCHED_CXXOPTS OFF)
set(FETCHED_CXXOPTS_TARGET)

# ----------------------
# Dependencies
# ----------------------

# cxxopts
find_package(cxxopts 3.2.0 EXACT CONFIG QUIET)

if(NOT cxxopts_FOUND OR NOT TARGET cxxopts::cxxopts)
    message("Fetching cxxopts")
    include(FetchContent)
    FetchContent_Declare(
        cxxopts
        GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
        GIT_TAG        v3.2.0
    )
    FetchContent_MakeAvailable(cxxopts)
    SET(FETCHED_CXXOPTS_TARGET cxxopts)
    set(FETCHED_CXXOPTS ON)
endif()

# ----------------------
# Library
# ----------------------

file(
    GLOB_RECURSE SOURCES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx
)

add_library(AnalysisCoreCpp SHARED ${SOURCES})
add_library(AnalysisCoreCpp::AnalysisCoreCpp ALIAS AnalysisCoreCpp)
target_include_directories(AnalysisCoreCpp
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(AnalysisCoreCpp
    PUBLIC cxxopts::cxxopts
)

# Export a build-tree package config for find_package(AnalysisCoreCpp CONFIG)
set(ANALYSISCORECPP_PACKAGE_DIR "${CMAKE_CURRENT_BINARY_DIR}/cmake")
set(ANALYSISCORECPP_CONFIG_FILE "${ANALYSISCORECPP_PACKAGE_DIR}/AnalysisCoreCppConfig.cmake")
set(ANALYSISCORECPP_TARGETS_FILE "${ANALYSISCORECPP_PACKAGE_DIR}/AnalysisCoreCppTargets.cmake")

export(
    TARGETS AnalysisCoreCpp ${FETCHED_CXXOPTS_TARGET}
    FILE "${ANALYSISCORECPP_TARGETS_FILE}"
    NAMESPACE AnalysisCoreCpp::
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/AnalysisCoreCppConfig.cmake.in"
    "${ANALYSISCORECPP_CONFIG_FILE}"
    INSTALL_DESTINATION "cmake"
)

# ----------------------
# Tests
# ----------------------

if(BUILD_TESTS)
    find_package(GTest REQUIRED)

    if(GTest_FOUND)
        message(STATUS "GTest found")
        enable_testing()
        include(GoogleTest)

        file(
            GLOB TEST_SOURCES
            CONFIGURE_DEPENDS
            tests/test_*.cpp
        )

        foreach(test_src ${TEST_SOURCES})
            get_filename_component(test_name ${test_src} NAME_WE)
            add_executable(${test_name} EXCLUDE_FROM_ALL ${test_src})
            target_include_directories(
                ${test_name}
                PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
            )
            target_link_libraries(${test_name}
                PRIVATE
                    AnalysisCoreCpp
                    cxxopts::cxxopts
                    GTest::gtest
                    GTest::gtest_main
            )
            gtest_discover_tests(${test_name})
            list(APPEND ALL_TEST_TARGETS ${test_name})
        endforeach()
        add_custom_target(tests DEPENDS ${ALL_TEST_TARGETS})
    else()
        message(WARNING "GTest not found. Tests will not be built. Install GTest to enable tests.")
    endif()
endif()

# ----------------------
# Clang-Tidy
# ----------------------

if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY clang-tidy)
    if(CLANG_TIDY)
        file(
            GLOB_RECURSE ALL_SOURCE_FILES
            ${CMAKE_SOURCE_DIR}/include/*.h
            ${CMAKE_SOURCE_DIR}/src/*.cxx
            ${CMAKE_SOURCE_DIR}/tests/*.cpp
        )
        add_custom_target(tidy
            COMMAND ${CLANG_TIDY}
                -p ${CMAKE_BINARY_DIR}
                ${ALL_SOURCE_FILES}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Running clang-tidy on all source files..."
        )
    else()
        message(WARNING "clang-tidy not found. 'tidy' target will not be available.")
    endif()
endif()