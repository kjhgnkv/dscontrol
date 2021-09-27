#include "SystemNode.hpp"

using namespace QtNodes;

QtNodes::SystemNode::SystemNode(NodeImp* dataModel, const QUuid &id, Item::NodeType type, Item::DeploymentType dtype)
: Node {dataModel, id, type}
{
    deploymentType_ = {dtype};
}

const QList<Node*> QtNodes::SystemNode::childNodes() const
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

void SystemNode::setParentNode(Node*)
{
    parentNode_ = (nullptr);
}

const QList<Node*> SystemNode::childNodesInLowestLevel()
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
