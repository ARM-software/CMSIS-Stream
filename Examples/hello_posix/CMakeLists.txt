cmake_minimum_required(VERSION 3.20)

project(hello_posix LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

get_filename_component(CMSIS_STREAM_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)

if(NOT TARGET posix_runtime)
    set(POSIX_RUNTIME_CONFIG_DIR "${CMAKE_CURRENT_LIST_DIR}/config" CACHE PATH "" FORCE)
    add_subdirectory("${CMSIS_STREAM_ROOT}/platform/posix_runtime" "${CMAKE_CURRENT_BINARY_DIR}/posix_runtime")
endif()

add_executable(hello_posix
    main.cpp
    config/stream_init.cpp
    hello_graph/hello_params.c
    hello_graph/scheduler_hello.cpp
)

target_include_directories(hello_posix PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/config"
    "${CMAKE_CURRENT_LIST_DIR}/hello_graph"
    "${CMSIS_STREAM_ROOT}/platform"
)

target_link_libraries(hello_posix PRIVATE posix_runtime)
