#pragma once

#include "AbstractArrow.h"
#include "Annulus.h"

namespace QtNodes
{
namespace Arrows
{
class CurvedArrow final : public AbstractArrow
{
public:
    CurvedArrow(const QPointF &startPoint = {}, const QPointF &endPoint = {}, const QUuid &uid = QUuid::createUuid(),
                QGraphicsItem* parentItem = nullptr, QObject* parentObject = nullptr);

    void setControlPointsPosToDefault();

    AbstractArrow* clone() const override;

protected:
    void resetLine() override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

    AnnulusSPtrVector transitPoints() const override;
    QLineF directDirection() const override;
    QLineF reverseDirection() const override;
    QPainterPath paintingLine() const override;
    std::pair<QPointF, QPointF> findIntersectionPoints(const QPointF &newStartPoint, const QPointF &newEndPoint,
                                                       const QPainterPath &startShape,
                                                       const QPainterPath &endShape) const override;

private:
    QJsonArray saveTransitPoints() const override;
    void restoreTransitPoints(const QJsonArray &restoringTransitPoints) override;

    QPainterPath createTinyShape(qreal shapeWidth) const;
    QPointF findStartIntersectionPoint(const QPointF &newStartPoint, const QPainterPath &startShape) const;

    QPointF findEndIntersectionPoint(const QPointF &newEndPoint, const QPainterPath &endShape) const;

    void createControlPoints();
    void inizializeControlPoints();
    void setFirstControlPointPosToDefault(const QPointF &startPointParentPos, const QPointF &centerPoint);
    void setSecondControlPointPosToDefault(const QPointF &endPointParentPos, const QPointF &centerPoint);
    void moveControlPoints(const QPointF &offset);

    QVector<QPointF> findIntersectionPoints(const QPainterPath &path) const;

private:
    const quint8 controlPointsCount_;
    AnnulusSPtrVector controlPoints_;
};
} // namespace Arrows
} // namespace QtNodes
