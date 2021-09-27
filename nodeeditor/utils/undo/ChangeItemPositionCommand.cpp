#include "ChangeItemPositionCommand.hpp"

#include "models/NodesScene.hpp"
#include "singletones/Facade.hpp"
//
#include "graphicsComposites/universalGraphicsObject/ResizableItem/ResizableItem.h"
#include "graphicsComposites/universalGraphicsObject/ResizableItem/ResizeBox.h"

namespace QtNodes
{
ChangeItemPositionCommand::ChangeItemPositionCommand(const QUuid &sceneId, const QUuid &itemId, const QPointF &oldPos,
                                                     const QPointF &newPos, QtNodes::AbstractSceneCommand* parent)
: AbstractSceneCommand(sceneId, parent)
, itemId_(itemId)
, oldPos_ {oldPos}
, newPos_ {newPos}
{
    setText("ChangeItemPositionCommand");
}

ChangeItemPositionCommand::~ChangeItemPositionCommand()
{
}

void ChangeItemPositionCommand::undo()
{
    auto sceneModel = Facade::Instance().sceneModel();
    if (nullptr == sceneModel)
    {
        return;
    }
    auto scene = sceneModel->getScene(sceneId_);
    if (nullptr == scene)
    {
        return;
    }
    QGraphicsItem* item = scene->graphicsItem(itemId_);
    if (nullptr == item)
    {
        return;
    }
    item->setPos(oldPos_);
}

void ChangeItemPositionCommand::redo()
{
    auto sceneModel = Facade::Instance().sceneModel();
    if (nullptr == sceneModel)
    {
        return;
    }
    auto scene = sceneModel->getScene(sceneId_);
    if (nullptr == scene)
    {
        return;
    }
    QGraphicsItem* item = scene->graphicsItem(itemId_);
    if (nullptr == dynamic_cast<UniversalMovableItem*>(item))
    {
        return;
    }
    item->setPos(newPos_);
}

} // namespace QtNodes
