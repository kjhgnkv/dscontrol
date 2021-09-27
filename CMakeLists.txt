cmake_minimum_required(VERSION 3.12)
project(DSControl3.0 VERSION "3.0.0")

message(STATUS "Project: ${PROJECT_NAME} ${PROJECT_VERSION}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake")

include(build)
include(create_installer)
include(qt)
include(execute_process)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
set(LIST_SUBDIRS
    DSControl
    DSDesigner
    manager
    network
    nodeeditor
    qt_plugin
    repository
    ui
    )

foreach (DIR ${LIST_SUBDIRS})
    add_subdirectory(${DIR})
endforeach ()
