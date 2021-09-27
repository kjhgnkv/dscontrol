#pragma once

#include "Node.hpp"
#include "lists/Message.hpp"

namespace QtNodes
{
class ComponentNode : public Node
{
Q_OBJECT
    // friend Sender;
public:
    explicit ComponentNode(NodeImp* dataModel, QUuid const &id, Item::NodeType type);

    std::list<Message> messages(Item::PortType type) const override;
    const QList<Node*> childNodesInLowestLevel() override;
};

} // namespace QtNodes
