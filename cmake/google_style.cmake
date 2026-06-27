# Google-style toolchain targets.
#
# Provides:
#   duke_format_check  - clang-format --dry-run -Werror over the codebase
#   duke_tidy          - clang-tidy with google-* checks over src/core/** (P1)
#
# clang-tidy is scoped to all of src (P2 widened from src/core). Platform-
# guarded files (dxgi_capture) compile to empty TUs off-Windows, which tidy
# handles without diagnostics.

find_program(CLANG_FORMAT_BIN NAMES clang-format clang-format-17 clang-format-16)
find_program(CLANG_TIDY_BIN NAMES clang-tidy clang-tidy-17 clang-tidy-16)

function(duke_add_style_targets source_dir)
    if(CLANG_FORMAT_BIN)
        file(GLOB_RECURSE DUKE_FORMAT_SOURCES
            "${source_dir}/*.cpp" "${source_dir}/*.h" "${source_dir}/*.mm")
        add_custom_target(duke_format_check
            COMMAND ${CLANG_FORMAT_BIN}
                --dry-run -Werror ${DUKE_FORMAT_SOURCES}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "clang-format dry-run check (Google style)")
    else()
        message(STATUS "clang-format not found; duke_format_check disabled")
    endif()

    if(CLANG_TIDY_BIN)
        file(GLOB_RECURSE DUKE_TIDY_SOURCES
            "${source_dir}/*.cpp" "${source_dir}/*.h")
        add_custom_target(duke_tidy
            COMMAND ${CLANG_TIDY_BIN} -p ${CMAKE_BINARY_DIR}
                --config-file=${CMAKE_SOURCE_DIR}/.clang-tidy
                ${DUKE_TIDY_SOURCES}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "clang-tidy (google-* checks) over src")
    else()
        message(STATUS "clang-tidy not found; duke_tidy disabled")
    endif()
endfunction()
