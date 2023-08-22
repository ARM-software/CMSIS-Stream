cmake_minimum_required (VERSION 3.14)
include(CMakePrintHelpers)

project(Graphviz)


add_custom_target(graphviz ALL)

dotdependency(graphviz test graph.py)
