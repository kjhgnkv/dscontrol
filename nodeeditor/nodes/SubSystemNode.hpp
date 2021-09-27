#pragma once

#include "Node.hpp"

namespace QtNodes
{
class SubSystemNode : public Node
{
public:
    explicit SubSystemNode(NodeImp* dataModel, const QUuid &id, Item::NodeType type);

    const QList<Node*> childNodes() const override;
    const QList<Node*> childNodesInLowestLevel() override;
};

} // namespace QtNodes
