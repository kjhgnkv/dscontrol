#pragma once

#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <ostream>
#include <sstream>

#if __linux__
    #include <netinet/if_ether.h>
#endif

#include <pcap.h>

class config;

namespace utils
{
typedef std::vector<std::string> VecString;

config cli_parser(int argc, char** argv);
void print_payload(const u_char* payload, int len);

bool is_integer(const std::string &s);

int to_number(const std::string &s);

template <typename T>
std::string to_string(const T &t)
{
    std::ostringstream convert;
    convert << t;
    return convert.str();
}

std::vector<std::string> get_recent_files();

void set_recent_file(const std::string &file_name);

template <class T1, class T2>
std::pair<T1, T2> make_pair(T1 x, T2 y)
{
    return (std::pair<T1, T2>(x, y));
}

class Ether
{
public:
    enum Type
    {
        ETHER_IP = ETHERTYPE_IP,
        ETHER_ARP = ETHERTYPE_ARP,
        ETHER_REVARP = ETHERTYPE_REVARP,
        ETHER_AARP = ETHERTYPE_AARP,
        ETHER_PUP = ETHERTYPE_PUP,
        ETHER_SPRITE = ETHERTYPE_SPRITE,
        ETHER_AT = ETHERTYPE_AT,
        ETHER_VLAN = ETHERTYPE_VLAN,
        ETHER_IPX = ETHERTYPE_IPX,
        ETHER_IPV6 = ETHERTYPE_IPV6,
        ETHER_LOOPBACK = ETHERTYPE_LOOPBACK,
        ETHER_NONE
    };

    static std::string to_string(uint16_t type)
    {
        switch (type)
        {
            case ETHER_IP:
                return "IP";
            case ETHER_ARP:
                return "ARP";
            case ETHER_REVARP:
                return "Reverse ARP";
            case ETHER_AARP:
                return "AppleTalk ARP";
            case ETHER_PUP:
                return "Xerox PUP";
            case ETHER_SPRITE:
                return "Sprite";
            case ETHER_AT:
                return "AppleTalk";
            case ETHER_VLAN:
                return "IEEE 802.1Q VLAN";
            case ETHER_IPX:
                return "IPX";
            case ETHER_IPV6:
                return "IPv6";
            case ETHER_LOOPBACK:
                return "loopback";
            case ETHER_NONE:
            default:
                return "?";
        }
    }

    static Type to_type(const std::string &type)
    {
        if (type == "IP")
            return ETHER_IP;
        if (type == "ARP")
            return ETHER_ARP;
        if (type == "Reverse ARP")
            return ETHER_REVARP;
        if (type == "AppleTalk ARP")
            return ETHER_AARP;
        if (type == "Xerox PUP")
            return ETHER_PUP;
        if (type == "Sprite")
            return ETHER_SPRITE;
        if (type == "AppleTalk")
            return ETHER_AT;
        if (type == "IEEE 802.1Q VLAN")
            return ETHER_VLAN;
        if (type == "IPX")
            return ETHER_IPX;
        if (type == "IPv6")
            return ETHER_IPV6;
        if (type == "loopback")
            return ETHER_LOOPBACK;

        return ETHER_NONE;
    }
};

class Transport
{
public:
    enum Type
    {
        TCP,
        UDP,
        ICMP,
        NONE
    };

    static std::string to_string(uint16_t type)
    {
        switch (type)
        {
            case TCP:
                return "TCP";
            case UDP:
                return "UDP";
            case ICMP:
                return "ICMP";
            case NONE:
            default:
                return "None";
        }
    }

    static Type to_type(const std::string &type)
    {
        if (type == "TCP")
            return TCP;
        if (type == "UDP")
            return UDP;
        if (type == "ICMP")
            return ICMP;

        return NONE;
    }
};

class Network
{
public:
    enum Type
    {
        IP,
        ARP,
        NONE
    };

    static std::string to_string(uint16_t type)
    {
        switch (type)
        {
            case IP:
                return "IP";
            case ARP:
                return "ARP";
            default:
                return "None";
        }
    }

    static Type to_type(const std::string &type)
    {
        if (type == "IP")
            return IP;
        if (type == "ARP")
            return ARP;

        return NONE;
    }
};

class IPPROTO
{
public:
    enum Type
    {
        IP = IPPROTO_IP,
        ICMP = IPPROTO_ICMP,
        IGMP = IPPROTO_IGMP,
        IPIP = IPPROTO_IPIP,
        TCP = IPPROTO_TCP,
        EGP = IPPROTO_EGP,
        PUP = IPPROTO_PUP,
        UDP = IPPROTO_UDP,
        IDP = IPPROTO_IDP,
        TP = IPPROTO_TP,
        DCCP = IPPROTO_DCCP,
        IPV6 = IPPROTO_IPV6,
        RSVP = IPPROTO_RSVP,
        GRE = IPPROTO_GRE,
        ESP = IPPROTO_ESP,
        AH = IPPROTO_AH,
        MTP = IPPROTO_MTP,
        BEETPH = IPPROTO_BEETPH,
        ENCAP = IPPROTO_ENCAP,
        PIM = IPPROTO_PIM,
        COMP = IPPROTO_COMP,
        SCTP = IPPROTO_SCTP,
        UDPLITE = IPPROTO_UDPLITE,
        MPLS = IPPROTO_MPLS,
        RAW = IPPROTO_RAW,
        MAX = IPPROTO_MAX,
        NONE
    };

