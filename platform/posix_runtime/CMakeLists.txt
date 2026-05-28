cmake_minimum_required(VERSION 3.20)


add_library(posix_runtime STATIC
    stream_event_queue.cpp
    stream_runtime_init.cpp
)

target_include_directories(posix_runtime PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_compile_features(posix_runtime PUBLIC cxx_std_17)

if(UNIX AND NOT APPLE)
    target_link_libraries(posix_runtime PUBLIC pthread)
endif()
