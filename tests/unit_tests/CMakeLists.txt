PROJECT(logging)

SET(TEST_LIBS ${LIBS} "unittest" "pthread")

ADD_EXECUTABLE(logging_tests  "${CMAKE_CURRENT_SOURCE_DIR}/log/log_test.cpp"
                              "${CMAKE_CURRENT_SOURCE_DIR}/log/source_test.cpp"
)
TARGET_LINK_LIBRARIES(logging_tests ${TEST_LIBS} logging)
ADD_TEST( NAME logging_tests WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COMMAND "logging_tests" )
ADD_DEPENDENCIES(unittest_modules logging_tests)

ADD_EXECUTABLE(container_tests  "${CMAKE_CURRENT_SOURCE_DIR}/container/circular_buffer_test.cpp"
)
TARGET_LINK_LIBRARIES(container_tests ${TEST_LIBS} logging)
ADD_TEST( NAME container_tests WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COMMAND "container_tests" )
ADD_DEPENDENCIES(unittest_modules container_tests)

ADD_EXECUTABLE(thread_tests  "${CMAKE_CURRENT_SOURCE_DIR}/thread/safe_circular_buffer_test.cpp"
)
TARGET_LINK_LIBRARIES(thread_tests ${TEST_LIBS} logging)
ADD_TEST( NAME thread_tests WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COMMAND "thread_tests" )
ADD_DEPENDENCIES(unittest_modules thread_tests)