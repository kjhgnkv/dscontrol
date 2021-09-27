#include "SubSystemNode.hpp"

using namespace QtNodes;

SubSystemNode::SubSystemNode(NodeImp* dataModel, const QUuid &id, Item::NodeType type)
: Node {dataModel, id, type}
{
}

const QList<Node*> QtNodes::SubSystemNode::childNodes() const
{
    QList<Node*> retval;

    for (const auto &node : childNodes_)
    {
        if (!retval.contains(node))
        {
            retval.append(node);
        }
    }

    return retval;
}

const QList<Node*> SubSystemNode::childNodesInLowestLevel()
{
    QList<Node*> retval;

    for (auto &node : childNodes_)
    {
        if (!retval.contains(node))
        {
            retval.append(node->childNodesInLowestLevel());
            retval.push_back(node);
        }
    }

    return retval;
}
