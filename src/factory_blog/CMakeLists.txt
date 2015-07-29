PROJECT(logging)

ADD_LIBRARY(fb_sdo SHARED  "${CMAKE_CURRENT_SOURCE_DIR}/dynamic_observer/console_observer.cpp"
                           "${CMAKE_CURRENT_SOURCE_DIR}/dynamic_observer/distributor.cpp"
)


# ADD_EXECUTABLE(prototyp ./prototyp/main.cpp ./prototyp/worker.cpp)
# TARGET_LINK_LIBRARIES(prototyp boost_log boost_thread boost_program_options boost_chrono ${CMAKE_THREAD_LIBS_INIT})
