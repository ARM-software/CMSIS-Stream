cmake_minimum_required (VERSION 3.14)
include(CMakePrintHelpers)

project(Simple)


add_executable(simple scheduler.cpp main.cpp)

dotdependency(simple simple ON create.py)
cppdependency(simple scheduler.cpp create.py)
add_sdf_dir(simple)

target_link_libraries(simple PRIVATE posix_runtime appcommon)

target_include_directories(simple PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
