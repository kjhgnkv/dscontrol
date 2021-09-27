#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "../sockets.h"
#include "../core.h"
#include "../../IO/IO.h"

sockets::base_socket::base_socket(u_char* args, const struct pcap_pkthdr* pkt_hdr, const u_char* packet)
: packet(packet)
, pkt_hdr(pkt_hdr)
, cap_len(pkt_hdr->caplen)
, len(pkt_hdr->len)
{
}

sockets::base_socket::~base_socket()
{
    pkt_hdr = NULL;
    packet = NULL;
}

std::vector<std::string> core::device_list()
{
    std::vector<std::string> devices;

    pcap_if_t* alldevs;
    pcap_if_t* d;

    char err_buf[PCAP_ERRBUF_SIZE];

    if (pcap_findalldevs(&alldevs, err_buf) == -1)
    {
        fprintf(stderr, "Error in pcap_find all devs_ex: %s\n", err_buf);
        exit(EXIT_FAILURE);
    }

    for (d = alldevs; d != NULL; d = d->next)
    {
        devices.push_back(d->name);
    }

    return devices;
}

void core::console_device_list(const std::vector<std::string> &lists)
{
    std::vector<std::string>::const_iterator it, end;

    for (it = lists.begin(), end = lists.end(); it != end; it++)
    {
        IO::println(*it);
    }
}

void sockets::base_socket::print()
{
    IO::print_socket(to_view());
}

bool sockets::base_socket::filter_size(int type, int length) const
{
    switch (type - 1)
    {
        case LengthType::Great:
            return len > length;
        case LengthType::Less:
            return len < length;
        case LengthType::Eaquals:
            return len == length;
    }
    return false;
}

std::vector<sockets::detail_view> sockets::base_socket::to_view()
{
    return _to_view();
}

sockets::table_view sockets::base_socket::to_row()
{
    return _to_row();
}

std::string sockets::base_socket::get_type()
{
    return _get_type();
}

std::string sockets::base_socket::source_layer(int type)
{
    return source_layer_(type);
}

std::string sockets::base_socket::destination_layer(int type)
{
    return destination_layer_(type);
}

std::string sockets::base_socket::protocol_layer(int type)
{
    return protocol_layer_(type);
}
