#pragma once

#include <QSizeF>

#include "AbstractSceneCommand.hpp"

namespace QtNodes
{
class ChangeItemSizeCommand : public AbstractSceneCommand
{
public:
    ChangeItemSizeCommand(const QUuid &sceneId, const QUuid &itemId, const QSizeF &oldSize, const QSizeF &newSize,
                          AbstractSceneCommand* parent = nullptr);
    virtual ~ChangeItemSizeCommand() override;

    void undo() override;
    void redo() override;

private:
    QUuid itemId_;
    QSizeF oldSize_;
    QSizeF newSize_;
};

} // namespace QtNodes
