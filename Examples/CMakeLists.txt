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

    if (${PRESCHEDULE})
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
if(MSVC)
    set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "/INCREMENTAL:NO")
endif()

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
add_subdirectory(example12 bin_example12)

add_subdirectory(simple bin_simple)
add_subdirectory(simpledsp bin_simpledsp)
add_subdirectory(cyclo bin_cyclo)
add_subdirectory(functionexample bin_functionexample)
add_subdirectory(callback bin_callback)


# Python examples
add_subdirectory(graphvizexample bin_graphvizexample)
add_subdirectory(yaml bin_yaml)
