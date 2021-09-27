#pragma once

#include "UniversalGraphicsItemImpl.h"

namespace QtNodes
{
class ComputerItemImpl : public UniversalGraphicsItemImpl
{
public:
    ComputerItemImpl(const QtNodes::Item::DeploymentType &m_deploymentType, const QString &caption = tr("Computer"));
};
} // namespace QtNodes
