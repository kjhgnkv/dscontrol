#include "ComponentNode.hpp"

#include "ComponentItemImpl.h"

using namespace QtNodes;

ComponentNode::ComponentNode(NodeImp* dataModel, const QUuid &id, Item::NodeType type)
: Node {dataModel, id, type}
{
}

std::list<Message> ComponentNode::messages(Item::PortType type) const
{
    if (const auto &cast = dynamic_cast<ComponentItemImpl*>(imp_))
    {
        return cast->componentMessages(type);
    }

    return {};
}

const QList<Node*> ComponentNode::childNodesInLowestLevel()
{
    return {this};
}
