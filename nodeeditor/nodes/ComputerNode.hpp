#pragma once

#include "Node.hpp"

namespace QtNodes
{
class ComputerNode : public Node
{
public:
    explicit ComputerNode(NodeImp* dataModel, const QUuid &id, Item::NodeType type);

    const QList<Node*> childNodes() const override;
    const QList<Node*> childNodesInLowestLevel() override;
};

} // namespace QtNodes
