# P1 tests. Header-only core is exercised via a standalone (no-gtest) test
# binary registered with CTest. Later phases add gtest + per-module suites.

add_executable(test_ring_buffer test_ring_buffer.cpp)
target_link_libraries(test_ring_buffer PRIVATE duke_core)
target_compile_features(test_ring_buffer PRIVATE cxx_std_17)

add_test(NAME ring_buffer COMMAND test_ring_buffer)
