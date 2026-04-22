cmake_minimum_required (VERSION 3.14)
include(CMakePrintHelpers)

project(Memory)

add_custom_target(memory ALL DEPENDS generated/scheduler.cpp)
dotdependency(memory test OFF create.py)
cppdependency(memory generated/scheduler.cpp create.py test.dot)
