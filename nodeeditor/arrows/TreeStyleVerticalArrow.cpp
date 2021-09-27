#include "TreeStyleVerticalArrow.h"

using namespace QtNodes::Arrows;

TreeStyleVerticalArrow::TreeStyleVerticalArrow(const QPointF &startPoint, const QPointF &endPoint, const QUuid &uuid,
                                               QGraphicsItem* parentItem, QObject* parentObject)
: TreeStyleArrow {startPoint, endPoint, uuid, parentItem, parentObject}
{
    setArrowType(ArrowType::TreeStyleVertical);
}

AbstractArrow* TreeStyleVerticalArrow::clone() const
{
    AbstractArrow* ret = new TreeStyleVerticalArrow(*this);
    ret->synchronize();
    return ret;
}

void TreeStyleVerticalArrow::synchronizeAxesIntersectionPoints()
{
    const qreal sourceHeight {srcBoundingRect().height()};
    const qreal destHeight {destBoundingRect().height()};
    const qreal srcTop {srcBoundingRect().top()};
    const qreal destTop {destBoundingRect().top()};
    const qreal srcCenterY {srcBoundingRect().center().y()};
    const qreal destCenterY {destBoundingRect().center().y()};
    const qreal srcBottom {srcBoundingRect().bottom()};
    const qreal destBottom {destBoundingRect().bottom()};
    const qreal curSegmentLength {segmentLength()};

    const bool isSourceHigher = sourceHeight > destHeight;
    const bool isDestTopOfSourceWithSegment = destBottom < srcTop - segmentLength();
    const bool isDestCenterTopOfSourceCenter = destCenterY < srcCenterY;
    const bool isDestBottomOfSourceWithSegment = destTop > srcBottom + segmentLength();
    const bool isBottomSide = destBottom < srcTop;

    qreal newAxesIntersectionPointY = {isBottomSide ? destBottom + curSegmentLength : destTop - curSegmentLength};

    if (isDestTopOfSourceWithSegment)
    {
        newAxesIntersectionPointY = destBottom + curSegmentLength;
    }
    else if (isDestBottomOfSourceWithSegment)
    {
        newAxesIntersectionPointY = destTop - curSegmentLength;
    }
    else if (isSourceHigher == isDestCenterTopOfSourceCenter)
    {
        newAxesIntersectionPointY = std::min(srcTop, destTop) - curSegmentLength;
    }
    else
    {
        newAxesIntersectionPointY = std::max(srcBottom, destBottom) + curSegmentLength;
    }

    const QPointF &newSrcAxesIntersectionPoint {newStartScenePos().x(), newAxesIntersectionPointY};
    const QPointF &newDestAxesIntersectionPoint {endItemCenterPoint().x(), newAxesIntersectionPointY};

    const QPointF &newSrcPointScenePos = mapFromScene(newSrcAxesIntersectionPoint);
    const QPointF &newDestPointScenePos = mapFromScene(newDestAxesIntersectionPoint);

    const QPointF &newSrcPointParentPos = mapToParent(newSrcPointScenePos);
    const QPointF &newDestPointParentPos = mapToParent(newDestPointScenePos);

    setAxesIntersectionPoints(newSrcPointParentPos, newDestPointParentPos);
}

void TreeStyleVerticalArrow::correctMouseOffset(QPointF &offset)
{
    offset.setY(0);
}

void TreeStyleVerticalArrow::correctSourceAxesIntersectionPoint()
{
    if (nullptr != smartStartPoint())
    {
        QPointF newAxesIntersectionPoint = mapFromScene(startPoint());
        newAxesIntersectionPoint.setY(srcAxesIntersectionPoint().y());
        setSrcAxesIntersectionPoint(newAxesIntersectionPoint);
    }
}
