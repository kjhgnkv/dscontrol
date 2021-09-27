#pragma once

#include "TreeStyleArrow.h"

namespace QtNodes
{
namespace Arrows
{
class TreeStyleVerticalArrow final : public TreeStyleArrow
{
public:
    TreeStyleVerticalArrow(const QPointF &startPoint = {}, const QPointF &endPoint = {},
                           const QUuid &uuid = QUuid::createUuid(), QGraphicsItem* parentItem = nullptr,
                           QObject* parentObject = nullptr);

    AbstractArrow* clone() const override;

private:
    void synchronizeAxesIntersectionPoints() override;
    void correctMouseOffset(QPointF &offset) override;
    void correctSourceAxesIntersectionPoint() override;
};
} // namespace Arrows
} // namespace QtNodes
