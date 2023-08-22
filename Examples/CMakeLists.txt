cmake_minimum_required (VERSION 3.21)
include(CMakePrintHelpers)

set(Python_FIND_REGISTRY "LAST")

find_package (Python COMPONENTS Interpreter)

function(dotdependency TARGET DOTNAME SCRIPT)
    set(pre_schedule ON)
    set (extra_args ${ARGN})
    list(LENGTH extra_args extra_count)
    if (${extra_count} GREATER 0)
        list(GET extra_args 0 pre_schedule)
    endif()
    
if (DOT)
    add_custom_command(TARGET ${TARGET} PRE_BUILD 
        BYPRODUCTS ${CMAKE_CURRENT_SOURCE_DIR}/${DOTNAME}.pdf
        COMMAND ${DOT} -Tpdf -o ${CMAKE_CURRENT_SOURCE_DIR}/${DOTNAME}.pdf ${CMAKE_CURRENT_SOURCE_DIR}/${DOTNAME}.dot
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${DOTNAME}.dot
        VERBATIM
        )

    add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${DOTNAME}.dot
        COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}
        VERBATIM
        )

    if (${pre_schedule})
    add_custom_command(TARGET ${TARGET} PRE_BUILD 
        BYPRODUCTS ${CMAKE_CURRENT_SOURCE_DIR}/pre_schedule_${DOTNAME}.pdf
        COMMAND ${DOT} -Tpdf -o ${CMAKE_CURRENT_SOURCE_DIR}/pre_schedule_${DOTNAME}.pdf ${CMAKE_CURRENT_SOURCE_DIR}/pre_schedule_${DOTNAME}.dot
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/pre_schedule_${DOTNAME}.dot
        VERBATIM
        )

    

    add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/pre_schedule_${DOTNAME}.dot
        COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}
        VERBATIM
        )
endif()
endif()
endfunction()

function(cppdependency TARGET SRC SCRIPT)
 add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${SRC}
        COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}
        VERBATIM
        ) 
endfunction()

function(pythondependency TARGET SRC SCRIPT)
    add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${SRC}
        COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}
        VERBATIM
        )
endfunction()

set(CGDIR ${CMAKE_CURRENT_SOURCE_DIR}/../Include/cg)

set(DSP ${CMSISDSP})

function(add_sdf_dir TARGET)
    target_include_directories(${TARGET} PRIVATE ${CGDIR}/src)
    target_include_directories(${TARGET} PRIVATE ${CGDIR}/nodes/cpp)
    target_include_directories(${TARGET} PRIVATE ${DSP}/Include)
endfunction()

project(Examples)

# Add DSP folder to module path
list(APPEND CMAKE_MODULE_PATH ${DSP})

add_subdirectory(${DSP}/Source bin_dsp)


add_subdirectory(example1 bin_example1)
add_subdirectory(example2 bin_example2)
add_subdirectory(example3 bin_example3)
add_subdirectory(example6 bin_example6)
add_subdirectory(example8 bin_example8)
add_subdirectory(example9 bin_example9)
add_subdirectory(example10 bin_example10)
add_subdirectory(example11 bin_example11)

add_subdirectory(simple bin_simple)
add_subdirectory(simpledsp bin_simpledsp)
add_subdirectory(cyclo bin_cyclo)
add_subdirectory(functionexample bin_functionexample)

# Python examples
add_subdirectory(example4_python bin_example4_python)
add_subdirectory(example5_python bin_example5_python)
add_subdirectory(example7_python bin_example7_python)
add_subdirectory(graphvizexample bin_graphvizexample)
