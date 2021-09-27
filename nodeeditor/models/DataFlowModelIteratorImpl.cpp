#include "DataFlowModelIteratorImpl.hpp"
#include "DataFlowModel.hpp"

namespace QtNodes
{
DataFlowModelIteratorImpl::DataFlowModelIteratorImpl(const DataFlowModel* model,
                                                     std::map<QUuid, std::shared_ptr<Node>>::const_iterator it)
: model_ {model}
, it_ {it}
{
}

NodeIteratorImpl &DataFlowModelIteratorImpl::operator++()
{
    ++it_;
    return *this;
}

NodeIndex DataFlowModelIteratorImpl::operator*() const
{
    if (model_)
    {
        return model_->nodeIndex(it_->first);
    }
    return {};
}

NodeIndex DataFlowModelIteratorImpl::operator->() const
{
    if (model_)
    {
        return model_->nodeIndex(it_->first);
    }
    return {};
}

bool DataFlowModelIteratorImpl::operator==(const NodeIteratorImpl &rhs) const
{
    try
    {
        const auto &rIt = dynamic_cast<const DataFlowModelIteratorImpl &>(rhs);
        return (model_ == rIt.model_ && it_ == rIt.it_) ? true : false;
    }
    catch (std::bad_cast &)
    {
    }
    return false;
}

bool DataFlowModelIteratorImpl::operator!=(const NodeIteratorImpl &rhs) const
{
    try
    {
        const auto &rIt = dynamic_cast<const DataFlowModelIteratorImpl &>(rhs);
        return (model_ != rIt.model_ || it_ != rIt.it_) ? true : false;
    }
    catch (std::bad_cast &)
    {
    }
    return true;
}

std::unique_ptr<NodeIteratorImpl> DataFlowModelIteratorImpl::copyItSelf() const
{
    return std::make_unique<DataFlowModelIteratorImpl>(*this);
}
} // namespace QtNodes
