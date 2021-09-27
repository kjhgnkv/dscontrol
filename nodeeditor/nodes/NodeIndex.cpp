#include "NodeIndex.hpp"

#include "models/NodesModel.hpp"

#include <QJsonObject>

namespace QtNodes
{
NodeIndex::NodeIndex()
: id_ {}
, internalPtr_ {}
, model_ {}
{
}

NodeIndex::NodeIndex(const QUuid &uuid, void* internalPtr, const NodesModel* model)
: id_ {uuid}
, internalPtr_ {internalPtr}
, model_ {model}
{
}

Node* NodeIndex::nodePointer() const
{
    if (model_)
    {
        return model_->nodePointer(*this);
    }
    return {};
}

const NodesModel* NodeIndex::model() const
{
    return model_;
}

QUuid NodeIndex::id() const
{
    return id_;
}

bool NodeIndex::isValid() const
{
    return !id().isNull() && model() != nullptr;
}

QString NodeIndex::nodeCaption() const
{
    if (isValid())
    {
        return model()->nodeCaption(*this);
    }
    return {};
}

QWidget* NodeIndex::nodeWidget() const
{
    if (isValid())
    {
        return model()->nodeWidget(*this);
    }
    return nullptr;
}

void NodeIndex::nodeRestore(const QJsonObject &info) const
{
    if (isValid())
    {
        model()->nodeRestore(*this, info);
    }
}

QJsonObject NodeIndex::nodeSave() const
{
    if (isValid())
    {
        return model()->nodeSave(*this);
    }
    return {};
}

QJsonObject NodeIndex::nodeCopy() const
{
    if (isValid())
    {
        return model()->nodeCopy(*this);
    }
    return {};
}

} // namespace QtNodes
