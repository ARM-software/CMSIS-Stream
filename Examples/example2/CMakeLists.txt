cmake_minimum_required (VERSION 3.14)
include(CMakePrintHelpers)

project(Example2)


add_executable(example2 generated/scheduler.cpp main.cpp)

dotdependency(example2 test ON graph.py )
cppdependency(example2 generated/scheduler.cpp graph.py)
add_sdf_dir(example2)

target_link_libraries(example2 PRIVATE examplenodes posix_runtime appcommon)

target_include_directories(example2 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(example2 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/generated)

target_link_libraries(example2 PRIVATE CMSISDSP)