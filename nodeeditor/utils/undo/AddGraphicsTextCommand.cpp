#include "utils/undo/AddGraphicsTextCommand.hpp"
#include "graphicsComposites/universalGraphicsObject/UniversalSelectableItem.h"
#include "models/NodesScene.hpp"
#include "scenes/FlowScene.hpp"
#include "singletones/Facade.hpp"
#include "graphicsComposites/text/TextGraphicsItem.hpp"
#include <QDebug>

namespace QtNodes
{
AddGraphicsTextCommand::AddGraphicsTextCommand(const QUuid &scene, const QPointF &pos, const QUuid &parentId,
                                               QUndoCommand* parent)
: AbstractSceneCommand(scene, parent)
, pos_ {pos}
, parentId_ {parentId}
{
    setText(QObject::tr("AddGraphicsTextCommand"));
}

AddGraphicsTextCommand::~AddGraphicsTextCommand()
{
}

void AddGraphicsTextCommand::undo()
{
    qDebug() << "Undo";

    auto scene = Facade::Instance().sceneModel()->getScene(sceneId());
    auto text = scene->graphicsItem(value_);

    if (scene && text)
    {
        itemState_ = text->save();

        scene->deleteGraphicsItemCommand(value_);
    }
}

void AddGraphicsTextCommand::redo()
{
    qDebug() << "Redo";

    auto scene = Facade::Instance().sceneModel()->getScene(sceneId());
    value_ = scene->addTextItem(pos_, itemState_, parentId_)->id();
}

} // namespace QtNodes
