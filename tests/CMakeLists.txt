PROJECT(logging)

# ADD_DEFINITIONS(-DTEST_DATA="${CMAKE_CURRENT_SOURCE_DIR}/data")

ADD_SUBDIRECTORY (benchmark)
ADD_SUBDIRECTORY (unit_tests)

ADD_EXECUTABLE(test_impl  "${CMAKE_CURRENT_SOURCE_DIR}/test_impl/main.cpp"
                          "${CMAKE_CURRENT_SOURCE_DIR}/test_impl/core.cpp")
TARGET_LINK_LIBRARIES(test_impl ${LIBS} "pthread")