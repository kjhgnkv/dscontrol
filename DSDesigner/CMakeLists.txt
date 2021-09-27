cmake_minimum_required(VERSION 3.12)
project(DSDesigner VERSION "0.1.1")

message(STATUS "Project: ${PROJECT_NAME} ${PROJECT_VERSION}")

include(qt)

include(include.cmake)
add_library(${PROJECT_NAME} ${DLLTYPE}
            ${qttoolbardialog}
            ${images}
            ${fontpanel}
            ${saveformastemplate}
            ${src}
            )

target_include_directories(${PROJECT_NAME} PRIVATE
                           ${CMAKE_CURRENT_SOURCE_DIR}
                           ../
                           )

message(${QtDesigner})

target_link_directories(${PROJECT_NAME} BEFORE PUBLIC
                        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
                        )

target_link_libraries(${PROJECT_NAME}
                      ${REQUIRED_LIBS_QUALIFIED}
                      manager
                      )