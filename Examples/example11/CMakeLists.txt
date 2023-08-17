cmake_minimum_required (VERSION 3.14)
include(CMakePrintHelpers)

project(Example11)


add_executable(example11 main.cpp)

sdf(example11 graph.py test)
add_sdf_dir(example11)

target_include_directories(example11 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(example11 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/generated)
