#pragma once

#include "SpecialStyleArrow.h"

namespace QtNodes
{
namespace Arrows
{
class TreeStyleArrow : public SpecialStyleArrow
{
protected:
    TreeStyleArrow(const QPointF &startPoint = {}, const QPointF &endPoint = {},
                   const QUuid &uuid = QUuid::createUuid(), QGraphicsItem* parentItem = nullptr,
                   QObject* parentObject = nullptr);

public:
    void updateGeometry(const QGraphicsItem* startItem, const QGraphicsItem* endItem, const ArrowPosition* position,
                        UpdateGeometryMode mode) override final;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    bool setDestAxesIntersectionPoint(const QPointF &newPoint);
    bool setSrcAxesIntersectionPoint(const QPointF &newPoint);
    void setAxesIntersectionPoints(const QPointF &newSourcePoint, const QPointF &newDestPoint);

    qreal segmentLength() const;
    QRectF srcBoundingRect() const;
    QRectF destBoundingRect() const;
    QPointF srcAxesIntersectionPoint() const;
    QPointF destAxesIntersectionPoint() const;
    QPointF newStartScenePos() const;

private:
    bool isEachAxesIntersectionPointOk() const;
    void setMaxTransitPointsCount(quint8 newMaxTransitPointsCount) override final;
    bool setAxesIntersectionPoints(AnnulusSPtrVector newAxesIntersectionPoints) override final;
    void correctPositionBeforeUpdateGeometry(const QGraphicsItem* startItem, const QGraphicsItem* endItem,
                                             ArrowPosition* newArrowPosition) const override final;

    virtual void correctMouseOffset(QPointF &offset) = 0;
    virtual void correctSourceAxesIntersectionPoint() = 0;

private:
    AnnulusSPtr srcAxesIntersectionPoint_;
    AnnulusSPtr destAxesIntersectionPoint_;
    QPointF newStartScenePos_;
    QRectF srcBoundingRect_;
    QRectF destBoundingRect_;
    bool isMoving_;
    const qreal segmentLength_;
};
} // namespace Arrows
} // namespace QtNodes
