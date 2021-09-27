#pragma once

#include "utils/undo/AbstractSceneCommand.hpp"
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

class RemoveGraphicsArrowCommand : public AbstractSceneCommand
{
public:
    RemoveGraphicsArrowCommand(const QUuid &sceneId, const QUuid &arrowId, QUndoCommand* parent = nullptr);
    virtual ~RemoveGraphicsArrowCommand() override;

    void undo() override;
    void redo() override;

private:
    QUuid sourceUid_;
    QUuid destUid_;
    QUuid arrowId_;
    QPointer<ArrowController> controller_;
};
} // namespace QtNodes
