#include "Message.hpp"

#include <QJsonArray>

QtNodes::Message::Message(const int &id)
: Serializable()
, name_ {""}
, id_ {id}
, messId_ {0}
, protocol_ {}
, internalProtocol_{}
, communications {}
{
}

QJsonObject QtNodes::Message::save() const
{
    QJsonObject retval;

    //TODO: remake to new structure
    retval["name"] = name_;
    retval["uuid"] = id_;
    retval["id"] = (int)messId_;
    QJsonObject proto;
    proto["id"] = protocol_.id;
    proto["name"] = protocol_.name;
    proto["factory"] = protocol_.factory;
    proto["component"] = protocol_.component;
    retval["protocol"] = proto;
    QJsonObject inproto;
    inproto["id"] = internalProtocol_.id;
    inproto["name"] = internalProtocol_.name;
    inproto["factory"] = internalProtocol_.factory;
    retval["inprotocol"] = inproto;

    QJsonObject tmp;
    QJsonArray arr;
    for (const auto &comm : communications)
    {
        tmp["id"] = comm.id_;
        tmp["name"] = comm.name_;
        arr.push_back(tmp);
    }

    retval["communications"] = arr;

    return retval;
}

void QtNodes::Message::restore(const QJsonObject &p)
{
    name_ = p["name"].toString();
    id_ = p["uuid"].toInt();
    messId_ = p["id"].toInt();
    auto proto = p["protocol"].toObject();
    protocol_.id = proto["id"].toInt();
    protocol_.name = proto["name"].toString();
    protocol_.factory = proto["factory"].toString();
    protocol_.component = proto["component"].toString();
    proto = p["inprotocol"].toObject();
    internalProtocol_.id = proto["id"].toInt();
    internalProtocol_.name = proto["name"].toString();
    internalProtocol_.factory = proto["factory"].toString();

    for (const auto &comm : p["communications"].toArray())
    {
        communications.push_back({comm.toObject()["id"].toInt(), comm.toObject()["name"].toString()});
    }
}
