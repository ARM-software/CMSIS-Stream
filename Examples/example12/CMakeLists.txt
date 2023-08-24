cmake_minimum_required (VERSION 3.14)
include(CMakePrintHelpers)

project(Example12)


add_executable(example12 static/static_scheduler.cpp 
    dynamic/dynamic_scheduler.cpp 
    main.cpp)

dotdependency(example12 static ON static_refcount.py)
dotdependency(example12 dynamic ON dynamic_refcount.py)

cppdependency(example12 static/static_scheduler.cpp static_refcount.py)
cppdependency(example12 dynamic/dynamic_scheduler.cpp dynamic_refcount.py)
add_sdf_dir(example12)

target_include_directories(example12 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(example12 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/static)
target_include_directories(example12 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dynamic)
