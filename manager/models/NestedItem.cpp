#include "NestedItem.hpp"

#include "ProjectModel.hpp"
#include <QColor>
#include <QDebug>

namespace dscontrol
{
NestedItem::NestedItem(ProjectModelItem* parent, const QColor &backColor, const QString &caption,
                       QtNodes::FlowScene* scene)
: caption_ {caption}
, parent_ {parent}
, scene_ {scene}
, backColor_ {backColor}
{
    if (parent_)
    {
        parent_->addChild(this);
    }
}

NestedItem::~NestedItem()
{
}

ProjectModelItem::Type NestedItem::type() const
{
    return Type::NestedItem;
}

ProjectModelItem* NestedItem::parentItem() const
{
    return parent_;
}

const QString &NestedItem::caption() const
{
    return caption_;
}

const QColor &NestedItem::backColor() const
{
    return backColor_;
}

void NestedItem::setCaption(const QString &caption)
{
    caption_ = caption;
}

void NestedItem::setBackColor(const QColor &color)
{
    backColor_ = color;
}

QtNodes::FlowScene* NestedItem::flowScene() const
{
    return scene_;
}

QString NestedItem::mimeCaption()
{
    switch (parent_->type())
    {
        case Type::System:
        {
            return tr("System");
        }
        case Type::SubSystem:
        {
            return tr("Subsystem");
        }
        case Type::Computers:
        {
            return tr("Device");
        }
        case Type::Applications:
        {
            return tr("Application");
        }
        case Type::Component:
        {
            return tr("Component");
        }
        default:
            return {};
    }
}

const QUuid NestedItem::id() const
{
    return id_;
}

void NestedItem::setId(const QUuid &id)
{
    id_ = id;
}

QtNodes::Item::DeploymentType NestedItem::deploymentType() const
{
    return deplType_;
}

void NestedItem::setDeploymentType(const QtNodes::Item::DeploymentType &id)
{
    deplType_ = id;
}
} // namespace dscontrol
