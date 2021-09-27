#pragma once

#include "PolyLineArrow.h"

namespace QtNodes
{
namespace Arrows
{
class SpecialStyleArrow : public PolyLineArrow
{
protected:
    SpecialStyleArrow(const QPointF &startPoint = {}, const QPointF &endPoint = {},
                      const QUuid &uuid = QUuid::createUuid(), QGraphicsItem* parentItem = nullptr,
                      QObject* parentObject = nullptr);

public:
    void setEndItemCenterPoint(const QPointF &newEndItemCenterPoint);
    void setStartItemCenterPoint(const QPointF &newStartItemCenterPoint);

    bool setLine(const QPolygonF &newLine) override final;
    void updateGeometry(const QGraphicsItem* startItem, const QGraphicsItem* endItem, const ArrowPosition* position,
                        UpdateGeometryMode mode) override;

protected:
    QPointF endItemCenterPoint() const;
    QPointF startItemCenterPoint() const;

    virtual void synchronizeAxesIntersectionPoints() = 0;

private:
    void restoreTransitPoints(const QJsonArray &restoringTransitPoints) override final;
    virtual bool setAxesIntersectionPoints(AnnulusSPtrVector newAxesIntersectionPoints) = 0;
    virtual void correctPositionBeforeUpdateGeometry(const QGraphicsItem* startItem, const QGraphicsItem* endItem,
                                                     ArrowPosition* newArrowPosition) const = 0;
private:
    QPointF startItemCenterPoint_; ///< The position of source item's center point in scene coordinates.
    QPointF endItemCenterPoint_;   ///< The position of destination item's center point in scene coordinates.
};
} // namespace Arrows
} // namespace QtNodes
