#pragma once

#include "NodeIndex.hpp"

#include <memory>

namespace QtNodes
{
/**\brief abstract interface for NodeIterator
 */
class NodeIteratorImpl
{
public:
    virtual ~NodeIteratorImpl() = default;

    virtual NodeIteratorImpl &operator++() = 0;

    /**\brief template method
     */
    NodeIteratorImpl &operator++(int)
    {
        return this->operator++();
    };
    virtual NodeIndex operator*() const = 0;
    virtual NodeIndex operator->() const = 0;
    virtual bool operator==(const NodeIteratorImpl &rhs) const = 0;
    virtual bool operator!=(const NodeIteratorImpl &rhs) const = 0;

    virtual std::unique_ptr<NodeIteratorImpl> copyItSelf() const = 0;
};
} // namespace QtNodes
