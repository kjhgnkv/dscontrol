#include "TreeStyleHorizontalArrow.h"
#include "EdgePoint.h"

using namespace QtNodes::Arrows;

TreeStyleHorizontalArrow::TreeStyleHorizontalArrow(const QPointF &startPoint, const QPointF &endPoint,
                                                   const QUuid &uuid, QGraphicsItem* parentItem, QObject* parentObject)
: TreeStyleArrow {startPoint, endPoint, uuid, parentItem, parentObject}
{
    setArrowType(ArrowType::TreeStyleHorizontal);
}

AbstractArrow* TreeStyleHorizontalArrow::clone() const
{
    AbstractArrow* ret = new TreeStyleHorizontalArrow(*this);
    ret->synchronize();
    return ret;
}

void TreeStyleHorizontalArrow::synchronizeAxesIntersectionPoints()
{
    const qreal sourceWidth {srcBoundingRect().width()};
    const qreal destWidth {destBoundingRect().width()};
    const qreal srcLeft {srcBoundingRect().left()};
    const qreal destLeft {destBoundingRect().left()};
    const qreal srcCenterX {srcBoundingRect().center().x()};
    const qreal destCenterX {destBoundingRect().center().x()};
    const qreal srcRight {srcBoundingRect().right()};
    const qreal destRight {destBoundingRect().right()};
    const qreal curSegmentLength {segmentLength()};

    const bool isSourceWider = sourceWidth > destWidth;
    const bool isDestLeftOfSourceWithSegment = destRight < srcLeft - curSegmentLength;
    const bool isDestCenterLeftOfSourceCenter = destCenterX < srcCenterX;
    const bool isDestRightOfSourceWithSegment = destLeft > srcRight + curSegmentLength;

    qreal newAxesIntersectionPointX {0.0};
    if (isDestLeftOfSourceWithSegment)
    {
        newAxesIntersectionPointX = destRight + curSegmentLength;
    }
    else if (isDestRightOfSourceWithSegment)
    {
        newAxesIntersectionPointX = destLeft - curSegmentLength;
    }
    else if (isSourceWider == isDestCenterLeftOfSourceCenter)
    {
        newAxesIntersectionPointX = std::min(srcLeft, destLeft) - curSegmentLength;
    }
    else
    {
        newAxesIntersectionPointX = std::max(srcRight, destRight) + curSegmentLength;
    }

    const QPointF &newSrcAxesIntersectionPoint {newAxesIntersectionPointX, newStartScenePos().y()};
    const QPointF &newDestAxesIntersectionPoint {newAxesIntersectionPointX, endItemCenterPoint().y()};

    const QPointF &newSrcPointScenePos = mapFromScene(newSrcAxesIntersectionPoint);
    const QPointF &newDestPointScenePos = mapFromScene(newDestAxesIntersectionPoint);

    const QPointF &newSrcPointParentPos = mapToParent(newSrcPointScenePos);
    const QPointF &newDestPointParentPos = mapToParent(newDestPointScenePos);

    setAxesIntersectionPoints(newSrcPointParentPos, newDestPointParentPos);
}

void TreeStyleHorizontalArrow::correctMouseOffset(QPointF &offset)
{
    offset.setX(0);
}

void TreeStyleHorizontalArrow::correctSourceAxesIntersectionPoint()
{
    if (nullptr != smartStartPoint())
    {
        QPointF newAxesIntersectionPoint = mapFromScene(startPoint());
        newAxesIntersectionPoint.setX(srcAxesIntersectionPoint().x());
        setSrcAxesIntersectionPoint(newAxesIntersectionPoint);
    }
}
