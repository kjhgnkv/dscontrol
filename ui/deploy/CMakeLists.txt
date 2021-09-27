cmake_minimum_required(VERSION 3.12)
project(deploy)

include(qt)
include(macchina)

include(include.cmake)
add_library(${PROJECT_NAME} ${DLLTYPE}
            ${deploy}
            )

target_include_directories(${PROJECT_NAME} PRIVATE
                           ${CMAKE_CURRENT_SOURCE_DIR}
                           ${Macchina_INCLUDE_DIRS}
                           ../../network
                           ../../network/businessComponents/DSServer
                           ../../nodeeditor
                           ../platform
                           )

target_link_directories(${PROJECT_NAME} BEFORE PUBLIC
                        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
                        ${Macchina_LIBRARY_DIRS}
                        )

target_link_libraries(${PROJECT_NAME}
                      ${REQUIRED_LIBS_QUALIFIED}
                      ${Macchina_LIBRARIES}
                      )
