cmake_minimum_required(VERSION 3.12)
project(nodeeditor VERSION "0.1.1")

message(STATUS "Project: ${PROJECT_NAME} ${PROJECT_VERSION}")

include(qt)
include(GenerateExportHeader)

qt5_add_resources(SOURCES resources/nodeeditorResources.qrc)
include(include.cmake)
add_library(${PROJECT_NAME} ${DLLTYPE}
            ${arrows}
            ${common}
            ${factories}
            ${graphicsComposites}
            ${frame}
            ${text}
            ${universalGraphicsObject}
            ${lists}
            ${models}
            ${nodes}
            ${scenes}
            ${singletones}
            ${src}
            ${styles}
            ${utils}
            ${views}
            ${widgets}
            ${SOURCES}
            )

add_library(${PROJECT_NAME}::nodes ALIAS ${PROJECT_NAME})

#set(CMAKE_CXX_VISIBILITY_PRESET hidden)
#set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

generate_export_header(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE
                           ${CMAKE_CURRENT_SOURCE_DIR}
                           ../
                           ../repository
                           )

target_link_directories(${PROJECT_NAME} BEFORE PUBLIC
                        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
                        )

target_link_libraries(${PROJECT_NAME}
                      ${REQUIRED_LIBS_QUALIFIED}
                      repository
                      )