#include "System.hpp"

#include <QJsonObject>

#include "../../network/businessComponents/DSMonitor/src/DSMonitorHelp.hpp"

using namespace monitor;

void System::update(const QJsonValue &value)
{
    QJsonObject object = value.toObject();

    QString keyID = dsmonitor::Help::System::toString(dsmonitor::Help::System::id).c_str();
    id_ = object[keyID].toString();

    QString keyProductName = dsmonitor::Help::System::toString(dsmonitor::Help::System::productName).c_str();
    productName_ = object[keyProductName].toString();

    QString keyKernelType = dsmonitor::Help::System::toString(dsmonitor::Help::System::kernelType).c_str();
    kernelType_ = object[keyKernelType].toString();

    QString keyArchitecture = dsmonitor::Help::System::toString(dsmonitor::Help::System::architecture).c_str();
    architecture_ = object[keyArchitecture].toString();
}

const QString &System::id() const
{
    return id_;
}

const QString &System::productName() const
{
    return productName_;
}

const QString &System::kernelType() const
{
    return kernelType_;
}

const QString &System::architecture() const
{
    return architecture_;
}

