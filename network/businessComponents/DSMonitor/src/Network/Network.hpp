#pragma once

#include <string>
#include <Poco/Net/NetworkInterface.h>

namespace dsmonitor
{
class Network
{
protected:
    typedef std::pair<std::string, std::string> PairString;

private:
    typedef Poco::Net::NetworkInterface NetworkInterface;

public:
    static bool getInfoNetwork(std::map<std::string, std::string> &infoNetwork);
    static bool findDevice(const std::string &ipFind, std::string &interface);
    static void showListNetInterface();
    static bool getIp(const std::string &ip_port, std::string &ip);
};
}