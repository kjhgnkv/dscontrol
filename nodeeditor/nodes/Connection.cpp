#include "Connection.hpp"
#include "Node.hpp"

#include <QtGlobal>

using QtNodes::Connection;
using QtNodes::ConnectionID;
using QtNodes::Node;
using QtNodes::NodeData;
using namespace QtNodes;

Connection::Connection(Node &nodeIn, Node &nodeOut, Node &nodeServer, CommId commId, Device::Type deviceType, const QString &deviceName,
                       const QString &commName, MsgId msgId)
: _uid {QUuid::createUuid()}
, outNode_ {&nodeOut}
, inNode_ {&nodeIn}
, serverNode_ {&nodeServer}
, commId_ {commId}
, deviceName_ {deviceName}
, deviceType_ {deviceType}
, msgId_ {msgId}
, commName_ {commName}
{
}

Connection::Connection(QtNodes::Node &nodeIn, QtNodes::Node &nodeOut)
: outNode_ {&nodeOut}
, inNode_ {&nodeIn}
{
}

Connection::~Connection()
{
    emit connectionDestroyed();
}

ConnectionID Connection::id() const
{
    ConnectionID ret;
    ret.lNodeID_ = getNode(QtNodes::Item::PortType::Out)->id();
    ret.rNodeID_ = getNode(QtNodes::Item::PortType::In)->id();
    ret.sNodeID_ = serverNode_->id();

    ret.commId_ = commId_;

    ret.deviceName_ = deviceName_;
    ret.deviceType_ = deviceType_;

    ret.commName_ = commName_;
    ret.msgId_ = msgId_;

    ret.id_ = _uid;

    return ret;
}

QUuid Connection::uuid() const
{
    return _uid;
}

void Connection::setUuid(const QUuid &uuid)
{
    _uid = uuid;
}

Node* Connection::getNode(QtNodes::Item::PortType e) const
{
    switch (e)
    {
        case QtNodes::Item::PortType::In:
        {
            return inNode_;
        }

        case QtNodes::Item::PortType::Out:
        {
            return outNode_;
        }

        default:
        {
            // not possible
            break;
        }
    }
    Q_UNREACHABLE();
}

void Connection::setCommId(CommId id)
{
    commId_ = id;
}

void Connection::setDeviceName(const QString &devName)
{
    deviceName_ = devName;
    emit deviceNameHasUpdated(deviceName_);
}

void Connection::setCommName(const QString &commName)
{
    commName_ = commName;
    emit updated(*this);
}

void Connection::setMessageId(MsgId msgId)
{
    msgId_ = msgId;
    emit updated(*this);
}

void Connection::setDeviceType(Device::Type deviceType)
{
    if (deviceType != deviceType_)
    {
        deviceType_ = deviceType;
        emit deviceTypeHasUpdated(deviceType_);
    }
}

QJsonObject Connection::save() const
{
    QJsonObject connectionJson;

    if (inNode_ && outNode_)
    {
        connectionJson["id"] = _uid.toString();
        connectionJson["in_id"] = inNode_->id().toString();
        connectionJson["in_index"] = commId_;
        connectionJson["out_id"] = outNode_->id().toString();
        connectionJson["server_id"] = serverNode_->id().toString();
        connectionJson["communicationName"] = commName_;
        connectionJson["deviceType"] = static_cast<int>(deviceType_);
        connectionJson["deviceName"] = deviceName_;
        connectionJson["messageIdx"] = msgId_;
    }

    return connectionJson;
}

void Connection::restore(const QJsonObject &obj)
{
    _uid = QUuid::fromString(obj["id"].toString());
    commId_ = obj["in_index"].toInt();
    msgId_ = obj["messageIdx"].toInt();
    deviceName_ = obj["deviceName"].toString();
    deviceType_ = static_cast<Device::Type>(obj["deviceType"].toInt());
    commName_ = obj["communicationName"].toString();
}

void Connection::setServer(Node& nodeServer)
{
    serverNode_ = &nodeServer;
}
