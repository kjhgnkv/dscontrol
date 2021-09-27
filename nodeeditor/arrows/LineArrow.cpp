#include "LineArrow.h"
#include "Annulus.h"
#include "ArrowPosition.h"
#include "EquilateralTriangle.h"
#include "PolyLineArrow.h"

using namespace QtNodes;
using namespace QtNodes::Arrows;

LineArrow::LineArrow(const QPointF &newStartPoint, const QPointF &newEndPoint, const QUuid &uid,
                     QGraphicsItem* parentItem, QObject* parentObject)
: PolyLineArrow {newStartPoint, newEndPoint, uid, parentItem, parentObject}
{
    setArrowType(ArrowType::Line);
    setMaxTransitPointsCount(0);
}

AbstractArrow* LineArrow::clone() const
{
    AbstractArrow* ret = new LineArrow(*this);
    ret->synchronize();
    return ret;
}

void LineArrow::setMaxTransitPointsCount(quint8 newMaxTransitPointsCount)
{
    PolyLineArrow::setMaxTransitPointsCount(newMaxTransitPointsCount);
}
