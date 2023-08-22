cmake_minimum_required (VERSION 3.14)
include(CMakePrintHelpers)

project(Example9)


add_executable(example9 generated/scheduler.cpp main.cpp)

dotdependency(example9 test graph.py)
cppdependency(example9 generated/scheduler.cpp graph.py)
add_sdf_dir(example9)

target_include_directories(example9 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(example9 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/generated)
