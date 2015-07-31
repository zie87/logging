PROJECT(logging)


ADD_EXECUTABLE(queues_benchmark  "${CMAKE_CURRENT_SOURCE_DIR}/queues/main.cpp")
TARGET_LINK_LIBRARIES(queues_benchmark ${LIBS} "pthread")