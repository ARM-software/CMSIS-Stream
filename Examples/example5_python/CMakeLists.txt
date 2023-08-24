cmake_minimum_required (VERSION 3.14)
include(CMakePrintHelpers)

project(Example5)


add_custom_target(example5 ALL DEPENDS main.py  sched.py)

dotdependency(example5 test ON graph.py)
pythondependency(example5 main.py graph.py)
