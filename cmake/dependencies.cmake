# Third-party dependencies via vcpkg manifest mode (see vcpkg.json).
#
# P2 requires spdlog + x264 + libyuv (all default deps now). protobuf remains
# optional until P3 (control protocol / `control` feature).

find_package(spdlog CONFIG REQUIRED)
find_package(x264 CONFIG REQUIRED)
find_package(libyuv CONFIG REQUIRED)

find_package(Protobuf CONFIG QUIET)
if(NOT Protobuf_FOUND)
    message(STATUS "protobuf not found (expected until P3 control feature)")
endif()

# vcpkg exposes x264 / libyuv under varying imported-target names across
# versions; normalize to DUKE_X264_TARGET / DUKE_LIBYUV_TARGET.
if(TARGET x264::x264)
    set(DUKE_X264_TARGET x264::x264)
elseif(TARGET x264)
    set(DUKE_X264_TARGET x264)
else()
    message(FATAL_ERROR "x264 found but no imported target (x264::x264 / x264)")
endif()

if(TARGET libyuv::libyuv)
    set(DUKE_LIBYUV_TARGET libyuv::libyuv)
elseif(TARGET unofficial::libyuv::libyuv)
    set(DUKE_LIBYUV_TARGET unofficial::libyuv::libyuv)
elseif(TARGET libyuv)
    set(DUKE_LIBYUV_TARGET libyuv)
else()
    message(FATAL_ERROR "libyuv found but no imported target (libyuv::libyuv / unofficial::libyuv::libyuv / libyuv)")
endif()
