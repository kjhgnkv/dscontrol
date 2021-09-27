#include "Network.hpp"

#include <iostream>

#include <Poco/Net/NetworkInterface.h>
#include <Poco/Net/NetException.h>
#include <Poco/Net/HostEntry.h>
#include <Poco/Net/DNS.h>

#include "../../Logger.h"
using namespace DS::dsmonitor;

#include "../DSMonitorHelp.hpp"

namespace dsmonitor
{
// get all ip-address local machine
bool Network::getInfoNetwork(std::map<std::string, std::string> &infoNetwork)
{
    try
    {
        // get host machine
        const Poco::Net::HostEntry &entry = Poco::Net::DNS::thisHost();
        // get and insert host name
        infoNetwork.insert(PairString(Help::Network::toString(Help::Network::hostName), entry.name()));

        // get all interface
        Poco::Net::NetworkInterface::Map m = Poco::Net::NetworkInterface::map(false, false);
        for (Poco::Net::NetworkInterface::Map::const_iterator it = m.begin(); it != m.end(); ++it)
        {
            // it not IPv4, ignore
            if (!it->second.supportsIPv4())
            {
                continue;
            }

            // get nterface
            const Poco::Net::NetworkInterface::AddressList &ipList = it->second.addressList();
            Poco::Net::NetworkInterface::AddressList::const_iterator ipIt;
            for (ipIt = ipList.begin(); ipIt != ipList.end(); ++ipIt)
            {
                if (!ipIt->get<Poco::Net::NetworkInterface::IP_ADDRESS>().isSiteLocal())
                {
                    continue;
                }

                // get ip address
                std::string address = ipIt->get<NetworkInterface::IP_ADDRESS>().toString();
                // get mask
                std::string netmask = ipIt->get<NetworkInterface::SUBNET_MASK>().toString();
                // get gateway
                std::string gateway = ipIt->get<NetworkInterface::BROADCAST_ADDRESS>().toString();
                // get name interface
                std::string interface = it->second.name();
                // insert data
                infoNetwork.insert(PairString(Help::Network::toString(Help::Network::IPv4), address));
                infoNetwork.insert(PairString(Help::Network::toString(Help::Network::Mask), netmask));
                infoNetwork.insert(PairString(Help::Network::toString(Help::Network::Gateway), gateway));
                infoNetwork.insert(PairString(Help::Network::toString(Help::Network::interface), interface));

                break;
            }
        }
    }
    catch (Poco::Exception &ex)
    {
        Logger::warning(ex.what());
        return true;
    }

    return false;
}

bool Network::findDevice(const std::string &ipFind, std::string &interface)
{
    // std::cout << ("Find: " + ipFind);
    try
    {
        const Poco::Net::HostEntry &entry = Poco::Net::DNS::thisHost();
        // std::cout << ("PC: " + entry.name());

        NetworkInterface::Map m = NetworkInterface::map(false, false);
        for (NetworkInterface::Map::const_iterator it = m.begin(); it != m.end(); ++it)
        {
            if (it->second.supportsIPv4())
            {
                NetworkInterface::AddressList::const_iterator ipIt;

                std::pair<unsigned, NetworkInterface> pair = *it;
                NetworkInterface networkInterface = pair.second;

                NetworkInterface::AddressList::const_iterator itNet, endNet;
                for (itNet = networkInterface.addressList().begin(), endNet = networkInterface.addressList().end(); itNet != endNet; itNet++)
                {
                    NetworkInterface::AddressTuple tuple = *itNet;

                    std::string address = tuple.get<NetworkInterface::IP_ADDRESS>().toString();
                    std::string netmask = tuple.get<NetworkInterface::SUBNET_MASK>().toString();
                    std::string gateway = tuple.get<NetworkInterface::BROADCAST_ADDRESS>().toString();

                    if (ipFind == address)
                    {
                        interface = networkInterface.adapterName();
                        return false;
                    }
                }
            }
        }
    }
    catch (Poco::Exception &ex)
    {
        // Logger::error(ex.what());
        return true;
    }

    // Logger::error("Not find network interface!");
    return true;
}

bool Network::getIp(const std::string &ip_port, std::string &ip)
{
    int pos = ip_port.find(':');
    if (pos == std::string::npos)
    {
        std::cerr << ("ip send incorrect!!!");
        return true;
    }

    std::string get_ip = ip_port.substr(0, pos);
    std::string get_port = ip_port.substr(pos + 1, ip_port.length());

    ip = get_ip;

    return false;
}

void Network::showListNetInterface()
{
    try
    {
        const Poco::Net::HostEntry &entry = Poco::Net::DNS::thisHost();
        // std::cout << ("PC: " + entry.name());

        NetworkInterface::Map m = NetworkInterface::map(false, false);
        for (NetworkInterface::Map::const_iterator it = m.begin(); it != m.end(); ++it)
        {
            if (!it->second.supportsIPv4())
            {
                continue;
            }

            std::pair<unsigned, NetworkInterface> pair = *it;
            NetworkInterface networkInterface = pair.second;

            // std::cout << "Interface: " << networkInterface.adapterName() << std::endl;
            // std::cout << "pair: " << pair.first << std::endl;

            NetworkInterface::AddressList::const_iterator itNet, endNet;
            for (itNet = networkInterface.addressList().begin(), endNet = networkInterface.addressList().end(); itNet != endNet; itNet++)
            {
                // std::cout << "displayName: " << networkInterface.displayName() << std::endl;
                // std::cout << "displayName: " << networkInterface.name() << std::endl;
                NetworkInterface::AddressTuple tuple = *itNet;

                std::string address = tuple.get<NetworkInterface::IP_ADDRESS>().toString();
                std::string netmask = tuple.get<NetworkInterface::SUBNET_MASK>().toString();
                std::string gateway = tuple.get<NetworkInterface::BROADCAST_ADDRESS>().toString();

                // std::cout << "0: " << address << std::endl;
                // std::cout << "1: " << netmask << std::endl;
                // std::cout << "2: " << gateway << std::endl;
            }
        }
    }
    catch (Poco::Exception &ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
}
}