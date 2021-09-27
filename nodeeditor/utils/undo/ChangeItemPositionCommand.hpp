#pragma once

#include <QPointF>

#include "AbstractSceneCommand.hpp"

namespace QtNodes
{
class ChangeItemPositionCommand : public AbstractSceneCommand
{
public:
    ChangeItemPositionCommand(const QUuid &sceneId, const QUuid &itemId, const QPointF &oldPos, const QPointF &newPos,
                              AbstractSceneCommand* parent = nullptr);
    virtual ~ChangeItemPositionCommand() override;

    void undo() override;
    void redo() override;

private:
    QUuid itemId_;
    QPointF oldPos_;
    QPointF newPos_;
};

} // namespace QtNodes
