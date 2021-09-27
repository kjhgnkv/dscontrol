#pragma once

#include "AbstractSceneCommand.hpp"
#include <QPointer>

namespace QtNodes
{
class FrameGraphicsItem;
class UniversalConnectionableItem;

class FlowScene;
class ArrowController;

namespace Arrows
{
class AbstractArrow;
}

class AddGraphicsArrowCommand : public AbstractSceneCommand
{
public:
    AddGraphicsArrowCommand(const QUuid &sceneId, const QUuid &source, const QUuid &dest, Arrows::AbstractArrow* arrow,
                            QUndoCommand* parent = nullptr);
    virtual ~AddGraphicsArrowCommand() override;

    void undo() override;
    void redo() override;

    ArrowController* arrowController() const;

private:
    QPointer<Arrows::AbstractArrow> arrow_;
    QUuid sourceUid_;
    QUuid destUid_;
    QUuid arrowId_;
    QPointer<ArrowController> controller_;
};
} // namespace QtNodes
