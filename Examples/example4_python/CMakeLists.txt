cmake_minimum_required (VERSION 3.14)
include(CMakePrintHelpers)

project(Example4)


add_custom_target(example4 ALL DEPENDS sched.py)

dotdependency(example4 test graph.py)
pythondependency(example4 main.py graph.py)
