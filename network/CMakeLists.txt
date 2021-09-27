cmake_minimum_required(VERSION 3.12)

project(network VERSION "0.1.1")

message(STATUS "Project: ${PROJECT_NAME} ${PROJECT_VERSION}")

include(qt)
include(macchina)

include(include.cmake)
add_library(${PROJECT_NAME} ${DLLTYPE}
            ${network}
            ${monitor}
            DSServerComm.cpp
            DSServerComm.h
            DSServerCommImp.cpp
            DSServerCommImp.h
            DSServerCommImpBase.h
            NetDefine.h
            )

find_package(Threads)

target_include_directories(${PROJECT_NAME} PRIVATE
                           ${CMAKE_CURRENT_SOURCE_DIR}
                           ${Macchina_INCLUDE_DIRS}
                            ../fineftp/include
                           )

target_link_directories(${PROJECT_NAME} BEFORE PUBLIC
                        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
                        ${Macchina_LIBRARY_DIRS}
                       ../fineftp/lib
                        )

target_link_libraries(${PROJECT_NAME}
                      ${REQUIRED_LIBS_QUALIFIED}
                      ${CMAKE_THREAD_LIBS_INIT}
                      ${Macchina_LIBRARIES}
                      fineftp-server
                      )

