#include "NodesModel.hpp"

#include "nodes/NodeIndex.hpp"

namespace QtNodes
{
NodesModel::NodesModel(QObject* parent)
: QObject {parent}
{
}

NodesModel::~NodesModel()
{
    emit modelDeleted();
}

NodeIndex NodesModel::createIndex(const QUuid &id, void* internalPointer) const
{
    return NodeIndex(id, internalPointer, this);
}

NodeIndex NodesModel::createNotValidIndex(const QUuid &id) const
{
    return NodeIndex(id, nullptr, nullptr);
}

bool NodesModel::checkIndex(const NodeIndex &index) const
{
    return (index.isValid() && index.model() == this) ? true : false;
}

} // namespace QtNodes
