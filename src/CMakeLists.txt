PROJECT(logging)

ADD_LIBRARY(logging SHARED  "${CMAKE_CURRENT_SOURCE_DIR}/log/distributor.cpp"
                            "${CMAKE_CURRENT_SOURCE_DIR}/log/observer.cpp"
)


# ADD_EXECUTABLE(prototyp ./prototyp/main.cpp ./prototyp/worker.cpp)
# TARGET_LINK_LIBRARIES(prototyp boost_log boost_thread boost_program_options boost_chrono ${CMAKE_THREAD_LIBS_INIT})
