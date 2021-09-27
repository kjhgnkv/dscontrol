cmake_minimum_required(VERSION 3.12)
project(monitor)

include(qt)
include(macchina)

include(include.cmake)
add_library(${PROJECT_NAME} ${DLLTYPE}
            ${component}
            ${node}
            ${push}
            ${computer}
            ${application}
            ${threads}
            ${monitor}
            ${dsmonitor}
            )

target_include_directories(${PROJECT_NAME} PRIVATE
                           ${CMAKE_CURRENT_SOURCE_DIR}
                           ${Macchina_INCLUDE_DIRS}
                           ../../manager
                           ../../network
                           )

target_link_directories(${PROJECT_NAME} BEFORE PUBLIC
                        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
                        ${Macchina_LIBRARY_DIRS}
                        )

target_link_libraries(${PROJECT_NAME}
                      ${REQUIRED_LIBS_QUALIFIED}
                      ${Macchina_LIBRARIES}
                      network
                      )

add_executable(${PROJECT_NAME}TestExe
               tmain.cpp

               ${component}
               ${node}
               ${push}

               ${computer}
               ${application}
               ${threads}

               ${monitor}
               ${dsmonitor}

               ../../manager/models/AbstractItemModelMonitor.cpp
               ../../manager/models/AbstractItemModelMonitor.hpp
               ../../manager/models/AbstractNodeItemMonitor.cpp
               ../../manager/models/AbstractNodeItemMonitor.hpp

               testJson.hpp
               )

target_include_directories(${PROJECT_NAME}TestExe PRIVATE
                           ${CMAKE_CURRENT_SOURCE_DIR}
                           ${Macchina_INCLUDE_DIRS}
                           ../../manager
                           )

target_link_directories(${PROJECT_NAME}TestExe BEFORE PUBLIC
                        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
                        ${Macchina_LIBRARY_DIRS}
                        )

target_link_libraries(${PROJECT_NAME}TestExe
                      ${REQUIRED_LIBS_QUALIFIED}
                      ${Macchina_LIBRARIES}
                      network
                      )
