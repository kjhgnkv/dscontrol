#include "AddGraphicsArrowCommand.hpp"

#include "common/ArrowController.h"
#include "graphicsComposites/universalGraphicsObject/UniversalConnectionableItem.h"
#include "graphicsComposites/universalGraphicsObject/UniversalGraphicsItem.h"
#include "arrows/AbstractArrow.h"
#include "models/NodesScene.hpp"
#include "singletones/Facade.hpp"

#include <QDebug>

namespace QtNodes
{
AddGraphicsArrowCommand::AddGraphicsArrowCommand(const QUuid &sceneId, const QUuid &source, const QUuid &dest,
                                                 Arrows::AbstractArrow* arrow, QUndoCommand* parent)
: AbstractSceneCommand(sceneId, parent)
, arrow_ {arrow}
, sourceUid_ {source}
, destUid_ {dest}
, arrowId_ {}
, controller_ {nullptr}
{
    setText(QObject::tr("AddGraphicsArrowCommand"));

    if (arrow_)
    {
        arrowId_ = arrow_->id();
    }
}

AddGraphicsArrowCommand::~AddGraphicsArrowCommand()
{
}

void AddGraphicsArrowCommand::undo()
{
    qDebug() << "Undo";

    auto scene = Facade::Instance().sceneModel()->getScene(sceneId());

    if (!controller_)
    {
        if (scene)
            controller_ = scene->arrowController(arrowId_);
    }

    if (controller_ && scene)
    {
        itemState_ = controller_->save();

        scene->deleteGraphicsItemCommand(controller_->arrow());
    }
}

void AddGraphicsArrowCommand::redo()
{
    qDebug() << "Redo";

    auto scene = Facade::Instance().sceneModel()->getScene(sceneId());
    auto source = dynamic_cast<UniversalConnectionableItem*>(scene->graphicsItem(sourceUid_));
    auto dest = dynamic_cast<UniversalConnectionableItem*>(scene->graphicsItem(destUid_));

    if (source && dest)
    {
        controller_ = scene->addArrowController(source, dest, arrow_, itemState_);
        if (controller_)
        {
            arrow_ = controller_->arrow();
        }
    }
}

ArrowController* AddGraphicsArrowCommand::arrowController() const
{
    return controller_;
}

} // namespace QtNodes
