cmake_minimum_required (VERSION 3.14)
include(CMakePrintHelpers)

project(RuntimeMode)


add_executable(runtime_mode scheduler.cpp main.cpp sched_flat.c)

dotdependency(runtime_mode runtime_mode ON create.py)
cppdependency(runtime_mode scheduler.cpp create.py)
add_sdf_dir(runtime_mode)

target_include_directories(runtime_mode PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(runtime_mode PRIVATE ${FLAT})
