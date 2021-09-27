#include "ComputerNode.hpp"

using namespace QtNodes;

ComputerNode::ComputerNode(NodeImp* dataModel, const QUuid &id, Item::NodeType type)
: Node {dataModel, id, type}
{
}

const QList<Node*> QtNodes::ComputerNode::childNodes() const
{
    QList<Node*> retval;

    for (auto &node : childNodes_)
    {
        if (!retval.contains(node))
        {
            retval.append(node);
        }
    }

    return retval;
}

const QList<Node*> ComputerNode::childNodesInLowestLevel()
{
    QList<Node*> retval;

    for (auto &node : childNodes_)
    {
        if (!retval.contains(node))
        {
            retval.append(node->childNodesInLowestLevel());
        }
        retval.push_back(node);
    }

    return retval;
}
