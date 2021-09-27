if (WIN32)
    set(PCAP_DIR "../../../../3rdparty/win10pcap")
    set(PCAP_INCLUDE "${PCAP_DIR}/include")
    set(PCAP_LIBRARY
        wpcap
        )

    include_directories(${PCAP_INCLUDE})
else ()
    set(PCAP_DIR "../../../../3rdparty/libpcap-1.9.1")
    set(PCAP_INCLUDE "${PCAP_DIR}")
    set(PCAP_LIBRARY
        pcap
        )
    add_subdirectory(${PCAP_DIR} build)
endif ()