PROJECT(logging)

ADD_EXECUTABLE(fb_sdo_test  "${CMAKE_CURRENT_SOURCE_DIR}/dynamic_observer/sdo_test.cpp"
)
TARGET_LINK_LIBRARIES(fb_sdo_test ${TEST_LIBS} fb_sdo)
ADD_TEST( NAME fb_sdo_test WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COMMAND "fb_sdo_test" )
ADD_DEPENDENCIES(unittest_modules fb_sdo_test)


# ADD_EXECUTABLE(fb_ado_test  "${CMAKE_CURRENT_SOURCE_DIR}/async_observer/message_queue_test.cpp"
#                             "${CMAKE_CURRENT_SOURCE_DIR}/async_observer/ado_test.cpp"
# )
# TARGET_LINK_LIBRARIES(fb_ado_test ${TEST_LIBS} fb_ado)
# ADD_TEST( NAME fb_ado_test WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COMMAND "fb_ado_test" )
# ADD_DEPENDENCIES(unittest_modules fb_ado_test)

