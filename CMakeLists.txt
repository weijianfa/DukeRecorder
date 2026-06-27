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

# Core is header-only (platform/config/frame/ring_buffer).
add_library(duke_core INTERFACE)
target_include_directories(duke_core INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(duke_core INTERFACE spdlog::spdlog)

# Encode: Annex-B writer (portable) + libyuv color convert + x264 encoder.
add_library(duke_encode STATIC
    src/encode/annexb_writer.cpp
    src/encode/color_convert.cpp
    src/encode/x264_encoder.cpp)
target_include_directories(duke_encode PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(duke_encode
    PUBLIC duke_core
    PRIVATE ${DUKE_X264_TARGET} ${DUKE_LIBYUV_TARGET})

# Capture: abstract interface + TestCaptureEngine (portable) + DXGI (Windows).
add_library(duke_capture STATIC
    src/capture/test_capture.cpp
    src/capture/dxgi_capture.cpp)
target_include_directories(duke_capture PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(duke_capture PUBLIC duke_core)
if(WIN32)
    target_link_libraries(duke_capture PUBLIC ${DUKE_PLATFORM_LIBS})
endif()

# Mux: raw Annex-B ES file sink (stands in for the P4 fmp4_muxer).
add_library(duke_mux STATIC src/mux/raw_es_sink.cpp)
target_include_directories(duke_mux PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(duke_mux PUBLIC duke_core)

# Main executable: CLI + Recorder orchestration + capture/encode/mux.
add_executable(duke_recorder
    src/main.cpp
    src/cli.cpp
    src/recorder.cpp)
target_link_libraries(duke_recorder PRIVATE duke_capture duke_encode duke_mux duke_core)

if(DUKE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(DUKE_ENABLE_STYLE_TARGETS)
    duke_add_style_targets("${CMAKE_CURRENT_SOURCE_DIR}/src")
endif()

message(STATUS "DukeRecorder ${PROJECT_VERSION} configured for ${CMAKE_SYSTEM_NAME}")
