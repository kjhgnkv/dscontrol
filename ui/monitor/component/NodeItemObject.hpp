#pragma once

#include <QJsonObject>
#include <QString>

namespace monitor
{
class NodeItemObject
{
public:
    QString name_;
    QJsonObject json_;
};
}