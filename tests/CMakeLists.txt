# P2 tests registered with CTest. The stub-only tests (test_x264_encoder,
# test_color_convert) reference globals that exist only in the local stubs, so
# they cannot link against real x264/libyuv and are NOT registered here — they
# run locally via g++ against /tmp stubs (see each file's header comment).

add_executable(test_ring_buffer test_ring_buffer.cpp)
target_link_libraries(test_ring_buffer PRIVATE duke_core)
add_test(NAME ring_buffer COMMAND test_ring_buffer)

add_executable(test_annexb_writer test_annexb_writer.cpp)
target_link_libraries(test_annexb_writer PRIVATE duke_encode duke_core)
add_test(NAME annexb_writer COMMAND test_annexb_writer)

add_executable(test_raw_es_sink test_raw_es_sink.cpp)
target_link_libraries(test_raw_es_sink PRIVATE duke_mux duke_core)
add_test(NAME raw_es_sink COMMAND test_raw_es_sink)

add_executable(test_test_capture test_test_capture.cpp)
target_link_libraries(test_test_capture PRIVATE duke_capture duke_core)
add_test(NAME test_capture COMMAND test_test_capture)

# Uses a FakeEncoder (no x264); exercises real Recorder + libyuv + sink wiring.
add_executable(test_recorder test_recorder.cpp)
target_sources(test_recorder PRIVATE "${CMAKE_SOURCE_DIR}/src/recorder.cpp")
target_link_libraries(test_recorder PRIVATE duke_capture duke_encode duke_mux duke_core)
add_test(NAME recorder COMMAND test_recorder)

add_executable(test_main_args test_main_args.cpp)
target_sources(test_main_args PRIVATE "${CMAKE_SOURCE_DIR}/src/cli.cpp")
target_link_libraries(test_main_args PRIVATE duke_core)
add_test(NAME main_args COMMAND test_main_args)
