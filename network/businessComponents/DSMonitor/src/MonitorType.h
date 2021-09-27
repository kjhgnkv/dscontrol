#pragma once

#include <string>
#include <vector>
#include <Poco/Net/NetworkInterface.h>
#include <Poco/SharedPtr.h>

#include "DSMonitorHelp.hpp"
#include "Network/Network.hpp"

namespace dsmonitor
{
struct ParserData;
struct MonitorData;
struct ProcessData;

class MonitorType : public Network
{
protected:
    typedef std::pair<std::string, ProcessData> PairProcess;

public:
    // updates all data
    MonitorData update(const std::vector<ParserData> &);

protected:
    // getting the data memory
    virtual bool getInfoMemory(std::map<std::string, std::string> &) = 0;
    // getting the data storage
    virtual bool getInfoStorage(std::map<std::string, std::string> &) = 0;
    // getting the data system
    virtual bool getInfoSystem(std::map<std::string, std::string> &) = 0;
    // getting the data cpu
    virtual bool getInfoCpu(std::map<std::string, std::string> &) = 0;
    // getting the data process's
    virtual bool processes(const std::string &procNameApp, const std::string &procPathApp, ProcessData &proc) = 0;
};
}