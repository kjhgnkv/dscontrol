#pragma once

#include "UniversalGraphicsItemImpl.h"
#include "nodeeditor/nodeeditor_export.h"

namespace QtNodes
{
class NODEEDITOR_EXPORT ApplicationItemImpl : public UniversalGraphicsItemImpl
{
public:
    ApplicationItemImpl(QtNodes::Item::DeploymentType m_deploymentType, const QString &caption = tr("Application"));
};
} // namespace QtNodes
