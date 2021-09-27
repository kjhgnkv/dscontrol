#pragma once

#include "FlowViewStyle.hpp"
#include "MiniViewStyle.hpp"
#include "NodeStyle.hpp"
#include <QObject>

namespace QtNodes
{
class StyleCollection
{
public:
    static const StyleCollection &instance();

    static NodeStyle const &nodeStyle();

    static const FlowViewStyle &flowViewStyle();

    static const MiniViewStyle &miniViewStyle();

private:
    StyleCollection() = default;

    StyleCollection(StyleCollection const &) = delete;

    StyleCollection &operator=(StyleCollection const &) = delete;

private:
    NodeStyle _nodeStyle;

    FlowViewStyle _flowViewStyle;

    MiniViewStyle miniViewStyle_;
};
} // namespace QtNodes
