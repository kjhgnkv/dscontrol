#include <cstdlib>
#include <sstream>

#include "../core/core.h"
#include "../IO/IO.h"
#include "sockets.h"

#if _WIN32
    #include <Shlobj.h>
#elif __linux

    #include <unistd.h>

#endif

core::sniffer* core::sniffer::sniffer_ = NULL;

core::sniffer::sniffer(config &config)
: configs(config)
, device(NULL)
, is_listen(false)
, err_buf()
{
}

core::sniffer* core::sniffer::GetInstance(config &config)
{
    if (sniffer_ == NULL)
    {
        sniffer_ = new sniffer(config);
    }

    return sniffer_;
}

void core::sniffer::pause_listening()
{
    pcap_breakloop(device);
}

void core::sniffer::start_listen()
{
    is_listen = true;
    if (!configs.device.empty())
    {
        init_listening_online();
    }
    else
    {
        init_listening_offline();
    }
}

bool core::sniffer::user_an_admin()
{
    #if defined _WIN32
    bool UserAnAdmin = IsUserAnAdmin();
    #elif __linux
    bool UserAnAdmin = !getuid();
    #endif

    return !UserAnAdmin;
}

void core::sniffer::init_listening_online()
{
    if (user_an_admin())
    {
        IO::print_err("Program not open root!");
        exit(EXIT_FAILURE);
    }

    struct bpf_program fp = {};
    if (device == NULL)
    {
        device = pcap_open_live(configs.device.c_str(), BUFSIZ, 1, -1, err_buf);
        if (device == NULL)
        {
            IO::print_err(err_buf);
            return;
        }
    }

    if (!configs.protoc_filter.empty())
    {
        if (pcap_compile(device, &fp, configs.protoc_filter.c_str(), 0, 0) == -1)
        {
            IO::print_err("Couldn't parse filter: " + configs.protoc_filter);
            exit(EXIT_FAILURE);
        }

        if (pcap_setfilter(device, &fp) == -1)
        {
            IO::print_err("Couldn't install filter");
            exit(EXIT_FAILURE);
        }
    }

    if (!configs.console)
    {
        pcap_loop(device, configs.amount, core::ui_handler, NULL);
    }
    else if (configs.to_file)
    {
        pcap_dumper_t* dumpfile = pcap_dump_open(device, configs.to_file_name.c_str());
        pcap_loop(device, configs.amount, core::file_handler, (unsigned char*)dumpfile);
    }
    else
    {
        pcap_loop(device, configs.amount, core::console_handler, NULL);
    }
}

void core::sniffer::init_listening_offline()
{
    pcap_t* dumpfile = pcap_open_offline(configs.from_file_name.c_str(), err_buf);

    if (dumpfile == NULL)
    {
        IO::println(err_buf);
        exit(EXIT_FAILURE);
    }

    if (!configs.console)
    {
        pcap_loop(dumpfile, configs.amount, &core::ui_handler, NULL);
    }
    else
    {
        pcap_loop(dumpfile, configs.amount, &core::console_handler, NULL);
    }
}

void core::sniffer::close()
{
    if (device != NULL)
    {
        pcap_close(device);
    }
}

std::ostream &sockets::hex_dump(std::stringstream &os, const char* buffer, std::size_t bufsize)
{
    for (size_t i = 0; i < bufsize; i++)
    {
        if (isalnum(buffer[i]))
        {
            os << buffer[i];
        }
        else
        {
            os << ".";
        }
    }

    return os;
}
