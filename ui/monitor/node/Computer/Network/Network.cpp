#include "Network.hpp"

#include <QJsonObject>

#include "../../network/businessComponents/DSMonitor/src/DSMonitorHelp.hpp"

using namespace monitor;

void Network::update(const QJsonValue &value)
{
    QJsonObject dataObject = value.toObject();

    QString keyHostName = dsmonitor::Help::Network::toString(dsmonitor::Help::Network::hostName).c_str();
    hostName_ = dataObject[keyHostName].toString();

    QString keyInterface = dsmonitor::Help::Network::toString(dsmonitor::Help::Network::interface).c_str();
    interface_ = dataObject[keyInterface].toString();

    QString keyIPv4 = dsmonitor::Help::Network::toString(dsmonitor::Help::Network::IPv4).c_str();
    ipAddress_ = dataObject[keyIPv4].toString();
}

const QString &Network::interface() const
{
    return interface_;
}

const QString &Network::hostName() const
{
    return hostName_;
}

const QString &Network::ipAddress() const
{
    return ipAddress_;
}
