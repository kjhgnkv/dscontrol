cmake_minimum_required(VERSION 3.12)
project(platform)

include(macchina)
include(qt)

qt5_add_resources(SOURCES ../resources.qrc)

include(include.cmake)
add_library(${PROJECT_NAME} ${DLLTYPE}
            ${custom}
            ${delegates}
            ${event_filters}
            ${platform}
            ${SOURCES}
            )

target_include_directories(${PROJECT_NAME} PRIVATE
                           ${CMAKE_CURRENT_SOURCE_DIR}
                           ${Macchina_INCLUDE_DIRS}

                           ../
                           ../../
                           ../../nodeeditor
                           ../../repository
                           ../../manager

                           ../../network
                           ../../network/DSUtils
                           ../../network/DSUtils/MosquittoClient
                           ../../network/MqttCommunication
                           ../../network/MqttCommunication/CommunicationSource
                           ../../network/DSMiddleware
                           ../../network/DSMiddleware/DSMiddlewareSource
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
                      manager
                      DSDesigner

                      component_creator
                      deploy
                      monitor
                      sniffer
                      update
                      )
