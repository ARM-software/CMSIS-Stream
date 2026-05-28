cmake_minimum_required(VERSION 3.20)

project(CMSISStreamPosixRuntime VERSION 0.0.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(POSIX_RUNTIME_CONFIG_DIR "" CACHE PATH
    "Directory containing the client application's copied POSIX runtime configuration"
)

if(POSIX_RUNTIME_CONFIG_DIR)
    set(POSIX_RUNTIME_BUILD_CONFIG_DIR ${POSIX_RUNTIME_CONFIG_DIR})
else()
    set(POSIX_RUNTIME_BUILD_CONFIG_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated_config)
    file(MAKE_DIRECTORY ${POSIX_RUNTIME_BUILD_CONFIG_DIR})
    file(WRITE ${POSIX_RUNTIME_BUILD_CONFIG_DIR}/stream_runtime_config.hpp
        "#pragma once\n"
    )
endif()

add_library(posix_runtime STATIC
    stream_event_queue.cpp
    stream_runtime_init.cpp
)

add_library(cmsis_stream::posix_runtime ALIAS posix_runtime)

set_target_properties(posix_runtime PROPERTIES
    OUTPUT_NAME cmsis_stream_runtime
)

target_include_directories(posix_runtime PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/CMSIS-Stream/platform/posix_runtime>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/CMSIS-Stream/platform>
)

target_include_directories(posix_runtime PRIVATE
    $<BUILD_INTERFACE:${POSIX_RUNTIME_BUILD_CONFIG_DIR}>
)

target_compile_features(posix_runtime PUBLIC cxx_std_17)

if(UNIX AND NOT APPLE)
    find_package(Threads REQUIRED)
    target_link_libraries(posix_runtime PUBLIC Threads::Threads)
endif()

set(POSIX_RUNTIME_CMAKE_INSTALL_DIR
    ${CMAKE_INSTALL_LIBDIR}/cmake/cmsis-stream
)

install(TARGETS posix_runtime
    EXPORT posix_runtimeTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES
    stream_event_queue.hpp
    stream_platform_config.hpp
    stream_rtos_events.h
    stream_runtime_init.hpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/CMSIS-Stream/platform/posix_runtime
)

install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../cg_enums.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../cg_pack.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../EventDisplay.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../EventQueue.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../GenericNodes.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../StreamNode.hpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/CMSIS-Stream/platform
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../core_nodes
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/CMSIS-Stream/platform
    FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "*.h"
)

install(DIRECTORY config/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/CMSIS-Stream/posix_runtime/config
)

install(EXPORT posix_runtimeTargets
    FILE posix_runtimeTargets.cmake
    NAMESPACE cmsis_stream::
    DESTINATION ${POSIX_RUNTIME_CMAKE_INSTALL_DIR}
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmsis-streamConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmsis-streamConfig.cmake
    INSTALL_DESTINATION ${POSIX_RUNTIME_CMAKE_INSTALL_DIR}
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/cmsis-streamConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/cmsis-streamConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/cmsis-streamConfigVersion.cmake
    DESTINATION ${POSIX_RUNTIME_CMAKE_INSTALL_DIR}
)
