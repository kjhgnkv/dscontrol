cmake_minimum_required(VERSION 3.12)
project(DSControl VERSION "0.1.1")

message(STATUS "Project: ${PROJECT_NAME} ${PROJECT_VERSION}")

add_executable(${PROJECT_NAME}
               main.cpp
               ../common/globals.h
               )

target_include_directories(${PROJECT_NAME} PRIVATE
                           ${CMAKE_CURRENT_SOURCE_DIR}
                           ../nodeeditor
                           ../network
                           )

target_link_directories(${PROJECT_NAME} PUBLIC
                        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
                        )

target_link_libraries(${PROJECT_NAME}
                      ${REQUIRED_LIBS_QUALIFIED}
                      monitor
                      DS
                      )
