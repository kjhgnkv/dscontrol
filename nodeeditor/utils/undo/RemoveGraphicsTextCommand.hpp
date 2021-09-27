#pragma once

#include <QPointF>

#include "utils/undo/AbstractSceneCommand.hpp"

namespace QtNodes
{
class FrameGraphicsItem;
class FlowScene;

class RemoveGraphicsTextCommand : public AbstractSceneCommand
{
public:
    RemoveGraphicsTextCommand(const QUuid &scene, const QUuid &textId, const QUuid &parentId = {},
                              QUndoCommand* parent = nullptr);
    ~RemoveGraphicsTextCommand() override;

    void undo() override;
    void redo() override;

private:
    QPointF pos_;
    QUuid textId_;
    QUuid parentId_;
};
} // namespace QtNodes
