#include "styles/StyleCollection.hpp"
#include <utility>

using QtNodes::FlowViewStyle;
using QtNodes::NodeStyle;
using QtNodes::StyleCollection;

NodeStyle const &StyleCollection::nodeStyle()
{
    return instance()._nodeStyle;
}

FlowViewStyle const &StyleCollection::flowViewStyle()
{
    return instance()._flowViewStyle;
}

const StyleCollection &StyleCollection::instance()
{
    static StyleCollection collection;

    return collection;
}

const QtNodes::MiniViewStyle &StyleCollection::miniViewStyle()
{
    return instance().miniViewStyle_;
}
