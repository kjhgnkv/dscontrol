#pragma once

#include "Annulus.h"

namespace QtNodes
{
namespace GeometricShapes
{
class EdgePoint final : public Annulus
{
Q_OBJECT
public:
    EdgePoint(const QPointF &centerPoint = {}, const QUuid &id = QUuid::createUuid(),
              QGraphicsItem* parentItem = nullptr, QObject* parentObject = nullptr);

    void setPositionMightBeChanged(bool mightBeChanged);

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

signals:
    void positionChange(const QPointF &newPosInSceneCoords, const QPointF &mouseOffset);

private:
    bool positionMightBeChanged_;
    QPointF mouseOffset_;
};
} // namespace GeometricShapes
} // namespace QtNodes

