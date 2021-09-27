cmake_minimum_required(VERSION 3.12)
project(manager VERSION "0.1.1")

message(STATUS "Project: ${PROJECT_NAME} ${PROJECT_VERSION}")

include(qt)
include(macchina)

qt5_add_resources(SOURCES resources/res_test.qrc)
include(include.cmake)
add_library(${PROJECT_NAME} ${DLLTYPE}
            ${models}
            ${manager}
            ${SOURCES}
            )
		
target_include_directories(${PROJECT_NAME} PRIVATE
                           ${CMAKE_CURRENT_SOURCE_DIR}
                           ${Macchina_INCLUDE_DIRS}
                           ../
                           ../ui
                           ../nodeeditor
                           ../repository

                           ../network
                           #../network/libraries/DSUtils

                           ../ui/monitor
                           )

target_link_directories(${PROJECT_NAME} BEFORE PUBLIC
                        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
                        ${Macchina_LIBRARY_DIRS}
                        )

target_link_libraries(${PROJECT_NAME}
                      ${REQUIRED_LIBS_QUALIFIED}
                      ${Macchina_LIBRARIES}

                      nodeeditor
                      repository
                      network
                      monitor
                      )