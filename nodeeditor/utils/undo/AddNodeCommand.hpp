#pragma once

#include "nodes/NodeIndex.hpp"
#include "utils/undo/AbstractModelCommand.hpp"

namespace QtNodes
{
class AddNodeCommand : public AbstractModelCommand
{
public:
    AddNodeCommand(DataFlowModel* item, Item::NodeType type, Item::DeploymentType deployType,
                   AbstractCommand* parent = nullptr);

    void undo() override;
    void redo() override;

    NodeIndex index() const;

private:
    const Item::NodeType type_;
    const Item::DeploymentType deployType_;
    NodeIndex index_;
};
} // namespace QtNodes
