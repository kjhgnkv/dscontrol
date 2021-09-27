// ProjectModelItem.cpp

#include "ProjectModelItem.hpp"

#include "models/DataFlowModel.hpp"
#include "models/ModelAdapter.hpp"

#include <QDebug>

namespace dscontrol
{
void ProjectModelItem::setFlowModel(QtNodes::DataFlowModel* model)
{
    Q_UNUSED(model)
}

int ProjectModelItem::rowOfItem(ProjectModelItem* childItem) const
{
    Q_UNUSED(childItem);
    return -1;
}

ProjectModelItem* ProjectModelItem::itemAtRow(int row) const
{
    Q_UNUSED(row);
    return nullptr;
}

void ProjectModelItem::setCaption(const QString &caption)
{
    Q_UNUSED(caption);
}

QString ProjectModelItem::mimeCaption()
{
    return {};
}

bool ProjectModelItem::addChild(ProjectModelItem* child)
{
    Q_UNUSED(child)
    return false;
}

void ProjectModelItem::dataChanged(int row, ProjectModelItem* child)
{
    Q_UNUSED(row);
    Q_UNUSED(child);
}

void ProjectModelItem::rowRemoved(int row, ProjectModelItem* parent)
{
    Q_UNUSED(row);
    Q_UNUSED(parent);
}

void ProjectModelItem::rowInserted(int row, ProjectModelItem* parent)
{
    Q_UNUSED(row);
    Q_UNUSED(parent);
}

QtNodes::FlowScene* ProjectModelItem::flowScene() const
{
    return nullptr;
}

void ProjectModelItem::removeItemAtRow(int row)
{
    Q_UNUSED(row);
}

const QUuid ProjectModelItem::id() const
{
    return {};
}

void ProjectModelItem::setId(const QUuid &id)
{
    Q_UNUSED(id);
}

QtNodes::Item::DeploymentType ProjectModelItem::deploymentType() const
{
    return {};
}

void ProjectModelItem::setDeploymentType(const QtNodes::Item::DeploymentType &id)
{
    Q_UNUSED(id);
}

} // namespace dscontrol
