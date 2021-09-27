#pragma once

#include <string>
#include <map>

namespace dsmonitor
{
struct ProcessData;

struct MonitorData
{
private:
    typedef std::map<std::string, std::string> MapString;

public:
    // data memory
    MapString infoMemory;
    // data storage
    MapString infoStorage;
    // data system
    MapString infoSystem;
    // data cpu
    MapString infoCpu;
    // data network
    MapString infoNetwork;
    // data process
    std::map<std::string, ProcessData> process;
};
}