#include "SpecialStyleArrow.h"
#include "ArrowPosition.h"

using namespace QtNodes::Arrows;

SpecialStyleArrow::SpecialStyleArrow(const QPointF &startPoint, const QPointF &endPoint, const QUuid &uuid,
                                     QGraphicsItem* parentItem, QObject* parentObject)
: PolyLineArrow {startPoint, endPoint, uuid, parentItem, parentObject}
, startItemCenterPoint_ {}
, endItemCenterPoint_ {}
{
}

bool SpecialStyleArrow::setLine(const QPolygonF &newLine)
{
    const bool hasSet = PolyLineArrow::setLine(newLine);
    if (!hasSet)
    {
        return false;
    }
    const auto &curTransitPoints = transitPoints();
    if (!curTransitPoints.empty())
    {
        return setAxesIntersectionPoints(transitPoints());
    }
    return hasSet;
}

void SpecialStyleArrow::restoreTransitPoints(const QJsonArray &restoringTransitPoints)
{
    PolyLineArrow::restoreTransitPoints(restoringTransitPoints);
    setAxesIntersectionPoints(transitPoints());
    emit arrowNeedToUpdateGeometry();
}

void SpecialStyleArrow::setEndItemCenterPoint(const QPointF &newEndItemCenterPoint)
{
    endItemCenterPoint_ = newEndItemCenterPoint;
}

void SpecialStyleArrow::setStartItemCenterPoint(const QPointF &newStartItemCenterPoint)
{
    startItemCenterPoint_ = newStartItemCenterPoint;
}

void SpecialStyleArrow::updateGeometry(const QGraphicsItem* startItem, const QGraphicsItem* endItem,
                                       const ArrowPosition* position, UpdateGeometryMode)
{
    if (paramsFineForUpdateGeometry(startItem, endItem, position))
    {
        setStartItemCenterPoint(startItem->mapToScene(startItem->boundingRect().center()));
        setEndItemCenterPoint(endItem->mapToScene(endItem->boundingRect().center()));
        synchronizeAxesIntersectionPoints();
        ArrowPosition newPosition {*position};
        correctPositionBeforeUpdateGeometry(startItem, endItem, &newPosition);
        PolyLineArrow::updateGeometry(startItem, endItem, &newPosition, UpdateGeometryMode::UpdatedBothItems);
    }
}

QPointF SpecialStyleArrow::endItemCenterPoint() const
{
    return endItemCenterPoint_;
}

QPointF SpecialStyleArrow::startItemCenterPoint() const
{
    return startItemCenterPoint_;
}
