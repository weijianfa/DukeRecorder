# Third-party dependencies via vcpkg manifest mode (see vcpkg.json).
#
# P1 only truly links spdlog. x264 / libyuv / protobuf are declared in the
# manifest so the toolchain is primed, but are first consumed in P2/P3.

find_package(spdlog CONFIG REQUIRED)

# The following are intentionally optional in P1; they become REQUIRED once the
# phase that consumes them lands (P2: x264/libyuv, P3: protobuf).
find_package(x264 CONFIG QUIET)
find_package(libyuv CONFIG QUIET)
find_package(Protobuf CONFIG QUIET)

if(NOT x264_FOUND)
    message(STATUS "x264 not found (expected until P2)")
endif()
