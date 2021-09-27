#include "TreeStyleArrow.h"
#include "EdgePoint.h"
#include "ArrowPosition.h"

#include <QGraphicsSceneMouseEvent>
#include <QDebug>

using namespace QtNodes::Arrows;
using namespace QtNodes::GeometricShapes;

TreeStyleArrow::TreeStyleArrow(const QPointF &startPoint, const QPointF &endPoint, const QUuid &uuid,
                               QGraphicsItem* parentItem, QObject* parentObject)
: SpecialStyleArrow {startPoint, endPoint, uuid, parentItem, parentObject}
, srcAxesIntersectionPoint_ {std::make_shared<GeometricShapes::Annulus>()}
, destAxesIntersectionPoint_ {std::make_shared<GeometricShapes::Annulus>()}
, newStartScenePos_ {}
, isMoving_ {false}
, segmentLength_ {50.0}
{
    setMaxTransitPointsCount(2);
    pushTransitPoint(srcAxesIntersectionPoint_);
    pushTransitPoint(destAxesIntersectionPoint_);
    setMovable(false);
    for (auto axesIntersectionPoint : transitPoints())
    {
        axesIntersectionPoint->setUndoRedoPositionEnable(false);
        axesIntersectionPoint->setFlag(ItemIsMovable, false);
        axesIntersectionPoint->setMovable(false);
    }
    EdgePointSPtr startSmart = smartStartPoint();
    if (nullptr != startSmart)
    {
        startSmart->setFlag(ItemIsMovable, false);
        newStartScenePos_ = startSmart->scenePos();
    }
    EdgePointSPtr endSmart = smartEndPoint();
    if (nullptr != endSmart)
    {
        endSmart->setFlag(ItemIsMovable, false);
    }
}

void TreeStyleArrow::updateGeometry(const QGraphicsItem* startItem, const QGraphicsItem* endItem,
                                    const ArrowPosition* position, UpdateGeometryMode mode)
{
    if (paramsFineForUpdateGeometry(startItem, endItem, position) && isEachAxesIntersectionPointOk())
    {
        srcBoundingRect_ = startItem->mapToScene(startItem->boundingRect()).boundingRect();
        destBoundingRect_ = endItem->mapToScene(endItem->boundingRect()).boundingRect();
        newStartScenePos_ = startItem->mapToScene(position->lastStartPos);
        SpecialStyleArrow::updateGeometry(startItem, endItem, position, mode);
        correctSourceAxesIntersectionPoint();
    }
}

void TreeStyleArrow::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (nullptr == event)
    {
        return;
    }
    if (Qt::LeftButton != event->button() || nullptr == srcAxesIntersectionPoint_)
    {
        return SpecialStyleArrow::mousePressEvent(event);
    }

    const EdgePointSPtr &start = smartStartPoint();
    const QPointF &eventScenePos = event->scenePos();
    if (nullptr != start && start->contains(start->mapFromScene(eventScenePos)))
    {
        return SpecialStyleArrow::mousePressEvent(event);
    }

    const auto &smartTransitPoints = transitPoints();
    if (smartTransitPoints.empty())
    {
        return SpecialStyleArrow::mousePressEvent(event);
    }

    const AnnulusSPtr &firstTransitPoint = smartTransitPoints.front();
    if (nullptr != firstTransitPoint && firstTransitPoint->contains(firstTransitPoint->mapFromScene(eventScenePos)))
    {
        return SpecialStyleArrow::mousePressEvent(event);
    }

    const QPointF &itemClickPos = event->pos();
    auto minProjectionPoint = minProjectionsPoint(itemClickPos);

    if (nullptr != minProjectionPoint.first)
    {
        delete minProjectionPoint.first;
        minProjectionPoint.first = nullptr;
    }

    const quint8 startPointIndex {0};
    isMoving_ = minProjectionPoint.second == startPointIndex;

    return SpecialStyleArrow::mousePressEvent(event);
}

void TreeStyleArrow::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (nullptr == event)
    {
        return;
    }
    if (!isMoving_ || nullptr == srcAxesIntersectionPoint_)
    {
        return SpecialStyleArrow::mouseMoveEvent(event);
    }

    QPointF mouseOffset = event->pos() - event->lastPos();
    correctMouseOffset(mouseOffset);
    const QPointF &oldStartPoint = startPoint();
    emit startPointPositionChange(oldStartPoint + mouseOffset, mouseOffset);
    if (oldStartPoint != startPoint())
    {
        correctSourceAxesIntersectionPoint();
    }
    SpecialStyleArrow::mouseMoveEvent(event);
}

void TreeStyleArrow::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    isMoving_ = {false};
    SpecialStyleArrow::mouseReleaseEvent(event);
}

bool TreeStyleArrow::setDestAxesIntersectionPoint(const QPointF &newPoint)
{
    if (nullptr != destAxesIntersectionPoint_)
    {
        destAxesIntersectionPoint_->setPos(newPoint);
        synchronize();
        return true;
    }
    return false;
}

bool TreeStyleArrow::setSrcAxesIntersectionPoint(const QPointF &newPoint)
{
    if (nullptr != srcAxesIntersectionPoint_)
    {
        srcAxesIntersectionPoint_->setPos(newPoint);
        synchronize();
        return true;
    }
    return false;
}

