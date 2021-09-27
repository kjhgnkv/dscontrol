#pragma once

#include "Node.hpp"

namespace QtNodes
{
class SystemNode : public Node
{
public:
    explicit SystemNode(NodeImp* dataModel, QUuid const &id, Item::NodeType type, Item::DeploymentType dtype);

    const QList<Node*> childNodes() const override;
    const QList<Node*> childNodesInLowestLevel() override;
    void setParentNode(Node* parent) override;
};

} // namespace QtNodes
