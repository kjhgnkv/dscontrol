#pragma once

#include <map>
#include <pcap.h>
#include <sstream>

#if __linux__
    #include <linux/icmp.h>
#endif

#include "../config.h"

namespace sockets
{
typedef std::pair<std::string, std::string> MapPairString;
typedef std::map<MapPairString::first_type, MapPairString::second_type> MapString;
// typedef std::vector<std::pair<std::string, std::string> > VecPairString;

std::ostream &hex_dump(std::stringstream &os, const char* buffer, std::size_t bufsize);

struct table_view
{
    std::string source;
    std::string destination;
    std::string protocol;
    std::string info;
    unsigned int size;
};

struct detail_view
{
    std::string preview;
    MapString properties;
    // VecPairString properties;
};

class base_socket
{
public:
    base_socket(u_char* args, const pcap_pkthdr* pkt_hdr, const u_char* packet);
    virtual ~base_socket();

    void print();

    std::string get_type();;

    table_view to_row();

    std::vector<detail_view> to_view();

    bool filter_size(int type, int length) const;

    std::string source_layer(int type);

    std::string destination_layer(int type);;

    std::string protocol_layer(int type);;

    const struct pcap_pkthdr* pkt_hdr;
    const u_char* packet;
    bpf_u_int32 cap_len;
    bpf_u_int32 len;

private:
    virtual std::string _get_type() = 0;
    virtual table_view _to_row() = 0;
    virtual std::vector<detail_view> _to_view() = 0;
    virtual std::string source_layer_(int type) = 0;
    virtual std::string destination_layer_(int type) = 0;
    virtual std::string protocol_layer_(int type) = 0;
};

base_socket* parse_packet(u_char* args, const struct pcap_pkthdr* pkt_hdr, const u_char* packet);

bool remove_package(base_socket* packet);

class ethernet : public sockets::base_socket
{
public:
    ethernet(u_char* args, const struct pcap_pkthdr* pkt_hdr, const u_char* packet);
    ~ethernet();

protected:
    std::string _get_type();
    sockets::table_view _to_row();
    std::vector<sockets::detail_view> _to_view();
    std::string source_layer_(int type);
    std::string destination_layer_(int type);
    std::string protocol_layer_(int type);

private:
    const struct ether_header* e_ptr;
};

class ip : public ethernet
{
public:
    ip(u_char* args, const struct pcap_pkthdr* pkt_hdr, const u_char* packet);
    ~ip();

protected:
    std::string _get_type();
    sockets::table_view _to_row();
    std::vector<sockets::detail_view> _to_view();
    std::string source_layer_(int type);
    std::string destination_layer_(int type);
    std::string protocol_layer_(int type);

private:
    const struct sniff_ip* ip_ptr;
};

class tcp : public ip
{
public:
    tcp(u_char* args, const struct pcap_pkthdr* pkt_hdr, const u_char* packet);
    ~tcp();

protected:
    std::string _get_type();
    sockets::table_view _to_row();
    std::vector<sockets::detail_view> _to_view();
    std::string source_layer_(int type);
    std::string destination_layer_(int type);
    std::string protocol_layer_(int type);

private:
    const struct sniff_tcp* tcp_ptr;
};

class udp : public ip
{
public:
    udp(u_char* args, const struct pcap_pkthdr* pkt_hdr, const u_char* packet);
    ~udp();

protected:
    std::string _get_type();
    sockets::table_view _to_row();
    std::vector<sockets::detail_view> _to_view();
    std::string source_layer_(int type);
    std::string destination_layer_(int type);
    std::string protocol_layer_(int type);

private:
    const struct udp_header* udp_ptr;
};

class arp : public ip
{
public:
    arp(u_char* args, const struct pcap_pkthdr* pkt_hdr, const u_char* packet);
    ~arp();

protected:
    std::string _get_type();
    sockets::table_view _to_row();
    std::vector<sockets::detail_view> _to_view();
    std::string source_layer_(int type);
    std::string destination_layer_(int type);
    std::string protocol_layer_(int type);

private:
    const struct arp_hdr* arp_ptr;
};

class icmp : public ip
{
public:
    icmp(u_char* args, const struct pcap_pkthdr* pkt_hdr, const u_char* packet);
    ~icmp();

protected:
    std::string _get_type();
    sockets::table_view _to_row();
    std::vector<sockets::detail_view> _to_view();
    std::string source_layer_(int type);
    std::string destination_layer_(int type);
    std::string protocol_layer_(int type);

private:
    const struct icmphdr* icmp_hdr;
};
}
