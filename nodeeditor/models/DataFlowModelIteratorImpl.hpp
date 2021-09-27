#pragma once

#include "nodes/NodeIndex.hpp"
#include "nodes/NodeIteratorImpl.hpp"
#include <memory>

namespace QtNodes
{
class DataFlowModel;
class Node;

class DataFlowModelIteratorImpl final : public NodeIteratorImpl
{
public:
    DataFlowModelIteratorImpl(const DataFlowModel* model, std::map<QUuid, std::shared_ptr<Node>>::const_iterator it);

    NodeIteratorImpl &operator++();
    NodeIndex operator*() const;
    NodeIndex operator->() const;
    bool operator==(const NodeIteratorImpl &rhs) const;
    bool operator!=(const NodeIteratorImpl &rhs) const;

    std::unique_ptr<NodeIteratorImpl> copyItSelf() const;

private:
    const DataFlowModel* model_;
    std::map<QUuid, std::shared_ptr<Node>>::const_iterator it_;
};
} // namespace QtNodes
