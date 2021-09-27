// NodeIterator.cpp

#include "nodes/NodeIteratorImpl.hpp"
#include "DataFlowModel.hpp"

namespace QtNodes
{
static NodeIndex invalid;

NodesModel::NodeIterator::NodeIterator(std::unique_ptr<NodeIteratorImpl> impl)
: impl_ {std::move(impl)}
{
}

NodesModel::NodeIterator::NodeIterator()
: impl_ {}
{
}

NodesModel::NodeIterator::~NodeIterator()
{
}

NodesModel::NodeIterator::NodeIterator(const NodeIterator &rhs)
: impl_ {rhs.impl_->copyItSelf()}
{
}

NodesModel::NodeIterator &NodesModel::NodeIterator::operator=(const NodeIterator &rhs)
{
    impl_ = rhs.impl_->copyItSelf();
    return *this;
}

NodesModel::NodeIterator &NodesModel::NodeIterator::operator++()
{
    if (impl_)
    {
        impl_->operator++();
    }
    return *this;
}

NodesModel::NodeIterator &NodesModel::NodeIterator::operator++(int)
{
    if (impl_)
    {
        impl_->operator++();
    }
    return *this;
}

NodeIndex NodesModel::NodeIterator::operator*() const
{
    if (impl_)
    {
        return impl_->operator*();
    }
    return invalid;
}

NodeIndex NodesModel::NodeIterator::operator->() const
{
    if (impl_)
    {
        return impl_->operator->();
    }
    return {};
}

bool NodesModel::NodeIterator::operator==(const NodeIterator &rhs) const
{
    if (impl_ && rhs.impl_)
    {
        return impl_->operator==(*(rhs.impl_));
    }
    if (!impl_ && !rhs.impl_)
    {
        return true;
    }
    return false;
}

bool NodesModel::NodeIterator::operator!=(const NodeIterator &rhs) const
{
    if (impl_ && rhs.impl_)
    {
        return impl_->operator!=(*(rhs.impl_));
    }
    if (!impl_ && !rhs.impl_)
    {
        return false;
    }
    return true;
}

} // namespace QtNodes
