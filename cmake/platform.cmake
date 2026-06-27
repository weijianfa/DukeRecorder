# Platform detection. Sets DUKE_PLATFORM_WINDOWS / DUKE_PLATFORM_MACOS and the
# platform-specific system libraries each module will link against.
#
# P1 only declares the wiring; the capture/encode backends consume these in P2/P5.

if(WIN32)
    add_compile_definitions(DUKE_PLATFORM_WINDOWS=1)
    set(DUKE_PLATFORM_LIBS
        d3d11 dxgi dxguid mfplat mfuuid wmcodecdspuuid
        CACHE STRING "Windows system libraries" FORCE)
elseif(APPLE)
    add_compile_definitions(DUKE_PLATFORM_MACOS=1)
    find_library(COREVIDEO_LIB CoreVideo REQUIRED)
    find_library(COREMEDIA_LIB CoreMedia REQUIRED)
    find_library(VIDEOTOOLBOX_LIB VideoToolbox REQUIRED)
    find_library(COREGRAPHICS_LIB CoreGraphics REQUIRED)
    set(DUKE_PLATFORM_LIBS
        ${COREVIDEO_LIB} ${COREMEDIA_LIB} ${VIDEOTOOLBOX_LIB} ${COREGRAPHICS_LIB}
        CACHE STRING "macOS system frameworks" FORCE)
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}. "
        "DukeRecorder targets Windows 10+ and macOS 13+ only.")
endif()
