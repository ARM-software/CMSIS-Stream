cmake_minimum_required (VERSION 3.14)
include(CMakePrintHelpers)

project(Example11)


add_executable(example11 generated/scheduler.cpp main.cpp)

dotdependency(example11 test ON graph.py)
cppdependency(example11 generated/scheduler.cpp graph.py)
add_sdf_dir(example11)

target_link_libraries(example11 PRIVATE examplenodes posix_runtime appcommon)
target_include_directories(example11 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(example11 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/generated)
