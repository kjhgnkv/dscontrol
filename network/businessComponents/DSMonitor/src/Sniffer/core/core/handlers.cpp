#include <cstring>
#include <cstdlib>

#include "../../utils/utils.h"
#include "../core.h"
#include "../sockets.h"

int count = 0;

void core::control_handler(int s)
{
    printf("\nCaptured: %d\n", count);
    exit(EXIT_SUCCESS);
}

void core::console_handler(u_char* args, const struct pcap_pkthdr* pkt_hdr, const u_char* packet)
{
    sockets::base_socket* sk = sockets::parse_packet(args, pkt_hdr, packet);
    count++;

    if (sk != NULL)
    {
        sk->print();
    }
}

void core::file_handler(u_char* dumpfile, const struct pcap_pkthdr* pkt_hdr, const u_char* packet)
{
    count++;
    pcap_pkthdr pktHdr = {};
    pktHdr.caplen = pkt_hdr->caplen;
    pktHdr.len = pkt_hdr->len;
    pktHdr.ts.tv_sec = pkt_hdr->ts.tv_sec;
    pktHdr.ts.tv_usec = pkt_hdr->ts.tv_usec;

    pcap_dump(dumpfile, &pktHdr, packet);
}

void core::ui_handler(u_char* args, const struct pcap_pkthdr* pkt_hdr, const u_char* packet)
{
    // TODO
    sockets::base_socket* package = sockets::parse_packet(NULL, pkt_hdr, packet);

    // sockets::table_view row = package->to_row();
    // mutex.lock();
    // package->print();
    // utils::logMemory();

    sockets::remove_package(package);
    // mutex.unlock();
}

void core::write_to_file(config &config, std::vector<pcap_pkthdr> &pkt_hdr, std::vector<const u_char*> &packet)
{
    pcap_t* dsc = pcap_open_dead(DLT_EN10MB, 65536);
    pcap_dumper_t* dumpfile = pcap_dump_open(dsc, config.to_file_name.c_str());

    for (size_t i = 0; i < pkt_hdr.size(); i++)
    {
        core::file_handler((uint8_t*)dumpfile, &pkt_hdr[i], packet[i]);
    }

    pcap_dump_close(dumpfile);
    pcap_close(dsc);
}