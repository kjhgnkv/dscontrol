#pragma once

#include "lists/Message.hpp"
#include "nodes/Serializable.hpp"

#include <QObject>
#include <QUuid>
#include <QVariantList>
#include <memory>

namespace QtNodes
{
class Message;
class CommunicationList;
struct MessageCommunication;

class MessageList final : public QObject, public Serializable
{
Q_OBJECT
public:
    enum Columns
    {
        Name,
        Id,
        Protocol,
        CommNames,
        CommCount
    };

    explicit MessageList(CommunicationList* commList, QObject* parent = nullptr);
    ~MessageList() override;

    // Serializable interface
public:
    QJsonObject save() const override;
    void restore(const QJsonObject &p) override;

    size_t size();

    std::vector<std::shared_ptr<Message>> messages();
    QString messageName(const int &messId);
    QString messageProtocol(const int &commId);
    std::vector<MessageCommunication> messageComms(const int &commId);
    quint64 messageId(const int &msgId);
    int messageUid(const int &msgUid);

    int addMessage();
    int addMessage(std::shared_ptr<Message> mess);
    bool removeMessage(const int &commId);
    void updateMessage(const int &commId, QVariantList updates, int role);

    void clear();
signals:
    void messageAdded(int);
    void messageToBeRemoved(int);
    void messageUpdated(int, int);

private:
    std::vector<std::shared_ptr<Message>> messages_;
    CommunicationList* commList_;
};

} // namespace QtNodes
