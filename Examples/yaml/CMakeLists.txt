cmake_minimum_required (VERSION 3.14)
include(CMakePrintHelpers)

project(Yaml)


add_custom_target(yaml ALL)

dotdependency(yaml test graph.py)
cppdependency(yaml scheduler.cpp graph.py)

