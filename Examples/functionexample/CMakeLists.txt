cmake_minimum_required (VERSION 3.14)
include(CMakePrintHelpers)

project(FunctionExample)


add_custom_target(functionexample ALL DEPENDS function.dot scheduler.cpp)


# No pre-schedule dot file generated
dotdependency(functionexample function OFF create.py)
cppdependency(functionexample scheduler.cpp create.py)

