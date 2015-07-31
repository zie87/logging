PROJECT(logging)

SET(TEST_LIBS ${LIBS} "unittest" "pthread")

ADD_SUBDIRECTORY (factory_blog)

ADD_EXECUTABLE(queues_tests  "${CMAKE_CURRENT_SOURCE_DIR}/queues/mpmc_buffer_test.cpp"
                             "${CMAKE_CURRENT_SOURCE_DIR}/queues/mpsc_queue_test.cpp"
                             "${CMAKE_CURRENT_SOURCE_DIR}/queues/spsc_buffer_test.cpp"
                             "${CMAKE_CURRENT_SOURCE_DIR}/queues/spsc_queue_test.cpp"
)
TARGET_LINK_LIBRARIES(queues_tests ${TEST_LIBS})
ADD_TEST( NAME queues_tests WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COMMAND "queues_tests" )
ADD_DEPENDENCIES(unittest_modules queues_tests)