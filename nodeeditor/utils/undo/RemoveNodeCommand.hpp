#pragma once

#include "nodes/NodeIndex.hpp"
#include "utils/undo/AbstractModelCommand.hpp"

namespace QtNodes
{
class DataFlowModel;

class RemoveNodeCommand : public AbstractModelCommand
{
public:
    RemoveNodeCommand(DataFlowModel* item, const NodeIndex &index, AbstractCommand* parent = nullptr);
    virtual ~RemoveNodeCommand() override;

    void undo() override;
    void redo() override;

    NodeIndex index();

    std::pair<bool, std::map<QUuid, std::shared_ptr<Node>>::iterator> value();

private:
    Item::NodeType type_;
    Item::DeploymentType deployType_;
    NodeIndex index_;
    std::pair<bool, std::map<QUuid, std::shared_ptr<Node>>::iterator> value_;
};
} // namespace QtNodes
