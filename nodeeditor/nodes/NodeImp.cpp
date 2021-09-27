#include "NodeImp.hpp"

#include <QDebug>

namespace QtNodes
{
NodeImp::NodeImp(const QString &caption)
: caption_ {caption}
, isInstall_ {false}
{
}

int NodeImp::type() const
{
    return None;
}

QString NodeImp::validationMessage() const
{
    return "";
}

bool NodeImp::isInstall() const
{
    return isInstall_;
}

void NodeImp::setInstall(const bool &isInstall)
{
    isInstall_ = isInstall;
}

QJsonObject NodeImp::save() const
{
    QJsonObject retval;
    retval["install"] = isInstall_;
    return retval;
}

void NodeImp::restore(const QJsonObject &obj)
{
    setInstall(obj["install"].toBool());
}

QPixmap NodeImp::icon() const
{
    return QPixmap();
}

QString NodeImp::caption() const
{
    return caption_;
}

QWidget* NodeImp::embeddedWidget()
{
    return nullptr;
}

void NodeImp::setCaption(const QString &caption)
{
    caption_ = caption;
    emit nodeUpdated();
}
} // namespace QtNodes
