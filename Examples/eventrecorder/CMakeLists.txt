cmake_minimum_required (VERSION 3.14)
include(CMakePrintHelpers)

project(EventRecorder)


add_executable(eventrecorder generated/scheduler.cpp main.cpp)

dotdependency(eventrecorder event_recorder create.py)
cppdependency(eventrecorder generated/scheduler.cpp create.py)
add_sdf_dir(eventrecorder)

target_include_directories(eventrecorder PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(eventrecorder PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/generated)
