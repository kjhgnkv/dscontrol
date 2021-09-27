#pragma once

#include <QPointF>
#include <QSizeF>

#include "utils/undo/AbstractSceneCommand.hpp"

namespace QtNodes
{
class FrameGraphicsItem;
class FlowScene;

class RemoveGraphicsFrameCommand : public AbstractSceneCommand
{
public:
    RemoveGraphicsFrameCommand(const QUuid &scene, const QUuid &frameId, QUndoCommand* parent = nullptr);
    virtual ~RemoveGraphicsFrameCommand() override;

    void undo() override;
    void redo() override;

private:
    QPointF pos_;
    QSizeF size_;
    QUuid frameId_;
};
} // namespace QtNodes
