cmake_minimum_required (VERSION 3.14)
include(CMakePrintHelpers)

project(Example3)


add_executable(example3 generated/scheduler.cpp main.cpp app_config.cpp)

dotdependency(example3 test ON graph.py)
cppdependency(example3 generated/scheduler.cpp graph.py)
add_sdf_dir(example3)

target_link_libraries(example3 PRIVATE examplenodes posix_runtime appcommon)
target_include_directories(example3 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(example3 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/generated)

target_link_libraries(example3 PRIVATE CMSISDSP)

file(COPY input_example3.txt DESTINATION ${CMAKE_BINARY_DIR})