cmake_minimum_required(VERSION 3.13)
set(name %name%GraphicalBusinessComponent)
project(${name})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

find_package(Qt5UiPlugin COMPONENTS Private REQUIRED)
find_package(Qt5UiTools COMPONENTS Private REQUIRED)
find_package(Qt5Widgets COMPONENTS Private REQUIRED)
find_package(Qt5Core COMPONENTS Private REQUIRED)

add_compile_definitions(QFORMINTERNAL_NAMESPACE QT_DESIGNER_STATIC)

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
    src/QtWidget.cpp
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
    Qt5::UiPlugin
    Qt5::UiTools
    Qt5::Widgets
    Qt5::Core
    )
target_compile_features(${name} PRIVATE cxx_std_17)

set(plugin_name ${name}_QtDesignerPlugin)
add_library(${plugin_name} MODULE)
set_target_properties(${plugin_name} PROPERTIES
    OUTPUT_NAME %library-name%.qdplugin
    COMPILE_FLAGS "-I${CMAKE_CURRENT_BINARY_DIR}"
    PREFIX "")
target_sources(${plugin_name} PRIVATE
    src/QtDesignerPlugin.cpp
    )
target_include_directories(${plugin_name} PRIVATE
    include
    )
target_link_libraries(${plugin_name} PRIVATE
    Qt5::UiPlugin
    Qt5::UiTools
    Qt5::Widgets
    Qt5::Core
    )
target_compile_features(${plugin_name} PRIVATE cxx_std_17)





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


add_bundle_content(${name})
add_custom_target(${name}BundleDependencies ALL
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${name}_QtDesignerPlugin>" "lib/"
    DEPENDS ${name}BundleContent
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    VERBATIM
    )
add_bundle_file(${name} ${name}BundleDependencies)

include(build.cmake)

