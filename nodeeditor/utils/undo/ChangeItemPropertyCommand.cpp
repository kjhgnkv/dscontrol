#include "ChangeItemPropertyCommand.hpp"

#include "models/NodesScene.hpp"
#include "scenes/FlowScene.hpp"
#include "singletones/Facade.hpp"

#include <QDebug>
#include "graphicsComposites/universalGraphicsObject/UniversalGraphicsItem.h"

namespace QtNodes
{
ChangeItemPropertyCommand::ChangeItemPropertyCommand(const QUuid &scene, const QUuid &item,
                                                     const Item::GraphicsItemProperties &property,
                                                     const QVariant &value, QUndoCommand* parent)
: AbstractSceneCommand(scene, parent)
, itemId_ {item}
, property_ {property}
, value_ {value}
, oldValue_ {}
{
    setText("ChangeItemPropertyCommand");
}

ChangeItemPropertyCommand::~ChangeItemPropertyCommand()
{
}

void ChangeItemPropertyCommand::undo()
{
    qDebug() << "Undo";
    auto scene = Facade::Instance().sceneModel()->getScene(sceneId_);

    if (scene)
    {
        auto item = (scene->graphicsItem(itemId_));
        if (item)
        {
            item->setPropertyCommand(property_, oldValue_);
        }
    }
}

void ChangeItemPropertyCommand::redo()
{
    auto scene = Facade::Instance().sceneModel()->getScene(sceneId_);

    if (scene)
    {
        auto item = (scene->graphicsItem(itemId_));
        if (item)
        {
            if (oldValue_.isNull())
            {
                oldValue_ = item->property(property_);
            }

            item->setPropertyCommand(property_, value_);
        }
    }
}
} // namespace QtNodes
