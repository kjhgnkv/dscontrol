#include "ComponentItemImpl.h"

#include "common/nodeeditor_globals.hpp"
#include <QDebug>
#include <QJsonArray>

namespace QtNodes
{
ComponentItemImpl::ComponentItemImpl(std::vector<int> messages, QtNodes::Item::DeploymentType deploymentType,
                                     const QString &caption)
: UniversalGraphicsItemImpl {deploymentType, caption}
, messages_ {messages}
{
    type_ = QtNodes::Item::NodeType::ComponentNode;
    viewMode_ = Component;
    minSize_ = {90, 110};
    curMinSize_ = minSize_;
    size_ = minSize_;
    font_.setPointSize(10);
    portWidth_ = 8;
    widgetMargin_ = 8;
    cornerWidth_ = 10;
}

QString ComponentItemImpl::componentType()
{
    return componentType_;
}

std::list<QtNodes::Message> ComponentItemImpl::componentMessages(const QtNodes::Item::PortType &type)
{
    switch (type)
    {
        case QtNodes::Item::PortType::In:
        {
            return messagesIn_;
        }
        case QtNodes::Item::PortType::Out:
        {
            return messagesOut_;
        }
        default:
        {
            return {};
        }
    }
}

void ComponentItemImpl::setComponentFilePath(const QString &path)
{
    componentFilePath_ = path;
}

void ComponentItemImpl::setComponentInfo(const QJsonObject &info)
{
    info_ = info;
    // FIXME rewrite to json array
    auto messagesStrIn = info_["messagesIn"].toArray();
    auto messagesStrOut = info_["messagesOut"].toArray();

    //TODO: remake to new structure
    if (!messagesStrIn.isEmpty())
    {
        for (const auto &i : messagesStrIn)
        {
            QtNodes::Message msg {0};
            msg.messId_ = i.toObject()["id"].toInt();
            msg.name_ = i.toObject()["name"].toString();
            msg.protocol_.name = i.toObject()["protocol"].toString();
            messagesIn_.emplace_back(msg);
        }
    }
    //TODO: remake to new structure
    if (!messagesStrOut.isEmpty())
    {
        for (const auto &i : messagesStrOut)
        {
            QtNodes::Message msg {0};
            msg.messId_ = i.toObject()["id"].toInt();
            msg.name_ = i.toObject()["name"].toString();
            msg.protocol_.name = i.toObject()["protocol"].toString();
            messagesOut_.emplace_back(msg);
        }
    }
    brief_ = info_["brief"].toString();
    componentType_ = info_["componentType"].toString();
    componentFilePath_ = info_["componentFilePath"].toString();
}

QJsonObject ComponentItemImpl::save() const
{
    QJsonObject jsonObj = UniversalGraphicsItemImpl::save();

    QStringList messagesListIn;
    QStringList messagesListOut;

    jsonObj["componentType"] = componentType_;
    jsonObj["componentFilePath"] = componentFilePath_;
    jsonObj["messagesIn"] = info_["messagesIn"].toArray();
    jsonObj["messagesOut"] = info_["messagesOut"].toArray();
    jsonObj["brief"] = brief_;
    return jsonObj;
}

void ComponentItemImpl::restore(const QJsonObject &p)
{
    UniversalGraphicsItemImpl::restore(p);

    setComponentInfo(p);
}
} // namespace QtNodes
