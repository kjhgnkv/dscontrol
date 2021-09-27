#include "LocalConnection.hpp"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariant>


LocalConnection::LocalConnection(int pid, const std::vector<Poco::UInt64> &ipAddresses
                                 // , ds::middleware::ApplicationType::Enum appType
                                 // , ds::middleware::TopicType::Enum topicType
                                 )
    : pid_{pid}
    , ipAddresses_{ipAddresses}
    // , appType_{appType}
    // , topicType_{topicType}
{
    key_.pid_ = pid;
    key_.ipAddresses_ = ipAddresses;
}

Poco::SharedPtr<Receiver> &LocalConnection::receiver()
{
    return localReceiver_;
}

void LocalConnection::setReceiver(Poco::SharedPtr<Receiver> &localReceiver)
{
    localReceiver_ = localReceiver;
}

bool LocalConnection::parse(const std::string &json, std::vector<Poco::SharedPtr<LocalConnection>> &connections)
{
    connections.clear();

    QJsonDocument doc = QJsonDocument::fromJson(QString::fromStdString(json).toUtf8());
    if (doc.isNull() || !doc.isObject())
    {
        return false;
    }

    QJsonObject obj = doc.object();
    qDebug() << obj;

    QJsonArray arr = obj["topics"].toArray();

    for (auto iVal : arr)
    {
        auto inObj = iVal.toObject();
        QJsonArray addresses = inObj["ipAddresses"].toArray();
        std::vector<Poco::UInt64> ipAddresses;
        for (auto jVal : addresses)
        {
            ipAddresses.push_back(jVal.toVariant().toUInt());
        }
        int pid = inObj["pid"].toInt();
        // ds::middleware::ApplicationType::Enum appType = ds::middleware::ApplicationType::fromString(inObj["appType"].toString().toStdString());
        // ds::middleware::TopicType::Enum topicType = ds::middleware::TopicType::fromString(inObj["topicType"].toString().toStdString());

        qDebug() << inObj["pid"];
        qDebug() << inObj["appType"];
        // qDebug() << ds::middleware::TopicType::fromString(inObj["topicType"].toString().toStdString());

        // TODO check if none

        // connections.push_back(Poco::SharedPtr<LocalConnection>(new LocalConnection(pid, ipAddresses, appType, topicType)));
    }

    return true;
}

const std::vector<Poco::UInt64> &LocalConnection::ipAddresses() const
{
    return ipAddresses_;
}

// ds::middleware::TopicType::Enum LocalConnection::topicType()
// {
//     return topicType_;
// }

int LocalConnection::pid() const
{
    return pid_;
}

// ds::middleware::ApplicationType::Enum LocalConnection::appType()
// {
//     return appType_;
// }

const ConnectionKey &LocalConnection::key() const
{
    return key_;
}
