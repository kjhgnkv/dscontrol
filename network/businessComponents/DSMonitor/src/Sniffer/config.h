#pragma once

#include <string>
#include <vector>

#define FILE_RECENT_LOGS "recent.txt"

struct config
{
    config()
    : to_file(false)
    , from_file(false)
    , both_out(false)
    , console(false)
    , device(std::string())
    , to_file_name("log.pcap")
    , from_file_name("log.pcap")
    , amount(-1)
    , protoc_filter(std::string())
    , captured(false)
    {
    }

    bool to_file;
    bool from_file;
    bool both_out;
    bool console;
    std::string device;
    std::string to_file_name;
    std::string from_file_name;
    int amount;
    std::string protoc_filter;
    bool captured;
};

class LengthType
{
public:
    enum Type
    {
        Great = 0,
        Less,
        Eaquals
    };
};

class LayerType
{
public:
    enum Type
    {
        Physic,
        Network,
        Transport
    };
};