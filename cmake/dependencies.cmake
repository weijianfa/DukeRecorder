# Third-party dependencies via vcpkg manifest mode (see vcpkg.json).
#
# P2 requires spdlog + x264 + libyuv (all default deps). protobuf remains
# optional until P3 (control protocol / `control` feature). pkgconf is pulled
# in as a fallback provider for pkg-config (libyuv's CMake config name varies
# across vcpkg versions, but vcpkg always installs a .pc file).

find_package(spdlog CONFIG REQUIRED)
find_package(x264 CONFIG REQUIRED)  # vcpkg exports x264::x264
find_package(PkgConfig QUIET)

# libyuv: CMake config name varies (libyuv / unofficial-libyuv); fall back to
# pkg-config, which vcpkg always provides.
find_package(libyuv CONFIG QUIET)
if(NOT libyuv_FOUND)
    find_package(unofficial-libyuv CONFIG QUIET)
endif()
if(NOT libyuv_FOUND AND PkgConfig_FOUND)
    pkg_check_modules(libyuv IMPORTED_TARGET libyuv)
endif()

find_package(Protobuf CONFIG QUIET)
if(NOT Protobuf_FOUND)
    message(STATUS "protobuf not found (expected until P3 control feature)")
endif()

# Normalize x264 to a single imported target name.
if(TARGET x264::x264)
    set(DUKE_X264_TARGET x264::x264)
elseif(TARGET x264)
    set(DUKE_X264_TARGET x264)
else()
    message(FATAL_ERROR "x264 found but no imported target (x264::x264 / x264)")
endif()

# Normalize libyuv to a single imported target name.
if(TARGET libyuv::libyuv)
    set(DUKE_LIBYUV_TARGET libyuv::libyuv)
elseif(TARGET unofficial::libyuv::libyuv)
    set(DUKE_LIBYUV_TARGET unofficial::libyuv::libyuv)
elseif(TARGET libyuv)
    set(DUKE_LIBYUV_TARGET libyuv)
elseif(TARGET PkgConfig::libyuv)
    set(DUKE_LIBYUV_TARGET PkgConfig::libyuv)
else()
    message(FATAL_ERROR
        "libyuv not found via CMake config or pkg-config. "
        "Ensure vcpkg installs libyuv (and pkgconf for the pkg-config fallback).")
endif()
