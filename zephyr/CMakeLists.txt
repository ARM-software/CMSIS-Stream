if(CONFIG_CMSIS_DSP)
zephyr_library()

zephyr_library_sources(
    ${CMAKE_CURRENT_SOURCE_DIR}/../platform/zephyr_runtime/stream_event_queue.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../platform/zephyr_runtime/stream_runtime_init.cpp
)

zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../platform/zephyr_runtime
  ${CMAKE_CURRENT_SOURCE_DIR}/../platform)
endif()