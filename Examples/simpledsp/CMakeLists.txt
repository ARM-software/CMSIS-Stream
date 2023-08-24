cmake_minimum_required (VERSION 3.14)
include(CMakePrintHelpers)

project(simpledsp)


add_executable(simpledsp generated/scheduler.cpp main.cpp)

dotdependency(simpledsp simpledsp ON create.py)
cppdependency(simpledsp generated/scheduler.cpp create.py)
add_sdf_dir(simpledsp)

target_include_directories(simpledsp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(simpledsp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/generated)
target_link_libraries(simpledsp PRIVATE CMSISDSP)