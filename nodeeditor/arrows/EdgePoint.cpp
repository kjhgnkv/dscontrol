#include "EdgePoint.h"

#include <QDebug>
#include <QGraphicsSceneMouseEvent>

using namespace QtNodes::GeometricShapes;

EdgePoint::EdgePoint(const QPointF &centerPoint, const QUuid &id, QGraphicsItem* parentItem, QObject* parentObject)
: Annulus {centerPoint, id, parentItem, parentObject}
, positionMightBeChanged_ {true}
, mouseOffset_ {}
{
    setUndoRedoPositionEnable(false);
    setMovable(false);
    setFlag(ItemIsMovable, true);
}

void EdgePoint::setPositionMightBeChanged(bool mightBeChanged)
{
    positionMightBeChanged_ = mightBeChanged;
}

void EdgePoint::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (nullptr != event)
    {
        mouseOffset_ = event->scenePos() - event->lastScenePos();
    }
    return Annulus::mouseMoveEvent(event);
}

void EdgePoint::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    mouseOffset_.setX(0.0);
    mouseOffset_.setY(0.0);
    return Annulus::mouseReleaseEvent(event);
}

QVariant EdgePoint::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    switch (change)
    {
        case ItemPositionChange:
        {
            const QPointF &posBefore = pos();
            emit positionChange(mapToScene(value.toPointF()), mouseOffset_);
            const QPointF &posAfter = pos();
            if (posAfter != posBefore || !positionMightBeChanged_)
            {
                positionMightBeChanged_ = {true};
                return Annulus::itemChange(change, posAfter);
            }
            break;
        }
        default:
        {
            break;
        }
    }
    return Annulus::itemChange(change, value);
}
