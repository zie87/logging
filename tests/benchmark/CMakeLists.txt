PROJECT(logging)


ADD_EXECUTABLE(logging_benchmark  "${CMAKE_CURRENT_SOURCE_DIR}/logging_perf.cpp")
TARGET_LINK_LIBRARIES(logging_benchmark ${LIBS} logging "pthread")