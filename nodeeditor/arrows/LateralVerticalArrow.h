#pragma once

#include "LateralArrow.h"

namespace QtNodes
{
namespace Arrows
{
class LateralVerticalArrow final : public LateralArrow
{
public:
    LateralVerticalArrow(const QPointF &startPoint = {}, const QPointF &endPoint = {},
                         const QUuid &uuid = QUuid::createUuid(), QGraphicsItem* parentItem = nullptr,
                         QObject* parentObject = nullptr);

    AbstractArrow* clone() const override;

private:
    void synchronizeAxesIntersectionPoints() override;
};
} // namespace Arrows
} // namespace QtNodes
