#if __linux__

    #include <netinet/ether.h>

#endif

#include "../../utils/utils.h"
#include "../core.h"
#include "../sockets.h"

sockets::ethernet::ethernet(u_char* args, const struct pcap_pkthdr* pkt_hdr, const u_char* packet)
: base_socket(args, pkt_hdr, packet)
, e_ptr((struct ether_header*)packet)
{
}

sockets::table_view sockets::ethernet::_to_row()
{
    struct sockets::table_view view;

    view.source = ether_ntoa((struct ether_addr*)e_ptr->ether_shost);
    view.destination = ether_ntoa((struct ether_addr*)e_ptr->ether_dhost);
    view.protocol = _get_type();
    view.size = pkt_hdr->len;

    std::stringstream ss;
    hex_dump(ss, sizeof(ether_header) + (const char*)packet, pkt_hdr->len);

    view.info = ss.str();

    return view;
}

std::string sockets::ethernet::_get_type()
{
    if (e_ptr == NULL)
    {
        return "?";
    }

    uint16_t type = ntohs(e_ptr->ether_type);
    return utils::Ether::to_string(type);
}

std::vector<sockets::detail_view> sockets::ethernet::_to_view()
{
    sockets::detail_view view = {};

    view.preview = "Ethernet";
    // view.properties.push_back(utils::make_pair("Source", ether_ntoa((struct ether_addr*)e_ptr->ether_shost)));
    // view.properties.push_back(utils::make_pair("Destination", ether_ntoa((struct ether_addr*)e_ptr->ether_dhost)));
    // view.properties.push_back(utils::make_pair("Type", get_type()));
    view.properties.insert(MapPairString("Source", ether_ntoa((struct ether_addr*)e_ptr->ether_shost)));
    view.properties.insert(MapPairString("Destination", ether_ntoa((struct ether_addr*)e_ptr->ether_dhost)));
    view.properties.insert(MapPairString("Type", get_type()));

    std::vector<sockets::detail_view> _to_view;
    _to_view.push_back(view);
    return _to_view;
}

std::string sockets::ethernet::source_layer_(int type)
{
    switch (type - 1)
    {
        case LayerType::Physic:
            return std::string(ether_ntoa((struct ether_addr*)e_ptr->ether_shost));
        default:
            return "";
    }
}

std::string sockets::ethernet::destination_layer_(int type)
{
    switch (type - 1)
    {
        case LayerType::Physic:
            return std::string(ether_ntoa((struct ether_addr*)e_ptr->ether_dhost));
        default:
            return "";
    }

}

std::string sockets::ethernet::protocol_layer_(int type)
{
    switch (type - 1)
    {
        case LayerType::Physic:
            return "ethernet";
        default:
            return "";
    }
}

sockets::ethernet::~ethernet()
{
    e_ptr = NULL;
}
