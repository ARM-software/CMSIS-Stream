cmake_minimum_required (VERSION 3.14)
include(CMakePrintHelpers)

project(Callback)


add_executable(callback scheduler.cpp main.cpp)

dotdependency(callback callback ON create.py)
cppdependency(callback scheduler.cpp create.py)
add_sdf_dir(callback)

target_include_directories(callback PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
