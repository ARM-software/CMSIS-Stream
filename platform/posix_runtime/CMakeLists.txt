cmake_minimum_required(VERSION 3.20)


add_library(posix_runtime STATIC
    cg_queue.cpp posix_thread.cpp
)

target_include_directories(posix_runtime PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)
