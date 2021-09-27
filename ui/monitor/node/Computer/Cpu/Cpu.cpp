#include "Cpu.hpp"

#include <QJsonObject>

#include "../../network/businessComponents/DSMonitor/src/DSMonitorHelp.hpp"

using namespace monitor;

void Cpu::update(const QJsonValue &value)
{
    QJsonObject object = value.toObject();

    QString keyName = dsmonitor::Help::Cpu::toString(dsmonitor::Help::Cpu::name).c_str();
    name_ = object[keyName].toString();

    QString keyMin = dsmonitor::Help::Cpu::toString(dsmonitor::Help::Cpu::min).c_str();
    QString keyMax = dsmonitor::Help::Cpu::toString(dsmonitor::Help::Cpu::max).c_str();
    minMaxbHz_ = object[keyMin].toString() + "/" + object[keyMax].toString();

    QString keyCache = dsmonitor::Help::Cpu::toString(dsmonitor::Help::Cpu::cache).c_str();
    caches_ = object[keyCache].toString();

    QString keyCores = dsmonitor::Help::Cpu::toString(dsmonitor::Help::Cpu::cores).c_str();
    cores_ = object[keyCores].toString();

    QString keyThreads = dsmonitor::Help::Cpu::toString(dsmonitor::Help::Cpu::threads).c_str();
    threads_ = object[keyThreads].toString();

    QString keyUsage = dsmonitor::Help::Cpu::toString(dsmonitor::Help::Cpu::usage).c_str();
    usage_ = object[keyUsage].toString();
}

const QString &Cpu::name() const
{
    return name_;
}

const QString &Cpu::usage() const
{
    return usage_;
}

const QString &Cpu::minMaxbHz() const
{
    return minMaxbHz_;
}

const QString &Cpu::cores() const
{
    return cores_;
}

const QString &Cpu::threads() const
{
    return threads_;
}

const QString &Cpu::caches() const
{
    return caches_;
}