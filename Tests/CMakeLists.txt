cmake_minimum_required (VERSION 3.14)
include(CMakePrintHelpers)

project(test)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

add_executable(test main_duplicate_sync.cpp duplicate_sync/scheduler.cpp)

target_include_directories(test PRIVATE duplicate_sync)
target_include_directories(test PRIVATE .)
target_include_directories(test PRIVATE ../Include/cg/src)
