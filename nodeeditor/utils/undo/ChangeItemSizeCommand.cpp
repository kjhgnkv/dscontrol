#include "ChangeItemSizeCommand.hpp"

#include "graphicsComposites/universalGraphicsObject/ResizableItem/ResizableItem.h"
#include "graphicsComposites/universalGraphicsObject/ResizableItem/ResizeBox.h"
#include "models/NodesScene.hpp"
#include "singletones/Facade.hpp"

namespace QtNodes
{
ChangeItemSizeCommand::ChangeItemSizeCommand(const QUuid &sceneId, const QUuid &itemId, const QSizeF &oldSize,
                                             const QSizeF &newSize, AbstractSceneCommand* parent)
: AbstractSceneCommand(sceneId, parent)
, itemId_(itemId)
, oldSize_ {oldSize}
, newSize_ {newSize}
{
    setText("ChangeItemSizeCommand");
}

ChangeItemSizeCommand::~ChangeItemSizeCommand()
{
}

void ChangeItemSizeCommand::undo()
{
    auto scene = Facade::Instance().sceneModel()->getScene(sceneId_);

    if (scene)
    {
        auto item = dynamic_cast<ResizableItem*>(scene->graphicsItem(itemId_));
        if (item)
        {
            item->setSize(oldSize_);
        }
    }
}

void ChangeItemSizeCommand::redo()
{
    auto scene = Facade::Instance().sceneModel()->getScene(sceneId_);

    if (scene)
    {
        auto item = dynamic_cast<ResizableItem*>(scene->graphicsItem(itemId_));
        if (item)
        {
            item->setSize(newSize_);
        }
    }
}

} // namespace QtNodes
