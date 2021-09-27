#include "RemoveGraphicsFrameCommand.hpp"

#include "graphicsComposites/universalGraphicsObject/UniversalSelectableItem.h"
#include "graphicsComposites/frame/FrameGraphicsItem.hpp"
#include "models/NodesScene.hpp"
#include "scenes/FlowScene.hpp"
#include "singletones/Facade.hpp"
#include <QDebug>

namespace QtNodes
{
RemoveGraphicsFrameCommand::RemoveGraphicsFrameCommand(const QUuid &scene, const QUuid &frameId, QUndoCommand* parent)
: AbstractSceneCommand(scene, parent)
, pos_ {}
, size_ {}
, frameId_ {frameId}
{
    setText(QObject::tr("Move"));

    auto sc = Facade::Instance().sceneModel()->getScene(sceneId());
    itemState_ = sc->graphicsItem(frameId_)->save();
}

RemoveGraphicsFrameCommand::~RemoveGraphicsFrameCommand()
{
}

void RemoveGraphicsFrameCommand::undo()
{
    qDebug() << "Undo";

    auto scene = Facade::Instance().sceneModel()->getScene(sceneId());

    if (scene)
    {
        scene->addGraphicsFrame({}, {}, itemState_);
    }
}

void RemoveGraphicsFrameCommand::redo()
{
    qDebug() << "Redo";

    auto scene = Facade::Instance().sceneModel()->getScene(sceneId());

    scene->deleteGraphicsItemCommand(frameId_);
}

} // namespace QtNodes
