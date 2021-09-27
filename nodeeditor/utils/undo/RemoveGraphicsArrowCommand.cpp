#include "RemoveGraphicsArrowCommand.hpp"

#include "common/ArrowController.h"
#include "graphicsComposites/universalGraphicsObject/UniversalConnectionableItem.h"
#include "models/NodesScene.hpp"
#include "singletones/Facade.hpp"

namespace QtNodes
{
RemoveGraphicsArrowCommand::RemoveGraphicsArrowCommand(const QUuid &sceneId, const QUuid &arrowId, QUndoCommand* parent)
: AbstractSceneCommand(sceneId, parent)
, sourceUid_ {}
, destUid_ {}
, arrowId_ {arrowId}
, controller_ {nullptr}
{
    auto scene = Facade::Instance().sceneModel()->getScene(sceneId_);

    controller_ = scene->arrowController(arrowId_);

    if (controller_)
    {
        sourceUid_ = controller_->source()->id();
        destUid_ = controller_->destination()->id();
    }
}

RemoveGraphicsArrowCommand::~RemoveGraphicsArrowCommand()
{
}

void RemoveGraphicsArrowCommand::undo()
{
    auto scene = Facade::Instance().sceneModel()->getScene(sceneId_);
    auto source = dynamic_cast<UniversalConnectionableItem*>(scene->graphicsItem(sourceUid_));
    auto dest = dynamic_cast<UniversalConnectionableItem*>(scene->graphicsItem(destUid_));

    if (source && dest)
    {
        controller_ = scene->addArrowController(source, dest, nullptr, itemState_);
    }
}

void RemoveGraphicsArrowCommand::redo()
{
    auto scene = Facade::Instance().sceneModel()->getScene(sceneId_);

    if (!controller_)
    {
        controller_ = scene->arrowController(arrowId_);
    }

    if (controller_ && scene)
    {
        itemState_ = controller_->save();
        scene->deleteGraphicsItemCommand(arrowId_);
    }
}
} // namespace QtNodes
