PROJECT(logging)

# ADD_DEFINITIONS(-DTEST_DATA="${CMAKE_CURRENT_SOURCE_DIR}/data")

ADD_SUBDIRECTORY (benchmark)
ADD_SUBDIRECTORY (unit_tests)

ADD_EXECUTABLE(test_impl  "${CMAKE_CURRENT_SOURCE_DIR}/test_impl/test_main.cpp"
                          "${CMAKE_CURRENT_SOURCE_DIR}/test_impl/core.cpp")
TARGET_LINK_LIBRARIES(test_impl ${LIBS} "pthread")

ADD_EXECUTABLE(bench_impl  "${CMAKE_CURRENT_SOURCE_DIR}/test_impl/bench_main.cpp"
                           "${CMAKE_CURRENT_SOURCE_DIR}/test_impl/core.cpp")
TARGET_LINK_LIBRARIES(bench_impl boost_log boost_thread boost_program_options boost_chrono boost_system ${CMAKE_THREAD_LIBS_INIT})