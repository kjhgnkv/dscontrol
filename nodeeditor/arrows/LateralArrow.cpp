#include "LateralArrow.h"
#include "EdgePoint.h"
#include "ArrowPosition.h"

using namespace QtNodes::Arrows;
using namespace QtNodes::GeometricShapes;

LateralArrow::LateralArrow(const QPointF &startPoint, const QPointF &endPoint, const QUuid &uuid,
                           QGraphicsItem* parentItem, QObject* parentObject)
: SpecialStyleArrow {startPoint, endPoint, uuid, parentItem, parentObject}
, axesIntersectionPoint_ {std::make_shared<GeometricShapes::Annulus>()}
{
    setMaxTransitPointsCount(1);
    axesIntersectionPoint_->setUndoRedoPositionEnable(false);
    axesIntersectionPoint_->setParentItem(this);
    axesIntersectionPoint_->setMovable(false);
    axesIntersectionPoint_->setFlag(ItemIsMovable, false);

    pushTransitPoint(axesIntersectionPoint_);

    EdgePointSPtr startSmart = smartStartPoint();
    if (nullptr != startSmart)
    {
        startSmart->setFlag(ItemIsMovable, false);
    }
    EdgePointSPtr endSmart = smartEndPoint();
    if (nullptr != endSmart)
    {
        endSmart->setFlag(ItemIsMovable, false);
    }

    setSmartPointRadius(axesIntersectionPoint_, smartPointsRadius());
}

void LateralArrow::setAxesIntersectionPoint(const QPointF &newAxesIntersectionPoint)
{
    if (nullptr != axesIntersectionPoint_)
    {
        axesIntersectionPoint_->setPos(newAxesIntersectionPoint);
    }
}

void LateralArrow::setMaxTransitPointsCount(quint8 newMaxTransitPointsCount)
{
    SpecialStyleArrow::setMaxTransitPointsCount(newMaxTransitPointsCount);
}

void LateralArrow::updateGeometry(const QGraphicsItem* startItem, const QGraphicsItem* endItem,
                                  const ArrowPosition* position, UpdateGeometryMode mode)
{
    return SpecialStyleArrow::updateGeometry(startItem, endItem, position, mode);
}

bool LateralArrow::setAxesIntersectionPoints(AnnulusSPtrVector newAxesIntersectionPoints)
{
    if (newAxesIntersectionPoints.size() != maxTransitPointsCount())
    {
        return false;
    }
    AnnulusSPtr newAxesIntersectionPoint = newAxesIntersectionPoints.front();
    if (newAxesIntersectionPoint == axesIntersectionPoint_)
    {
        return false;
    }
    axesIntersectionPoint_ = std::move(newAxesIntersectionPoint);
    synchronizeAxesIntersectionPoints();
    return true;
}

void LateralArrow::correctPositionBeforeUpdateGeometry(const QGraphicsItem* startItem, const QGraphicsItem* endItem,
                                                       ArrowPosition* newArrowPosition) const
{
    if (nullptr == axesIntersectionPoint_ || !paramsFineForUpdateGeometry(startItem, endItem, newArrowPosition))
    {
        return;
    }

    const QPointF &axesIntersectionPointSrcPos = startItem->mapFromItem(this, axesIntersectionPoint_->pos());
    const QPointF &axesIntersectionPointDestPos = endItem->mapFromItem(this, axesIntersectionPoint_->pos());
    const bool srcContains = startItem->contains(axesIntersectionPointSrcPos);
    const bool destContains = endItem->contains(axesIntersectionPointDestPos);
    const bool newEnabled = !srcContains && !destContains;
    axesIntersectionPoint_->setEnabled(newEnabled);

    if (srcContains)
    {
        newArrowPosition->currentStartPos = axesIntersectionPointSrcPos;
    }
    if (destContains)
    {
        newArrowPosition->currentEndPos = axesIntersectionPointDestPos;
    }
}
