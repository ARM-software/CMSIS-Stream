cmake_minimum_required (VERSION 3.14)
include(CMakePrintHelpers)

project(FunctionExample)


add_custom_target(functionexample ALL)

# No pre-schedule dot file generated
dotdependency(functionexample function create.py OFF)
cppdependency(functionexample scheduler.cpp create.py)
