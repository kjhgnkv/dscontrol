#include "LateralHorizontalArrow.h"

using namespace QtNodes::Arrows;

LateralHorizontalArrow::LateralHorizontalArrow(const QPointF &startPoint, const QPointF &endPoint, const QUuid &uuid,
                                               QGraphicsItem* parentItem, QObject* parentObject)
: LateralArrow {startPoint, endPoint, uuid, parentItem, parentObject}
{
    setArrowType(ArrowType::LateralHorizontal);
}

AbstractArrow* LateralHorizontalArrow::clone() const
{
    AbstractArrow* ret = new LateralHorizontalArrow(*this);
    ret->synchronize();
    return ret;
}

void LateralHorizontalArrow::synchronizeAxesIntersectionPoints()
{
    setAxesIntersectionPoint({startItemCenterPoint().x(), endItemCenterPoint().y()});
}
