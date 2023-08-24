cmake_minimum_required (VERSION 3.14)
include(CMakePrintHelpers)

project(Graphviz)


add_custom_target(graphviz ALL DEPENDS test.dot)

dotdependency(graphviz test ON graph.py)
