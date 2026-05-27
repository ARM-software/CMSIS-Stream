# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)


find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(cmsis_stream_hello)

target_sources(app PRIVATE
  src/main.cpp
  src/stream_init.cpp
  src/hello_graph/scheduler_hello.cpp
  src/hello_graph/hello_params.c
)

target_include_directories(app PRIVATE
  src
  src/config
  src/hello_graph
)
