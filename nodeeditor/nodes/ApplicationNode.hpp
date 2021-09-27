#pragma once

#include "Node.hpp"

namespace QtNodes
{
class ApplicationNode : public Node
{
public:
    explicit ApplicationNode(NodeImp* dataModel, const QUuid &id, Item::NodeType type);

    const QList<Node*> childNodes() const override;
    const QList<Node*> childNodesInLowestLevel() override;
};

} // namespace QtNodes
