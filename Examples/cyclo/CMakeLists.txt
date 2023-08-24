cmake_minimum_required (VERSION 3.14)
include(CMakePrintHelpers)

project(cyclo)


add_executable(cyclo generated/scheduler.cpp main.cpp)

dotdependency(cyclo cyclo ON create.py)
cppdependency(cyclo generated/scheduler.cpp create.py)
add_sdf_dir(cyclo)

target_include_directories(cyclo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(cyclo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/generated)
