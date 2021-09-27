#pragma once

#include "NodeItemObject.hpp"

#include <QJsonObject>

namespace monitor
{
class NodeItemComponent
{
public:
    QString name_;
    QJsonObject json_;

    std::vector<NodeItemObject*> objects_;
};
}
