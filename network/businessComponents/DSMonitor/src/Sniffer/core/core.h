#pragma once

#include <pcap.h>
#include <vector>
#include <string>

// #include <netinet/ether.h>
// #include <netinet/if_ether.h>
// #include <utility>

// #include "config.h"
// #include "core/structs.h"
// #include "core/sockets.h"

class config;

namespace core
{
void control_handler(int s);

class sniffer
{
public:
    static sniffer* GetInstance(config &config);

    void start_listen();
    void pause_listening();
    void close();

protected:
    explicit sniffer(config &config);

    void init_listening_offline();
    void init_listening_online();
    static bool user_an_admin();

private:
    config &configs;
    pcap_t* device;
    bool is_listen;
    char err_buf[PCAP_ERRBUF_SIZE];
    static sniffer* sniffer_;
};

std::vector<std::string> device_list();

void console_device_list(const std::vector<std::string> &lists);

void console_handler(u_char* args, const struct pcap_pkthdr* pkt_hdr, const u_char* packet);

void file_handler(u_char* args, const struct pcap_pkthdr* pkt_hdr, const u_char* packet);

void ui_handler(u_char* args, const struct pcap_pkthdr* pkt_hdr, const u_char* packet);

void write_to_file(config &config, std::vector<pcap_pkthdr> &pkt_hdr, std::vector<const u_char*> &packet);
}