void TreeStyleArrow::setAxesIntersectionPoints(const QPointF &newSourcePoint, const QPointF &newDestPoint)
{
    const bool previousNeedToBeUpdated = needToBeUpdated_;
    needToBeUpdated_ = {false};

    bool isUpdated = setSrcAxesIntersectionPoint(newSourcePoint);
    isUpdated = setDestAxesIntersectionPoint(newDestPoint) || isUpdated;
    if (isUpdated)
    {
        needToBeUpdated_ = {true};
        synchronize();
    }

    needToBeUpdated_ = previousNeedToBeUpdated;
}

qreal TreeStyleArrow::segmentLength() const
{
    return segmentLength_;
}

QRectF TreeStyleArrow::srcBoundingRect() const
{
    return srcBoundingRect_;
}

QRectF TreeStyleArrow::destBoundingRect() const
{
    return destBoundingRect_;
}

QPointF TreeStyleArrow::srcAxesIntersectionPoint() const
{
    return nullptr != srcAxesIntersectionPoint_ ? srcAxesIntersectionPoint_->pos() : QPointF {};
}

QPointF TreeStyleArrow::destAxesIntersectionPoint() const
{
    return nullptr != destAxesIntersectionPoint_ ? destAxesIntersectionPoint_->pos() : QPointF {};
}

QPointF TreeStyleArrow::newStartScenePos() const
{
    return newStartScenePos_;
}

bool TreeStyleArrow::isEachAxesIntersectionPointOk() const
{
    return nullptr != srcAxesIntersectionPoint_ && nullptr != destAxesIntersectionPoint_;
}

void TreeStyleArrow::setMaxTransitPointsCount(quint8 newMaxTransitPointsCount)
{
    SpecialStyleArrow::setMaxTransitPointsCount(newMaxTransitPointsCount);
}

void TreeStyleArrow::correctPositionBeforeUpdateGeometry(const QGraphicsItem* startItem, const QGraphicsItem* endItem,
                                                         ArrowPosition* newArrowPosition) const
{
    if (paramsFineForUpdateGeometry(startItem, endItem, newArrowPosition) && isEachAxesIntersectionPointOk())
    {
        const QPointF &srcAxesIntersectionPointPos = srcAxesIntersectionPoint_->pos();
        const QPointF &srcAxesIntersectionPointSrcPos = startItem->mapFromItem(this, srcAxesIntersectionPointPos);
        const QPointF &srcAxesIntersectionPointDestPos = endItem->mapFromItem(this, srcAxesIntersectionPointPos);

        const QPointF &destAxesIntersectionPointPos = destAxesIntersectionPoint_->pos();
        const QPointF &destAxesIntersectionPointSrcPos = startItem->mapFromItem(this, destAxesIntersectionPointPos);
        const QPointF &destAxesIntersectionPointDestPos = endItem->mapFromItem(this, destAxesIntersectionPointPos);

        const bool srcContainsSrcPoint = startItem->contains(srcAxesIntersectionPointSrcPos);
        const bool destContainsSrcPoint = endItem->contains(srcAxesIntersectionPointDestPos);
        const bool srcContainsDestPoint = startItem->contains(destAxesIntersectionPointSrcPos);
        const bool destContainsDestPoint = endItem->contains(destAxesIntersectionPointDestPos);

        const bool newEnabled = !srcContainsSrcPoint && !destContainsSrcPoint && !srcContainsDestPoint && !destContainsDestPoint;
        for (auto axesIntersectionPoint : transitPoints())
        {
            axesIntersectionPoint->setEnabled(newEnabled);
        }

        if (srcContainsSrcPoint)
        {
            newArrowPosition->currentStartPos = srcAxesIntersectionPointSrcPos;
        }
        else if (srcContainsDestPoint)
        {
            newArrowPosition->currentStartPos = destAxesIntersectionPointSrcPos;
        }

        if (destContainsDestPoint)
        {
            newArrowPosition->currentEndPos = destAxesIntersectionPointDestPos;
        }
        else if (destContainsSrcPoint)
        {
            newArrowPosition->currentEndPos = srcAxesIntersectionPointDestPos;
        }
    }
}

bool TreeStyleArrow::setAxesIntersectionPoints(AnnulusSPtrVector newAxesIntersectionPoints)
{
    if (newAxesIntersectionPoints.size() != maxTransitPointsCount())
    {
        return false;
    }
    AnnulusSPtr newSrcAxesIntersectionPoint = newAxesIntersectionPoints.front();
    if (newSrcAxesIntersectionPoint == srcAxesIntersectionPoint_)
    {
        return false;
    }
    AnnulusSPtr newDestAxesIntersectionPoint = newAxesIntersectionPoints.back();
    if (newDestAxesIntersectionPoint == destAxesIntersectionPoint_)
    {
        return false;
    }
    srcAxesIntersectionPoint_ = std::move(newSrcAxesIntersectionPoint);
    destAxesIntersectionPoint_ = std::move(newDestAxesIntersectionPoint);
    synchronizeAxesIntersectionPoints();
    return true;
}
