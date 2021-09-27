#include "AddGraphicsFrameCommand.hpp"

#include "graphicsComposites/universalGraphicsObject/UniversalSelectableItem.h"
#include "graphicsComposites/frame/FrameGraphicsItem.hpp"
#include "models/NodesScene.hpp"
#include "scenes/FlowScene.hpp"
#include "singletones/Facade.hpp"

#include <QDebug>

namespace QtNodes
{
AddGraphicsFrameCommand::AddGraphicsFrameCommand(const QUuid &scene, const QPointF &pos, const QSizeF &size,
                                                 QUndoCommand* parent)
: AbstractSceneCommand(scene, parent)
, pos_ {pos}
, size_ {size}
{
    setText(QObject::tr("Move"));
}

AddGraphicsFrameCommand::~AddGraphicsFrameCommand()
{
}

void AddGraphicsFrameCommand::undo()
{
    qDebug() << "Undo";

    auto scene = Facade::Instance().sceneModel()->getScene(sceneId());
    auto frame = scene->graphicsItem(value_);

    if (frame)
    {
        itemState_ = frame->save();

        scene->deleteGraphicsItemCommand(value_);
    }
}

void AddGraphicsFrameCommand::redo()
{
    qDebug() << "Redo";

    auto scene = Facade::Instance().sceneModel()->getScene(sceneId());

    value_ = scene->addGraphicsFrame(pos_, size_, itemState_)->id();
}

} // namespace QtNodes
