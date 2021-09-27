#include "Memory.hpp"

#include <QJsonObject>

#include "../../network/businessComponents/DSMonitor/src/DSMonitorHelp.hpp"

using namespace monitor;

void Memory::update(const QJsonValue &value)
{
    QJsonObject object = value.toObject();

    QString keyLoad = dsmonitor::Help::Memory::toString(dsmonitor::Help::Memory::load).c_str();
    load_ = object[keyLoad].toString();

    QString keyTotal = dsmonitor::Help::Memory::toString(dsmonitor::Help::Memory::total).c_str();
    total_ = object[keyTotal].toString();

    QString keyFree = dsmonitor::Help::Memory::toString(dsmonitor::Help::Memory::free).c_str();
    free_ = object[keyFree].toString();

    QString keyUsed = dsmonitor::Help::Memory::toString(dsmonitor::Help::Memory::used).c_str();
    used_ = object[keyUsed].toString();
}

const QString &Memory::load() const
{
    return load_;
}

const QString &Memory::total() const
{
    return total_;
}

const QString &Memory::free() const
{
    return free_;
}

const QString &Memory::used() const
{
    return used_;
}
