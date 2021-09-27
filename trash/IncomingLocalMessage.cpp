#include "IncomingLocalMessage.hpp"
#include <QJsonDocument>
#include <QJsonObject>

IncomingLocalMessage::IncomingLocalMessage(const std::string &msgType, const std::string &block,
                                           const std::string &appType, const std::string &error, int errorId,
                                           const std::string &data, const std::string &json)
: appType_ {appType}
, block_ {block}
, msgType_ {msgType}
, error_ {error}
, errorId_ {errorId}
, data_ {data}
, json_ {json}
{
}

const std::string IncomingLocalMessage::appType() const
{
    return appType_;
}

const std::string IncomingLocalMessage::block() const
{
    return block_;
}

const std::string IncomingLocalMessage::msgType() const
{
    return msgType_;
}

const std::string IncomingLocalMessage::error() const
{
    return error_;
}

int IncomingLocalMessage::errorId() const
{
    return errorId_;
}

const std::string IncomingLocalMessage::data() const
{
    return data_;
}

const std::string IncomingLocalMessage::json() const
{
    return json_;
}

bool IncomingLocalMessage::parse(const std::string &json, std::shared_ptr<IncomingLocalMessage> &msg)
{
    QJsonDocument doc = QJsonDocument::fromJson(QString::fromStdString(json).toUtf8());
    if (doc.isNull() || !doc.isObject())
    {
        return false;
    }

    QJsonObject obj = doc.object();

    std::string block {};
    std::string msgType = obj["msgType"].toString().toStdString();
    if (!msgType.empty())
    {
        block = obj["block"].toString().toStdString();
    }
    std::string appType = obj["appType"].toString().toStdString();
    std::string error = obj["error"].toString().toStdString();
    int errorId = obj["errorId"].toInt();
    std::string data = obj["data"].toString().toStdString();

    msg.reset(new IncomingLocalMessage {msgType, block, appType, error, errorId, data, json});

    return true;
}
