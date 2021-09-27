cmake_minimum_required(VERSION 3.12)
project(DS)

set(LIST_SUBDIRS
    component_creator
    deploy
    monitor
    platform
    sniffer
    update)

foreach (DIR ${LIST_SUBDIRS})
    add_subdirectory(${DIR})
endforeach ()

include(GenerateExportHeader)

add_library(${PROJECT_NAME} ${DLLTYPE}
            ../common/globals.h
            app_icon.rc
            resources.qrc
            )

generate_export_header(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE
                           ${CMAKE_CURRENT_SOURCE_DIR}
                           )

target_link_directories(${PROJECT_NAME} BEFORE PUBLIC
                        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
                        )

target_link_libraries(${PROJECT_NAME}
                      ${REQUIRED_LIBS_QUALIFIED}

                      nodeeditor
                      nodeeditor::nodes
                      DSDesigner
                      manager
                      network
                      # quazip5
                      repository

                      component_creator
                      deploy
                      monitor
                      platform
                      sniffer
                      update
                      )