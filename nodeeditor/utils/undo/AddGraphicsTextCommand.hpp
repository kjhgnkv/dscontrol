#pragma once

#include <QPointF>

#include "utils/undo/AbstractSceneCommand.hpp"

namespace QtNodes
{
class FlowScene;
class TextGraphicsItem;

class AddGraphicsTextCommand : public AbstractSceneCommand
{
public:
    AddGraphicsTextCommand(const QUuid &scene, const QPointF &pos, const QUuid &parentId = QUuid {},
                           QUndoCommand* parent = nullptr);
    virtual ~AddGraphicsTextCommand() override;

    void undo() override;
    void redo() override;

private:
    QPointF pos_;
    QUuid parentId_;
};
} // namespace QtNodes
