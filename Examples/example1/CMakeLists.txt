cmake_minimum_required (VERSION 3.14)
include(CMakePrintHelpers)

project(Example1)


add_executable(example1 generated/scheduler.cpp main.cpp)

dotdependency(example1 test ON graph.py)
cppdependency(example1 generated/scheduler.cpp graph.py)
add_sdf_dir(example1)

target_link_libraries(example1 PRIVATE posix_runtime appcommon)

target_include_directories(example1 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(example1 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/generated)
