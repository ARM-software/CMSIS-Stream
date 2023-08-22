cmake_minimum_required (VERSION 3.14)
include(CMakePrintHelpers)

project(Example7)


add_custom_target(example7 ALL DEPENDS sched.py)

dotdependency(example7 test graph.py)
pythondependency(example7 main.py graph.py)
