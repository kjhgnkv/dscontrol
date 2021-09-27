cmake_minimum_required(VERSION 3.10)
project(SubModuleSniffer)
set(name SubModuleSniffer)

set(CMAKE_CXX_STANDARD 98)

if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions(-Wno-sign-compare)
endif (CMAKE_COMPILER_IS_GNUCC)

include(pcap.cmake)
include(../../build.cmake)

set(SnifferSRC
    core/core.h
    core/sniffer.cpp
    core/sockets.h
    core/structs.h

    core/core/handlers.cpp
    core/core/init.cpp

    core/socket/arp.cpp
    core/socket/ethernet.cpp
    core/socket/general.cpp
    core/socket/icmp.cpp
    core/socket/ip.cpp
    core/socket/tcp.cpp
    core/socket/udp.cpp

    IO/IO.cpp
    IO/IO.h

    utils/cli_parser.cpp
    utils/files.cpp
    utils/print_payload.cpp
    utils/utils.cpp
    utils/utils.h
    )

add_library(${name}
            ${SnifferSRC}
            )

add_executable(TestExampleDSSniffer
               tmain.cpp
               ${SnifferSRC}
               )

target_link_libraries(TestExampleDSSniffer PRIVATE
                      -lpcap
                      )

target_link_libraries(${name} PRIVATE
                      -lpcap
                      )

#add_bundle(${name})
