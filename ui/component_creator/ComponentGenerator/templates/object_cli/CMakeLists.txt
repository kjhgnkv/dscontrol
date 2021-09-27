cmake_minimum_required(VERSION 3.13)
set(name %name%BusinessComponent)
project(${name})

if(CMAKE_COMPILER_IS_GNUCC)
    add_definitions(-Wno-sign-compare)
endif(CMAKE_COMPILER_IS_GNUCC)

if(NOT CMAKE_DEBUG_POSTFIX)
    set(CMAKE_DEBUG_POSTFIX d)
endif()

include(macchina.cmake)

set(Macchina_LIBRARIES_RELEASE PocoFoundation PocoUtil PocoOSP)
set(Macchina_LIBRARIES_DEBUG PocoFoundationd PocoUtild PocoOSPd)

add_library(${name} MODULE
    src/BundleObject.cpp
    )
set_target_properties(${name} PROPERTIES
    OUTPUT_NAME %library-name%
    PREFIX ""
    )
target_include_directories(${name} PRIVATE
    include
    "%include-directory%"
    "${Macchina_INCLUDE_DIRS}"
    )
target_link_directories(${name} PRIVATE
    "${Macchina_LIBRARY_DIRS}"
    )
target_link_libraries(${name} PRIVATE
    optimized "${Macchina_LIBRARIES_RELEASE}"
    debug "${Macchina_LIBRARIES_DEBUG}"
    )


add_custom_target(${name}Directory ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/components"
    VERBATIM
    )

macro(add_bundle_content name)
    add_custom_target(${name}BundleContent ALL
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/Bundle.bndlspec" "."
        COMMAND ${CMAKE_COMMAND} -E make_directory "lib"
        COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${name}>" "lib/"
        DEPENDS ${name}
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        VERBATIM
        )
endmacro()

if(WIN32)
    set(Macchina_BUNDLE_UTIL_ARG "/output-dir=${CMAKE_BINARY_DIR}\\components")
else()
    set(Macchina_BUNDLE_UTIL_ARG "--output-dir=${CMAKE_BINARY_DIR}/components")
endif()

macro(add_bundle_file name namedep)
    add_custom_target(${name}Bundle ALL
        COMMAND ${Macchina_BUNDLE_UTIL} ${Macchina_BUNDLE_UTIL_ARG} "Bundle.bndlspec"
        DEPENDS ${name}Directory ${namedep}
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        VERBATIM
        )
endmacro()

macro(add_bundle name)
    add_bundle_content(${name})
    add_bundle_file(${name} ${name}BundleContent)
endmacro()

add_bundle(${name})

include(build.cmake)