    static std::string to_string(u_char type)
    {
        switch (type)
        {
            case IP:
                return "IP";
            case ICMP:
                return "ICMP";
            case IGMP:
                return "IGMP";
            case IPIP:
                return "IPIP";
            case TCP:
                return "TCP";
            case EGP:
                return "EGP";
            case PUP:
                return "PUP";
            case UDP:
                return "UDP";
            case IDP:
                return "IDP";
            case TP:
                return "TP";
            case DCCP:
                return "DCCP";
            case IPV6:
                return "IPV6";
            case RSVP:
                return "RSVP";
            case GRE:
                return "GRE";
            case ESP:
                return "ESP";
            case AH:
                return "AH";
            case MTP:
                return "MTP";
            case BEETPH:
                return "BEETPH";
            case ENCAP:
                return "ENCAP";
            case PIM:
                return "PIM";
            case COMP:
                return "COMP";
            case SCTP:
                return "SCTP";
            case UDPLITE:
                return "UDPLITE";
            case MPLS:
                return "MPLS";
            case RAW:
                return "RAW";
            case MAX:
                return "MAX";
            case NONE:
            default:
                return "IP*";
        }
    }

    static Type to_type(const std::string &type)
    {
        if (type == "IP")
            return IP;
        if (type == "ICMP")
            return ICMP;
        if (type == "IGMP")
            return IGMP;
        if (type == "IPIP")
            return IPIP;
        if (type == "TCP")
            return TCP;
        if (type == "EGP")
            return EGP;
        if (type == "PUP")
            return PUP;
        if (type == "UDP")
            return UDP;
        if (type == "IDP")
            return IDP;
        if (type == "TP")
            return TP;
        if (type == "DCCP")
            return DCCP;
        if (type == "IPV6")
            return IPV6;
        if (type == "RSVP")
            return RSVP;
        if (type == "GRE")
            return GRE;
        if (type == "ESP")
            return ESP;
        if (type == "AH")
            return AH;
        if (type == "MTP")
            return MTP;
        if (type == "BEETPH")
            return BEETPH;
        if (type == "ENCAP")
            return ENCAP;
        if (type == "PIM")
            return PIM;
        if (type == "COMP")
            return COMP;
        if (type == "SCTP")
            return SCTP;
        if (type == "UDPLITE")
            return UDPLITE;
        if (type == "MPLS")
            return MPLS;
        if (type == "RAW")
            return RAW;
        if (type == "MAX")
            return MAX;

        return NONE;
    }
};

class Detail
{
public:
    enum Type
    {
        Source,
        Destination,
        Type,
        Ethernet,

    };
};

#ifdef __linux

    #include <iostream>
    #include <fstream>
    #include <unistd.h>

static bool logMemory()
{
    using std::ios_base;
    using std::ifstream;
    using std::string;

    double vm_usage = 0.0;
    double resident_set = 0.0;

    // 'file' stat seems to give the most reliable results
    //
    ifstream stat_stream("/proc/self/stat", ios_base::in);

    // dummy vars for leading entries in stat that we don't care about
    //
    string pid, comms, state, ppid, pgrp, session, tty_nr;
    string tpgid, flags, minflt, cminflt, majflt, cmajflt;
    string utime, stime, cutime, cstime, priority, nice;
    string O, itrealvalue, starttime;

    // the two fields we want
    //
    unsigned long vsize;
    long rss;

    stat_stream >> pid >> comms >> state >> ppid >> pgrp >> session >> tty_nr >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt >> utime >> stime >> cutime >> cstime >> priority >> nice >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

    stat_stream.close();

    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
    vm_usage = vsize / 1024.0;
    static double vm_usage_max = 0;
    resident_set = rss * page_size_kb;

    if (vm_usage > vm_usage_max)
    {
        vm_usage_max = vm_usage;

        // Poco::Dynamic::Var vvm_usage(vm_usage);
        // Poco::Dynamic::Var vresident_set(resident_set);

        std::cout << ("MEMORY: vm_usage=") << vm_usage << "; resident_set=" << resident_set << std::endl;

        return false;
    }

    return true;
}

#endif
}
