cmake_minimum_required(VERSION 3.20)

project(DukeRecorder
    VERSION 1.0.0
    DESCRIPTION "Enterprise-grade cross-platform desktop screen recording engine"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Export compile_commands.json for clangd / clang-tidy.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(DUKE_BUILD_TESTS "Build unit tests" ON)
option(DUKE_ENABLE_STYLE_TARGETS "Add clang-format / clang-tidy targets" ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(platform)
include(dependencies)
include(google_style)

# Core is header-only in P1 (platform/config/frame/ring_buffer). Expose as an
# INTERFACE library so downstream targets pick up include paths and deps.
add_library(duke_core INTERFACE)
target_include_directories(duke_core INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(duke_core INTERFACE spdlog::spdlog)

# Main executable. P1: parse config, print version, exit cleanly.
add_executable(duke_recorder src/main.cpp)
target_link_libraries(duke_recorder PRIVATE duke_core)

if(DUKE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(DUKE_ENABLE_STYLE_TARGETS)
    duke_add_style_targets("${CMAKE_CURRENT_SOURCE_DIR}/src")
endif()

message(STATUS "DukeRecorder ${PROJECT_VERSION} configured for ${CMAKE_SYSTEM_NAME}")
