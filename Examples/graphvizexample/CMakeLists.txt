cmake_minimum_required (VERSION 3.14)
include(CMakePrintHelpers)

project(Graphviz)


add_custom_target(graphviz ALL DEPENDS test.dot)

dotdependency(graphviz test ON graph.py)
set(outputs pre_schedule_test.dot test.dot)
list(TRANSFORM outputs PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")

add_custom_command(
    OUTPUT ${outputs}
    COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/graph.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/graph.py
    VERBATIM
)