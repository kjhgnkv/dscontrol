#pragma once

#include "common/nodeeditor_globals.hpp"
#include "utils/undo/AbstractSceneCommand.hpp"

namespace QtNodes
{
class ChangeItemPropertyCommand : public AbstractSceneCommand
{
public:
    ChangeItemPropertyCommand(const QUuid &scene, const QUuid &item, const Item::GraphicsItemProperties &property,
                              const QVariant &value, QUndoCommand* parent = nullptr);
    ~ChangeItemPropertyCommand();

    void undo() override;
    void redo() override;

private:
    QUuid itemId_;
    Item::GraphicsItemProperties property_;
    QVariant value_;
    QVariant oldValue_;
};

} // namespace QtNodes
