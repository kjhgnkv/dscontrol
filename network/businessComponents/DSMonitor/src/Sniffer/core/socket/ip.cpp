#include "../structs.h"
#include "../sockets.h"
#include "../../utils/utils.h"

sockets::ip::ip(u_char* args, const struct pcap_pkthdr* pkt_hdr, const u_char* packet)
: ethernet(args, pkt_hdr, packet)
, ip_ptr((struct sniff_ip*)(packet + SIZE_ETHERNET))
{
}

std::string sockets::ip::_get_type()
{
    if (ip_ptr == NULL)
    {
        return "?";
    }

    return utils::IPPROTO::to_string(ip_ptr->ip_p);
}

std::string get_flag(u_short ip_off)
{
    if (ip_off & IP_RF)
    {
        return "0x8000, reserved fragment flag";
    }
    if (ip_off & IP_DF)
    {
        return "0x4000, don't fragment flag";
    }
    if (ip_off & IP_MF)
    {
        return "0x2000, more fragments flag";
    }
    if (ip_off & IP_OFFMASK)
    {
        return "0x1fff, mask for fragmenting bits";
    }
    return "Unknown";
}

std::vector<sockets::detail_view> sockets::ip::_to_view()
{
    std::vector<sockets::detail_view> data = ethernet::_to_view();

    sockets::detail_view view;

    view.preview = "Internet Protocol Version 4";
    // view.properties.push_back(utils::make_pair("Source", inet_ntoa(ip_ptr->ip_src)));
    // view.properties.push_back(utils::make_pair("Destination", inet_ntoa(ip_ptr->ip_dst)));
    // view.properties.push_back(utils::make_pair("Header size", utils::to_string(ip_ptr->ip_vhl >> 2)));
    // view.properties.push_back(utils::make_pair("Total length", utils::to_string(ip_ptr->ip_len)));
    // view.properties.push_back(utils::make_pair("Flag", get_flag(ip_ptr->ip_off)));
    // view.properties.push_back(utils::make_pair("Time to life", utils::to_string(ip_ptr->ip_ttl)));
    // view.properties.push_back(utils::make_pair("Checksum", utils::to_string(ip_ptr->ip_sum)));
    // view.properties.push_back(utils::make_pair("Type", get_type()));
    view.properties.insert(MapPairString("Source", inet_ntoa(ip_ptr->ip_src)));
    view.properties.insert(MapPairString("Destination", inet_ntoa(ip_ptr->ip_dst)));
    view.properties.insert(MapPairString("Header size", utils::to_string(ip_ptr->ip_vhl >> 2)));
    view.properties.insert(MapPairString("Total length", utils::to_string(ip_ptr->ip_len)));
    view.properties.insert(MapPairString("Flag", get_flag(ip_ptr->ip_off)));
    view.properties.insert(MapPairString("Time to life", utils::to_string(ip_ptr->ip_ttl)));
    view.properties.insert(MapPairString("Checksum", utils::to_string(ip_ptr->ip_sum)));
    view.properties.insert(MapPairString("Type", get_type()));

    data.push_back(view);

    return data;
}

sockets::table_view sockets::ip::_to_row()
{
    struct sockets::table_view view;
    view.source = inet_ntoa(ip_ptr->ip_src);
    view.destination = inet_ntoa(ip_ptr->ip_dst);
    view.protocol = _get_type();
    view.size = pkt_hdr->len;
    std::stringstream ss;
    hex_dump(ss, sizeof(ether_header) + sizeof(sniff_ip) + (const char*)packet, pkt_hdr->len);

    view.info = ss.str();

    return view;
}

std::string sockets::ip::source_layer_(int type)
{
    switch (type - 1)
    {
        case LayerType::Physic:
            return ethernet::source_layer_(type);
        case LayerType::Network:
            return inet_ntoa(ip_ptr->ip_src);
        default:
            return "";
    }
}

std::string sockets::ip::destination_layer_(int type)
{
    switch (type - 1)
    {
        case LayerType::Physic:
            return ethernet::destination_layer_(type);
        case LayerType::Network:
            return inet_ntoa(ip_ptr->ip_dst);
        default:
            return "";
    }
}

std::string sockets::ip::protocol_layer_(int type)
{
    switch (type - 1)
    {
        case LayerType::Physic:
            return "Ethernet";
        case LayerType::Network:
            return "IPv4";
        default:
            return "";
    }
}

sockets::ip::~ip()
{
    ip_ptr = NULL;
}
