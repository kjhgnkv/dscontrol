#pragma once

#include "UniversalGraphicsItemImpl.h"
#include "nodeeditor/nodeeditor_export.h"

namespace QtNodes
{
class NODEEDITOR_EXPORT SubSystemItemImpl : public UniversalGraphicsItemImpl
{
public:
    SubSystemItemImpl(const QtNodes::Item::DeploymentType &m_deploymentType, const QString &caption = tr("SubSystem"));
};
} // namespace QtNodes
