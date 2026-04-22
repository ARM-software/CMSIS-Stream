cmake_minimum_required (VERSION 3.14)
include(CMakePrintHelpers)

project(EventRecorder)


add_executable(eventrecorder generated/scheduler.cpp main.cpp)

dotdependency(eventrecorder event_recorder ON create.py)
cppdependency(eventrecorder generated/scheduler.cpp create.py pre_schedule_event_recorder.dot event_recorder.dot)
add_sdf_dir(eventrecorder)

target_include_directories(eventrecorder PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(eventrecorder PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/generated)
