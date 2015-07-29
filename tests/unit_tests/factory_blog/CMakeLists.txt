PROJECT(logging)

ADD_EXECUTABLE(fb_sdo_test  "${CMAKE_CURRENT_SOURCE_DIR}/dynamic_observer/sdo_test.cpp"
)
TARGET_LINK_LIBRARIES(fb_sdo_test ${TEST_LIBS} fb_sdo)
ADD_TEST( NAME fb_sdo_test WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COMMAND "fb_sdo_test" )
ADD_DEPENDENCIES(unittest_modules fb_sdo_test)

# ADD_EXECUTABLE(container_tests  "${CMAKE_CURRENT_SOURCE_DIR}/container/circular_buffer_test.cpp"
# )
# TARGET_LINK_LIBRARIES(container_tests ${TEST_LIBS} logging)
# ADD_TEST( NAME container_tests WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COMMAND "container_tests" )
# ADD_DEPENDENCIES(unittest_modules container_tests)

# ADD_EXECUTABLE(thread_tests  "${CMAKE_CURRENT_SOURCE_DIR}/thread/safe_circular_buffer_test.cpp"
# )
# TARGET_LINK_LIBRARIES(thread_tests ${TEST_LIBS} logging)
# ADD_TEST( NAME thread_tests WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COMMAND "thread_tests" )
# ADD_DEPENDENCIES(unittest_modules thread_tests)
