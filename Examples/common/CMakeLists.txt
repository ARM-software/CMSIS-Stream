cmake_minimum_required(VERSION 3.20)

add_library(appcommon INTERFACE)

target_include_directories(appcommon INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})