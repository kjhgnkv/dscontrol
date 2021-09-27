#pragma once

#include <QPointF>
#include <QSizeF>

#include "AbstractSceneCommand.hpp"

namespace QtNodes
{
class FrameGraphicsItem;
class FlowScene;

class AddGraphicsFrameCommand : public AbstractSceneCommand
{
public:
    AddGraphicsFrameCommand(const QUuid &scene, const QPointF &pos, const QSizeF &size, QUndoCommand* parent = nullptr);
    virtual ~AddGraphicsFrameCommand() override;

    void undo() override;
    void redo() override;

private:
    QPointF pos_;
    QSizeF size_;
};
} // namespace QtNodes
