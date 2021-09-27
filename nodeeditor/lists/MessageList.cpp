#include "MessageList.hpp"

#include "lists/CommunicationList.hpp"
#include "lists/Message.hpp"

#include <QJsonArray>
#include <QJsonDocument>
#include <QVector>
#include <QDebug>
#include <QFile>

namespace QtNodes
{
QtNodes::MessageList::MessageList(QtNodes::CommunicationList* commList, QObject* parent)
: QObject(parent)
, commList_ {commList}
{
    qDebug() << "MessageList";

    connect(commList_, qOverload<QString>(&CommunicationList::communicationToBeRemoved),
            [=, &messages_ = messages_](const QString &name) mutable
            {
                for (const auto &mess : messages_)
                {
                    auto &comms = mess->communications;
                    if (auto found = std::find_if(comms.begin(), comms.end(), [=](const auto &val)
                        {
                            if (val.name_ == name)
                            {
                                return true;
                            }
                            return false;
                        });
                    found != comms.end())
                    {
                        comms.erase(found);
                        emit messageUpdated(mess->id_, Columns::CommNames);
                    }
                }
            });

    connect(commList_, &CommunicationList::communicationUpdated, [this](int commId, QVector<int> roles)
    {
        for (const auto &role : roles)
        {
            if (role == CommunicationList::Columns::CommName)
            {
                auto name = commList_->communicationName(commId);
                for (const auto &mess : messages_)
                {
                    auto &comms = mess->communications;
                    if (auto found = std::find_if(comms.begin(), comms.end(), [=](const auto &val)
                        {
                            if (val.id_ == commId)
                            {
                                return true;
                            }
                            return false;
                        });
                    found != comms.end())
                    {
                        found->name_ = name;
                        emit messageUpdated(mess->id_, Columns::CommNames);
                    }
                }
            }
        }
    });
}

MessageList::~MessageList()
{
}

QJsonObject MessageList::save() const
{
    qDebug() << "MessageList save:";
    QJsonObject retval;

    QJsonArray messages;
    for (const auto &msg : messages_)
    {
        messages.push_back(msg->save());
    }

    retval["messages"] = messages;
    //TODO: for test remove
//     QFile file{"messages"};
//     if (file.open(QIODevice::WriteOnly))
//     {
//         file.write(QJsonDocument{retval}.toJson());
//         file.close();
//     }

    

    qDebug() << retval;
    return retval;
}

void MessageList::restore(const QJsonObject &p)
{
    QJsonArray arr = p["messages"].toArray();
    std::vector<std::shared_ptr<Message>> vec {};

    for (const auto &mess : arr)
    {
        auto m = std::make_shared<Message>(0);
        m->restore(mess.toObject());
        vec.push_back(m);
    }

    std::sort(vec.begin(), vec.end(), [](const auto &valS, const auto &valE)
    {
        return valS->id_ < valE->id_;
    });

    for (const auto &mess : vec)
    {
        addMessage(mess);
    }
}

size_t MessageList::size()
{
    return messages_.size();
}

std::vector<std::shared_ptr<Message>> MessageList::messages()
{
    return messages_;
}

QString MessageList::messageName(const int &messId)
{
    if (auto found = std::find_if(messages_.begin(), messages_.end(), [messId](const auto &val)
        {
            if (val->id_ == messId)
                return true;
            return false;
        });
    found != messages_.end())
    {
        return found->get()->name_;
    }
    return {};
}

QString MessageList::messageProtocol(const int &messId)
{
    if (auto found = std::find_if(messages_.begin(), messages_.end(), [messId](const auto &val)
        {
            if (val->id_ == messId)
                return true;
            return false;
        });
    found != messages_.end())
    {
        return found->get()->protocol_.name;
    }
    return {};
}

std::vector<MessageCommunication> MessageList::messageComms(const int &messId)
{
    if (auto found = std::find_if(messages_.begin(), messages_.end(), [messId](const auto &val)
        {
            if (val->id_ == messId)
                return true;
            return false;
        });
    found != messages_.end())
    {
        return found->get()->communications;
    }
    return {};
}

quint64 MessageList::messageId(const int &msgUid)
{
    if (auto found = std::find_if(messages_.begin(), messages_.end(), [msgUid](const auto &val)
        {
            if (val->id_ == msgUid)
                return true;
            return false;
        });
    found != messages_.end())
    {
        return found->get()->messId_;
    }
    return {};
}

int MessageList::messageUid(const int &msgId)
{
    if (auto found = std::find_if(messages_.begin(), messages_.end(), [msgId](const auto &val)
        {
            if (val->messId_ == msgId)
                return true;
            return false;
        });
    found != messages_.end())
    {
        return found->get()->id_;
    }
    return {};
}

int MessageList::addMessage()
{
    messages_.push_back(std::make_shared<QtNodes::Message>(size()));

    emit messageAdded(int(size() - 1));

    return int(size() - 1);
}

int MessageList::addMessage(std::shared_ptr<Message> mess)
{
    mess->id_ = size();
    messages_.push_back(mess);

    emit messageAdded(mess->id_);
    // }

    return mess->id_;
}

bool MessageList::removeMessage(const int &commId)
{
    int row {};

    if (auto found = std::find_if(messages_.begin(), messages_.end(), [commId, &row](const auto &val) mutable
        {
            if (val->id_ == commId)
                return true;

            row++;
            return false;
        });
    found != messages_.end())
    {
        emit messageToBeRemoved(row);
        auto next = messages_.erase(found);

        // NOTE Maybe need fix
        for (; next != messages_.end(); next++)
        {
            (next->get()->id_)--;
        }
        return true;
    }
    return false;
}

void MessageList::updateMessage(const int &commId, QVariantList updates, int role)
{
    switch (role)
    {
        case Columns::Name:
        {
            auto name = updates.takeFirst().toString();
            bool update = true;
            for (const auto &comm : messages_)
            {
                if (comm->name_ == name)
                {
                    update = false;
                    break;
                }
            }
            if (update)
            {
                messages_.at(commId)->name_ = name;
            }
            break;
        }

        case Columns::Id:
        {
            auto id = updates.takeFirst().toInt();
            bool update = true;
            for (const auto &comm : messages_)
            {
                if (comm->messId_ == id)
                {
                    update = false;
                    break;
                }
            }
            if (update)
            {
                messages_.at(commId)->messId_ = id;
            }
            break;
        }

        case Columns::Protocol:
        {
            messages_.at(commId)->protocol_.name = updates.takeFirst().toString();
            break;
        }

        case Columns::CommNames:
        {
            auto &comms = messages_.at(commId)->communications;
            for (const auto &data : updates)
            {
                auto cast = data.value<MessageCommunication>();
                auto found = std::find_if(comms.begin(), comms.end(), [=](const auto &val)
                {
                    if (val.id_ == cast.id_)
                    {
                        return true;
                    }
                    return false;
                });
                if (found != comms.end())
                {
                    found->name_ = cast.name_;
                }
                else
                {
                    comms.push_back(cast);
                }
            }
            break;
        }
        case Columns::CommCount:
        {
            auto &communications = messages_.at(commId)->communications;
            communications.clear();
            for (const auto &data : updates)
            {
                auto cast = data.value<MessageCommunication>();
                communications.push_back(cast);
            }
            break;
        }

        default:
            break;
    }

    emit messageUpdated(commId, role);
}

void MessageList::clear()
{
    for (const auto &mess : messages_)
    {
        emit messageToBeRemoved(mess->id_);
    }
    messages_.clear();
}
} // namespace QtNodes
