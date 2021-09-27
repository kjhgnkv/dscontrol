#include "RemoveGraphicsTextCommand.hpp"

#include "graphicsComposites/universalGraphicsObject/UniversalSelectableItem.h"
#include "graphicsComposites/frame/FrameGraphicsItem.hpp"
#include "models/NodesScene.hpp"
#include "scenes/FlowScene.hpp"
#include "singletones/Facade.hpp"
#include <QDebug>

namespace QtNodes
{
RemoveGraphicsTextCommand::RemoveGraphicsTextCommand(const QUuid &scene, const QUuid &textId, const QUuid &parentId,
                                                     QUndoCommand* parent)
: AbstractSceneCommand(scene, parent)
, pos_ {}
, textId_ {textId}
, parentId_ {parentId}
{
    setText(QObject::tr("Move"));

    auto sc = Facade::Instance().sceneModel()->getScene(sceneId());
    itemState_ = sc->graphicsItem(textId_)->save();
}

RemoveGraphicsTextCommand::~RemoveGraphicsTextCommand()
{
}

void RemoveGraphicsTextCommand::undo()
{
    qDebug() << "Undo";

    auto scene = Facade::Instance().sceneModel()->getScene(sceneId());

    if (scene)
    {
        scene->addTextItem({}, itemState_, parentId_);
    }
}

void RemoveGraphicsTextCommand::redo()
{
    qDebug() << "Redo";

    auto scene = Facade::Instance().sceneModel()->getScene(sceneId());

    scene->deleteGraphicsItemCommand(textId_);
}

} // namespace QtNodes
