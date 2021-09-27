#include "Communication.hpp"

#include <QDebug>

QtNodes::Communication::Communication(const int &id)
: deviceType_ {Device::Type::None}
, deviceName_ {QObject::tr("None")}
, commName_ {""}
, id_ {id}
, connectionMethod_ {""}
, commSender_ {""}
, commReceiver_ {""}
, commFragment_ {false}
{
    qDebug() << "Communication: contructor";
}

QJsonObject QtNodes::Communication::save() const
{
    QJsonObject retval;

    retval["DeviceType"] = static_cast<int>(deviceType_);
    retval["DeviceName"] = deviceName_;
    retval["Name"] = commName_;
    retval["Id"] = id_;
    retval["Method"] = connectionMethod_;
    retval["Component"] = methodComponent_;
    retval["Send"] = commSender_;
    retval["Recv"] = commReceiver_;
    retval["Fragment"] = commFragment_;

    return retval;
}

void QtNodes::Communication::restore(const QJsonObject &p)
{
    deviceType_ = static_cast<Device::Type>(p["DeviceType"].toInt());
    deviceName_ = p["DeviceName"].toString();
    commName_ = p["Name"].toString();
    id_ = p["Id"].toInt();
    connectionMethod_ = p["Method"].toString();
    methodComponent_ = p["Component"].toString();
    commSender_ = p["Send"].toString();
    commReceiver_ = p["Recv"].toString();
    commFragment_ = p["Fragment"].toBool();
}
