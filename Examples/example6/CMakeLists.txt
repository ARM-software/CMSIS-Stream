cmake_minimum_required (VERSION 3.14)
include(CMakePrintHelpers)

project(Example6)


add_executable(example6 generated/scheduler.cpp main.cpp mfccConfigData.c)

dotdependency(example6 test ON graph.py)
cppdependency(example6 generated/scheduler.cpp graph.py)
add_sdf_dir(example6)

target_include_directories(example6 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(example6 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/generated)

target_link_libraries(example6 PRIVATE CMSISDSP)

file(COPY input_example6.txt DESTINATION ${CMAKE_BINARY_DIR})