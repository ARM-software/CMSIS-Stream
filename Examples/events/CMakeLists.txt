cmake_minimum_required (VERSION 3.14)
include(CMakePrintHelpers)

project(Events)


add_executable(events scheduler.cpp host_com.cpp main.cpp )

dotdependency(events events ON create.py)
cppdependency(events scheduler.cpp create.py)
add_sdf_dir(events)

target_link_libraries(events PRIVATE posix_runtime appcommon)

target_include_directories(events PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(events PUBLIC CG_EVENTS)