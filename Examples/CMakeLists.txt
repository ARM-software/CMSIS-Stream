cmake_minimum_required (VERSION 3.21)
include(CMakePrintHelpers)

set(Python_FIND_REGISTRY "LAST")

find_package (Python COMPONENTS Interpreter)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

function(dotdependency TARGET DOTNAME PRESCHEDULE SCRIPT)
    #set (DEPS ${ARGN})
    #list(TRANSFORM DEPS PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)
    #list(JOIN DEPS "\n" OUT)
    #list(GET DEPS 0 SCRIPT)
    #message(${SCRIPT})

if(DEFINED ENV{PYTHONPATH} AND NOT "$ENV{PYTHONPATH}" STREQUAL "")
        set(MY_PYTHONPATH "$ENV{PYTHONPATH}${CMAKE_HOST_PATH_SEPARATOR}${CMAKE_SOURCE_DIR}")
 else()
        set(MY_PYTHONPATH "${CMAKE_SOURCE_DIR}")
 endif()
    
    
if (DOT)
    add_custom_command(TARGET ${TARGET} PRE_BUILD 
        BYPRODUCTS ${CMAKE_CURRENT_SOURCE_DIR}/${DOTNAME}.pdf
        COMMAND ${DOT} -Tpdf -o ${CMAKE_CURRENT_SOURCE_DIR}/${DOTNAME}.pdf ${CMAKE_CURRENT_SOURCE_DIR}/${DOTNAME}.dot
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        VERBATIM
        )

    add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${DOTNAME}.dot
        COMMAND ${CMAKE_COMMAND} -E env "PYTHONPATH=${MY_PYTHONPATH}" ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}
        VERBATIM
        )

    if (${PRESCHEDULE})
    add_custom_command(TARGET ${TARGET} PRE_BUILD 
        BYPRODUCTS ${CMAKE_CURRENT_SOURCE_DIR}/pre_schedule_${DOTNAME}.pdf
        COMMAND ${DOT} -Tpdf -o ${CMAKE_CURRENT_SOURCE_DIR}/pre_schedule_${DOTNAME}.pdf ${CMAKE_CURRENT_SOURCE_DIR}/pre_schedule_${DOTNAME}.dot
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        VERBATIM
        )

    

    add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/pre_schedule_${DOTNAME}.dot
        COMMAND ${CMAKE_COMMAND} -E env "PYTHONPATH=${MY_PYTHONPATH}" ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}
        VERBATIM
        )
endif()
endif()
endfunction()

function(cppdependency TARGET SRC SCRIPT)
 set(optional_outputs ${ARGN})   # may be empty
 set(outputs "${SRC}")          # always-present output
 list(APPEND outputs ${optional_outputs}) 
 list(TRANSFORM outputs PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")

 if(DEFINED ENV{PYTHONPATH} AND NOT "$ENV{PYTHONPATH}" STREQUAL "")
        set(MY_PYTHONPATH "$ENV{PYTHONPATH}${CMAKE_HOST_PATH_SEPARATOR}${CMAKE_SOURCE_DIR}")
 else()
        set(MY_PYTHONPATH "${CMAKE_SOURCE_DIR}")
 endif()

 add_custom_command(OUTPUT ${outputs}
        COMMAND ${CMAKE_COMMAND} -E env "PYTHONPATH=${MY_PYTHONPATH}" ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}
        VERBATIM
        ) 
if(MSVC)
    set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "/INCREMENTAL:NO")
endif()

endfunction()


set(NODEDIR ${CMAKE_SOURCE_DIR}/Examples/nodes/cpp)


set(DSP ${CMSISDSP})

function(add_sdf_dir TARGET)
    target_include_directories(${TARGET} PRIVATE ${NODEDIR})
    target_include_directories(${TARGET} PRIVATE ${DSP}/Include)
endfunction()

project(Examples)

# Add DSP folder to module path
list(APPEND CMAKE_MODULE_PATH ${DSP})

add_subdirectory(common)
add_subdirectory(${DSP}/Source bin_dsp)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../platform/posix_runtime bin_runtime)
add_subdirectory(nodes/cpp)

add_subdirectory(events)

add_subdirectory(example1)
add_subdirectory(example2)
add_subdirectory(example3)
add_subdirectory(example6)
add_subdirectory(example8)
add_subdirectory(example9)
add_subdirectory(example10)
add_subdirectory(example11)
add_subdirectory(example12)
#
add_subdirectory(simple)
add_subdirectory(simpledsp)
add_subdirectory(cyclo)
add_subdirectory(functionexample)
add_subdirectory(callback)
add_subdirectory(memory)
#
#
## Python examples
add_subdirectory(graphvizexample)
add_subdirectory(yaml)

