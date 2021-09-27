#pragma once

#include "UniversalGraphicsItemImpl.h"
#include "lists/Message.hpp"
#include "nodeeditor/nodeeditor_export.h"
#include "Connection.hpp"

namespace QtNodes
{
class NODEEDITOR_EXPORT ComponentItemImpl : public UniversalGraphicsItemImpl
{
public:
    ComponentItemImpl(std::vector<int> messages, QtNodes::Item::DeploymentType deploymentType,
                      const QString &caption = tr("Component"));

    std::list<QtNodes::Message> componentMessages(const QtNodes::Item::PortType &type);
    void setComponentInfo(const QJsonObject &info);
    void setComponentFilePath(const QString &path);

    QString componentType();

private:
    std::vector<int> messages_;
    QJsonObject info_;
    QString componentType_;
    QString componentFilePath_;
    std::list<QtNodes::Message> messagesIn_;
    std::list<QtNodes::Message> messagesOut_;
    QString brief_;

public:
    QJsonObject save() const;
    void restore(const QJsonObject &p);
};
} // namespace QtNodes
