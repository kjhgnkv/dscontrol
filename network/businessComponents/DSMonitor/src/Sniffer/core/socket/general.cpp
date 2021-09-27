#include "../structs.h"
#include "../../utils/utils.h"
#include "../sockets.h"
#include "../core.h"
#include "../../IO/IO.h"

sockets::base_socket* parse_transport(u_char* args, const struct pcap_pkthdr* pkt_hdr, const u_char* packet,
                                      const std::string &type)
{
    switch (utils::Transport::to_type(type))
    {
        case utils::Transport::TCP:
            return new sockets::tcp(args, pkt_hdr, packet);
        case utils::Transport::UDP:
            return new sockets::udp(args, pkt_hdr, packet);
        case utils::Transport::ICMP:
            return new sockets::icmp(args, pkt_hdr, packet);
        case utils::Transport::NONE:
        default:
            return NULL;
    }
}

sockets::base_socket* parse_network(u_char* args, const struct pcap_pkthdr* pkt_hdr, const u_char* packet,
                                    const std::string &type)
{
    switch (utils::Network::to_type(type))
    {
        case utils::Network::IP:
        {
            sockets::base_socket* sockIP = new sockets::ip(args, pkt_hdr, packet);
            sockets::base_socket* sockTransportIP = parse_transport(args, pkt_hdr, packet, sockIP->get_type());

            if (sockTransportIP != NULL)
            {
                delete sockIP;
                return sockTransportIP;
            }

            delete sockTransportIP;
            return sockIP;
        }
        case utils::Network::ARP:
        {
            sockets::base_socket* sockARP = new sockets::arp(args, pkt_hdr, packet);
            sockets::base_socket* sockTransportARP = parse_transport(args, pkt_hdr, packet, sockARP->get_type());

            if (sockTransportARP != NULL)
            {
                delete sockARP;
                return sockTransportARP;
            }

            delete sockTransportARP;
            return sockARP;
        }
        case utils::Network::NONE:
        default:
            return NULL;
    }
}

sockets::base_socket* sockets::parse_packet(u_char* args, const struct pcap_pkthdr* pkt_hdr, const u_char* packet)
{
    if (pkt_hdr->caplen < ETHER_HDRLEN)
    {
        IO::print_err("Packet length less than ethernet header length");
        return NULL;
    }

    ethernet* eth_sock = new ethernet(args, pkt_hdr, packet);
    base_socket* sock = parse_network(args, pkt_hdr, packet, eth_sock->get_type());

    if (sock != NULL)
    {
        delete eth_sock;
        return sock;
    }

    delete sock;
    return eth_sock;
}

bool sockets::remove_package(sockets::base_socket* packet)
{
    if (packet != NULL)
    {
        delete packet;
        return false;
    }

    return true;
}
