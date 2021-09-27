#include "LateralVerticalArrow.h"

using namespace QtNodes::Arrows;

LateralVerticalArrow::LateralVerticalArrow(const QPointF &startPoint, const QPointF &endPoint, const QUuid &uuid,
                                           QGraphicsItem* parentItem, QObject* parentObject)
: LateralArrow {startPoint, endPoint, uuid, parentItem, parentObject}
{
    setArrowType(ArrowType::LateralVertical);
}

AbstractArrow* LateralVerticalArrow::clone() const
{
    AbstractArrow* ret = new LateralVerticalArrow(*this);
    ret->synchronize();
    return ret;
}

void LateralVerticalArrow::synchronizeAxesIntersectionPoints()
{
    setAxesIntersectionPoint({endItemCenterPoint().x(), startItemCenterPoint().y()});
}
