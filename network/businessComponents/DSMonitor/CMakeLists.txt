cmake_minimum_required(VERSION 3.10)
project(ModuleDSMonitor)
set(name ModuleDSMonitor)

set(CMAKE_CXX_STANDARD 98)

set(Macchina_LIBRARIES_RELEASE PocoFoundation PocoUtil PocoOSP PocoNet PocoJSON PocoXML)
set(Macchina_LIBRARIES_DEBUG PocoFoundationd PocoUtild PocoOSPd PocoNetd PocoJSONd PocoXMLd)

set(BroadcasterWithMQTT ../../ServerComponents/components/transport/BroadcasterWithMQTT/include)
set(MQTT ../../ServerComponents/components/transport/MQTT/include)
set(DSServer ../../businessComponents/DSServer)
set(DSHost ../../DSHost/include)
set(DSUtils ../../libraries/DSUtils)

#add_executable(${name}
#               tmain.cpp
#               )

add_library(${name} MODULE)

set_target_properties(${name} PROPERTIES
                      OUTPUT_NAME ds.host.components.dsmonitor
                      PREFIX "")

set(MonitorSRC
    src/Monitor.cpp
    src/Monitor.h
    src/MonitorType.cpp
    src/MonitorType.h

    src/Network/Network.cpp
    src/Network/Network.hpp

    src/DSMonitorHelp.hpp
    src/MonitorData.hpp
    src/ProcessData.hpp

    src/ExportJson/ExportJSON.cpp
    src/ExportJson/ExportJSON.h

    src/ParserXML/Parser.cpp
    src/ParserXML/ParserData.hpp
    src/ParserXML/Parser.h
    )

# Add Monitor SRC
if (WIN32)
    message(STATUS "Building for Windows")
    list(APPEND MonitorSRC
         src/OS/Windows/WindowsMonitor.cpp
         src/OS/Windows/WindowsMonitor.h

         src/OS/Windows/TextEncoder.cpp
         src/OS/Windows/TextEncoder.h
         )
elseif (UNIX)
    message(STATUS "Building for Linux")
    list(APPEND MonitorSRC
         src/OS/Linux/LinuxMonitor.cpp
         src/OS/Linux/LinuxMonitor.h

         src/OS/Linux/linux/osrelease.h
         src/OS/Linux/linux/linuxhelp.hpp
         src/OS/Linux/linux/cache.h
         src/OS/Linux/linux/cpu.h
         src/OS/Linux/linux/memory.h
         src/OS/Linux/linux/mount.h
         src/OS/Linux/linux/stat.h
         src/OS/Linux/linux/status.h
         )
else ()
    message(FATAL_ERROR "UNKHNOW SYSTEM")
endif ()

target_sources(${name} PRIVATE
               ${MonitorSRC}
                Controller.cpp
                Controller.h
                DSMonitorObject.cpp
                Handler/MessageHandler.cpp
                Handler/MessageHandler.h
                Logger.cpp
                Logger.h
               )

target_include_directories(${name} PRIVATE
                           ${BroadcasterWithMQTT}
                           ${MQTT}
                           ${DSServer}
                           ${DSHost}
                           ${DSUtils}
                           ${Macchina_INCLUDE_DIRS}
                           )

target_link_directories(${name} PRIVATE
                        ${Macchina_LIBRARY_DIRS}
                        ${LIBRARY_OUTPUT_DIRECTORY}
                        )

if (WIN32)
    add_library(DSUtils STATIC IMPORTED)
    set(DSUtilsPath
        ${CMAKE_SOURCE_DIR}/../libraries/build/lib/DSUtils.lib
        )
    set_property(TARGET DSUtils PROPERTY IMPORTED_LOCATION
                 ${DSUtilsPath}
                 )
else ()
    add_library(DSUtils SHARED IMPORTED)
    set(DSUtilsPath
        ${CMAKE_SOURCE_DIR}/../libraries/build/lib/libDSUtils.so
        )
    set_property(TARGET DSUtils PROPERTY IMPORTED_LOCATION
                 ${DSUtilsPath})
endif ()

target_link_libraries(${name} PRIVATE
                      ${Macchina_LIBRARIES}
                      DSUtils
                      )

add_bundle(${name})
