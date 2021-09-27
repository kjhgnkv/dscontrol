cmake_minimum_required(VERSION 3.12)
project(sniffer)

include(qt)

include(include.cmake)
add_library(${PROJECT_NAME} ${DLLTYPE}
            ${node}
            ${sniffer}
            )

target_include_directories(${PROJECT_NAME} PRIVATE
                           ${CMAKE_CURRENT_SOURCE_DIR}
                           ../../manager
                           )

target_link_directories(${PROJECT_NAME} BEFORE PUBLIC
                        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
                        )

target_link_libraries(${PROJECT_NAME} ${REQUIRED_LIBS_QUALIFIED})