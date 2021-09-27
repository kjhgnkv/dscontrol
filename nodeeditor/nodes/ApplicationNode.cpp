#include "ApplicationNode.hpp"

using namespace QtNodes;

QtNodes::ApplicationNode::ApplicationNode(NodeImp* dataModel, const QUuid &id, Item::NodeType type)
: Node {dataModel, id, type}
{
}

const QList<Node*> QtNodes::ApplicationNode::childNodes() const
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

const QList<Node*> ApplicationNode::childNodesInLowestLevel()
{
    return childNodes();
}
