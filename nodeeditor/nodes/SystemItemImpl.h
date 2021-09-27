#pragma once

#include "UniversalGraphicsItemImpl.h"
#include <QPixmap>
#include <QString>

namespace QtNodes
{
class SystemItemImpl : public UniversalGraphicsItemImpl
{
public:
    SystemItemImpl(const QtNodes::Item::DeploymentType &m_deploymentType, const QString &caption = tr("System"));
};
} // namespace QtNodes
