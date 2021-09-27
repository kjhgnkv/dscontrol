#pragma once

#include <QString>
#include <QStringList>
#include <QUuid>

#include "nodes/Serializable.hpp"
#include "common/nodeeditor_globals.hpp"

namespace QtNodes
{
// TODO associate with real communication
struct MessageCommunication
{
    CommId id_;
    QString name_;

    inline bool operator==(const MessageCommunication &other)
    {
        return this->id_ == other.id_ && this->name_ == other.name_;
    }
};

class Message : public Serializable
{
public:
    explicit Message(const int &id);
    ~Message() = default;

    QString name_;
    int id_;
    quint64 messId_;
    struct Protocol
    {
        int id = 0;
        QString name = "default";
        QString factory = "default";
        QString component = "";
    };
    Protocol protocol_;
    Protocol internalProtocol_;
    std::vector<MessageCommunication> communications;

    // Serializable interface
public:
    QJsonObject save() const;
    void restore(const QJsonObject &p);
};

} // namespace QtNodes
Q_DECLARE_METATYPE(QtNodes::MessageCommunication)
