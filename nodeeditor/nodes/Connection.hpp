#pragma once

#include "nodeeditor/nodeeditor_export.h"
#include "ConnectionID.hpp"
#include "Serializable.hpp"
#include <QObject>
#include <QUuid>
#include <QVariant>
#include <memory>

class QPointF;

namespace QtNodes
{
class Node;
class NodeData;

/// Connection is a representation in DataFlowScene of a connection
/// It is part of the model, and not the rendering system.
/// This class is not to be used if you're implementing FlowScene model yourself
class NODEEDITOR_EXPORT Connection : public QObject, public Serializable
{
Q_OBJECT
public:
    enum class Type
    {
        Out = 0,
        In = 1
    };

public:
    Connection(Node &nodeIn, Node &nodeOut, Node &nodeServer, CommId commIdx, Device::Type deviceType, const QString &deviceName,
               const QString &commName, MsgId msgIdx);

    Connection(Node &nodeIn, Node &nodeOut);
    Connection(const Connection &) = delete;
    Connection &operator=(const Connection &) = delete;
    ~Connection() override;

    QJsonObject save() const override;
    void restore(const QJsonObject &obj) override;

    ConnectionID id() const;
    QUuid uuid() const;
    Node* getNode(QtNodes::Item::PortType) const;

    void setUuid(const QUuid &uuid);
    void setCommId(CommId id);
    void setDeviceName(const QString &devName);
    void setCommName(const QString &commName);
    void setMessageId(MsgId msgId);
    void setDeviceType(Device::Type deviceType);
    void setServer(Node& nodeServer);

signals:
    void updated(Connection &conn) const;
    void deviceTypeHasUpdated(Device::Type deviceType);
    void deviceNameHasUpdated(const QString &name);
    void connectionDestroyed() const;

private:
    QUuid _uid;
    Node* outNode_ = nullptr;
    Node* inNode_ = nullptr;
    Node* serverNode_ = nullptr;

    CommId commId_;

    QString deviceName_;
    Device::Type deviceType_;
    QtNodes::MsgId msgId_;
    QString commName_;
};

} // namespace QtNodes
