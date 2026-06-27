# Third-party dependencies via vcpkg manifest mode (see vcpkg.json).
#
# P2 requires spdlog + x264 + libyuv (all default deps). protobuf remains
# optional until P3 (control protocol / `control` feature).
#
# Discovery notes (verified against the vcpkg ports):
#   - x264: vcpkg ships ONLY a pkg-config file (no CMake config), so find_package
#     CONFIG cannot work. Use pkg_check_modules, with a find_path/find_library
#     fallback for environments without pkg-config.
#   - libyuv: vcpkg ships a CMake config whose target is `yuv` (see the port's
#     `usage` file), not libyuv::libyuv.
# pkgconf is a vcpkg dependency to guarantee pkg-config is available everywhere.

find_package(spdlog CONFIG REQUIRED)
find_package(PkgConfig QUIET)

# --- x264 ---
if(PkgConfig_FOUND)
    pkg_check_modules(x264 IMPORTED_TARGET x264)
endif()
if(TARGET PkgConfig::x264)
    set(DUKE_X264_TARGET PkgConfig::x264)
else()
    # Fallback: locate header + lib directly (vcpkg toolchain supplies paths).
    find_path(X264_INCLUDE_DIR NAMES x264.h)
    find_library(X264_LIBRARY NAMES x264 libx264)
    if(X264_INCLUDE_DIR AND X264_LIBRARY)
        add_library(duke::x264 INTERFACE IMPORTED)
        target_include_directories(duke::x264 INTERFACE ${X264_INCLUDE_DIR})
        target_link_libraries(duke::x264 INTERFACE ${X264_LIBRARY})
        set(DUKE_X264_TARGET duke::x264)
    else()
        message(FATAL_ERROR
            "x264 not found: no pkg-config `x264` and no x264.h/libx264 on the "
            "search paths. Ensure vcpkg installs x264.")
    endif()
endif()

# --- libyuv ---
find_package(libyuv CONFIG QUIET)
if(libyuv_FOUND)
    if(TARGET yuv)
        set(DUKE_LIBYUV_TARGET yuv)
    elseif(TARGET libyuv::libyuv)
        set(DUKE_LIBYUV_TARGET libyuv::libyuv)
    elseif(TARGET libyuv)
        set(DUKE_LIBYUV_TARGET libyuv)
    else()
        message(FATAL_ERROR "libyuv config found but no target (yuv / libyuv::libyuv / libyuv)")
    endif()
else()
    if(PkgConfig_FOUND)
        pkg_check_modules(libyuv IMPORTED_TARGET libyuv)
    endif()
    if(TARGET PkgConfig::libyuv)
        set(DUKE_LIBYUV_TARGET PkgConfig::libyuv)
    else()
        message(FATAL_ERROR
            "libyuv not found: no CMake config and no pkg-config `libyuv`. "
            "Ensure vcpkg installs libyuv.")
    endif()
endif()

# --- protobuf (optional until P3) ---
find_package(Protobuf CONFIG QUIET)
if(NOT Protobuf_FOUND)
    message(STATUS "protobuf not found (expected until P3 control feature)")
endif()
