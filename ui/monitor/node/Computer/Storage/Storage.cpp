#include "Storage.hpp"

#include <QJsonObject>

#include "../../network/businessComponents/DSMonitor/src/DSMonitorHelp.hpp"

using namespace monitor;

void Storage::update(const QJsonValue &value)
{
    QJsonObject object = value.toObject();

    QString keyLoad = dsmonitor::Help::Storage::toString(dsmonitor::Help::Storage::load).c_str();
    load_ = object[keyLoad].toString();

    QString keyFree = dsmonitor::Help::Storage::toString(dsmonitor::Help::Storage::free).c_str();
    free_ = object[keyFree].toString();

    QString keyTotal = dsmonitor::Help::Storage::toString(dsmonitor::Help::Storage::total).c_str();
    total_ = object[keyTotal].toString();

    QString keyUsed = dsmonitor::Help::Storage::toString(dsmonitor::Help::Storage::used).c_str();
    used_ = object[keyUsed].toString();

    QString keyDevice = dsmonitor::Help::Storage::toString(dsmonitor::Help::Storage::device).c_str();
    device_ = object[keyDevice].toString();

    QString keyType = dsmonitor::Help::Storage::toString(dsmonitor::Help::Storage::type).c_str();
    type_ = object[keyType].toString();

    QString keyPath = dsmonitor::Help::Storage::toString(dsmonitor::Help::Storage::path).c_str();
    path_ = object[keyPath].toString();
}

const QString &Storage::load() const
{
    return load_;
}

const QString &Storage::free() const
{
    return free_;
}

const QString &Storage::total() const
{
    return total_;
}

const QString &Storage::used() const
{
    return used_;
}

const QString &Storage::device() const
{
    return device_;
}

const QString &Storage::type() const
{
    return type_;
}

const QString &Storage::path() const
{
    return path_;
}