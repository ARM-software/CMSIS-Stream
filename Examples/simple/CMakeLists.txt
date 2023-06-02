cmake_minimum_required (VERSION 3.14)
include(CMakePrintHelpers)

project(Simple)


add_executable(simple main.cpp)

sdfroot(simple create.py simple)
add_sdf_dir(simple)

target_include_directories(simple PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
