#pragma once

#include "SpecialStyleArrow.h"

namespace QtNodes
{
namespace Arrows
{
class LateralArrow : public SpecialStyleArrow
{
protected:
    LateralArrow(const QPointF &startPoint = {}, const QPointF &endPoint = {}, const QUuid &uuid = QUuid::createUuid(),
                 QGraphicsItem* parentItem = nullptr, QObject* parentObject = nullptr);

public:
    void updateGeometry(const QGraphicsItem* startItem, const QGraphicsItem* endItem, const ArrowPosition* position,
                        UpdateGeometryMode mode) override final;

protected:
    void setAxesIntersectionPoint(const QPointF &newAxesIntersectionPoint);

private:
    void setMaxTransitPointsCount(quint8 newMaxTransitPointsCount) override final;
    bool setAxesIntersectionPoints(AnnulusSPtrVector newAxesIntersectionPoints) override final;
    void correctPositionBeforeUpdateGeometry(const QGraphicsItem* startItem, const QGraphicsItem* endItem,
                                             ArrowPosition* newArrowPosition) const override final;
private:
    AnnulusSPtr axesIntersectionPoint_;
};
} // namespace Arrows
} // namespace QtNodes
