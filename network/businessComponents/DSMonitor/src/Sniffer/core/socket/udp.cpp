#include <sstream>

#include "../structs.h"
#include "../core.h"
#include "../sockets.h"
#include "../../utils/utils.h"

sockets::udp::udp(u_char* args, const struct pcap_pkthdr* pkt_hdr, const u_char* packet)
: ip(args, pkt_hdr, packet)
, udp_ptr((struct udp_header*)(packet + SIZE_ETHERNET + sizeof(sniff_ip)))
{
}

std::string sockets::udp::_get_type()
{
    return "UDP";
}

std::vector<sockets::detail_view> sockets::udp::_to_view()
{
    std::vector<sockets::detail_view> data = ip::_to_view();

    sockets::detail_view view;

    view.preview = "User Datagram Protocol";
    // view.properties.push_back(utils::make_pair("Source", utils::to_string(ntohs(udp_ptr->udph_srcport))));
    // view.properties.push_back(utils::make_pair("Destination", utils::to_string(ntohs(udp_ptr->udph_destport))));
    // view.properties.push_back(utils::make_pair("Length", utils::to_string(udp_ptr->udph_chksum)));
    // view.properties.push_back(utils::make_pair("Checksum", utils::to_string(udp_ptr->udph_len)));
    view.properties.insert(MapPairString("Source", utils::to_string(ntohs(udp_ptr->udph_srcport))));
    view.properties.insert(MapPairString("Destination", utils::to_string(ntohs(udp_ptr->udph_destport))));
    view.properties.insert(MapPairString("Length", utils::to_string(udp_ptr->udph_chksum)));
    view.properties.insert(MapPairString("Checksum", utils::to_string(udp_ptr->udph_len)));

    data.push_back(view);

    return data;
}

sockets::table_view sockets::udp::_to_row()
{
    struct sockets::table_view view;

    view.source = utils::to_string(ntohs(udp_ptr->udph_srcport));
    view.destination = utils::to_string(ntohs(udp_ptr->udph_destport));

    view.protocol = _get_type();
    view.size = pkt_hdr->len;

    std::stringstream ss;
    hex_dump(ss, sizeof(ether_header) + sizeof(sniff_ip) + sizeof(udp_header) + (const char*)packet, pkt_hdr->len);

    view.info = ss.str();

    return view;
}

std::string sockets::udp::source_layer_(int type)
{
    switch (type - 1)
    {
        case LayerType::Physic:
        case LayerType::Network:
            return sockets::ip::source_layer_(type);
        case LayerType::Transport:
            return utils::to_string(ntohs(udp_ptr->udph_srcport));
        default:
            return "";
    }
}

std::string sockets::udp::destination_layer_(int type)
{
    switch (type - 1)
    {
        case LayerType::Physic:
        case LayerType::Network:
            return sockets::ip::destination_layer_(type);
        case LayerType::Transport:
            return utils::to_string(ntohs(udp_ptr->udph_destport));
        default:
            return "";
    }
}

std::string sockets::udp::protocol_layer_(int type)
{
    switch (type - 1)
    {
        case LayerType::Physic:
        case LayerType::Network:
            return sockets::ip::protocol_layer_(type);
        case LayerType::Transport:
            return "UDP";
        default:
            return "";
    }
}

sockets::udp::~udp()
{
    udp_ptr = NULL;
}
