#include "UniversalMovableItem.h"
//
#include "utils/undo/AbstractSceneCommand.hpp"
//
#include "factories/AbstractCommandFactory.hpp"
#include "factories/CommandFactory.hpp"
//
#include "singletones/UndoController.hpp"
//
#include <QDebug>
#include <QGraphicsSceneMouseEvent>

using namespace QtNodes;

UniversalMovableItem::UniversalMovableItem(const QUuid &id, QGraphicsItem* parentItem, QObject* parentObject)
: UniversalSelectableItem {id, parentItem, parentObject}
, isPressed_ {false}
, isMovable_ {true}
, isPosOnPressEventValid_ {false}
, undoRedoPositionEnable_ {false}
, posOnPressEvent_ {}
{
    setFlag(ItemIsMovable, false);
}

UniversalMovableItem::UniversalMovableItem(const UniversalMovableItem &another)
: UniversalSelectableItem {another}
, isPressed_ {another.isPressed_}
, isMovable_ {another.isMovable_}
, isPosOnPressEventValid_ {another.isPosOnPressEventValid_}
, undoRedoPositionEnable_ {another.undoRedoPositionEnable_}
, posOnPressEvent_ {another.posOnPressEvent_}
{
}

QJsonObject UniversalMovableItem::save() const
{
    QJsonObject retval = UniversalSelectableItem::save();
    retval["undoRedoPositionEnable"] = undoRedoPositionEnable_;
    retval["isMovable"] = isMovable_;
    QJsonObject obj {};

    auto savingPos = pos();
    obj = {{  "x", savingPos.x()}
           , {"y", savingPos.y()}
    };
    retval["pos"] = obj;

    return retval;
}

void UniversalMovableItem::restore(const QJsonObject &restoringObject)
{
    setUndoRedoPositionEnable(false);

    UniversalSelectableItem::restore(restoringObject);
    QJsonObject obj;
    obj = restoringObject["pos"].toObject();
    setPos({obj["x"].toDouble(), obj["y"].toDouble()});
    setMovable(restoringObject["isMovable"].toBool());
    setUndoRedoPositionEnable(restoringObject["undoRedoPositionEnable"].toBool());
}

bool UniversalMovableItem::isMovable() const
{
    return isMovable_;
}

void UniversalMovableItem::setMovable(bool movable)
{
    isMovable_ = movable;
}

void UniversalMovableItem::setUndoRedoPositionEnable(bool enable)
{
    undoRedoPositionEnable_ = enable;
}

bool UniversalMovableItem::undoRedoPositionEnable() const
{
    return undoRedoPositionEnable_;
}

void UniversalMovableItem::setPos(const QPointF &newPos, bool undoRedoPositionEnable)
{
    const bool savedUndoRedoPositionEnable = undoRedoPositionEnable_;
    setUndoRedoPositionEnable(undoRedoPositionEnable);
    setPos(newPos);
    setUndoRedoPositionEnable(savedUndoRedoPositionEnable);
}

void UniversalMovableItem::setPos(qreal x, qreal y, bool undoRedoPositionEnable)
{
    setPos(QPointF {x, y}, undoRedoPositionEnable);
}

void UniversalMovableItem::moveBy(qreal dx, qreal dy, bool undoRedoPositionEnable)
{
    setPos(pos() + QPointF {dx, dy}, undoRedoPositionEnable);
}

void UniversalMovableItem::setPos(const QPointF &newPos)
{
    QPointF currentPos = pos();
    if (currentPos == newPos)
    {
        return;
    }
    auto scene = flowScene();
    if (nullptr != scene && undoRedoPositionEnable_)
    {
        auto command = UndoController::Instance().factory()->createChangeItemPositionCommand(scene->id(), id(),
                                                                                             currentPos, newPos,
                                                                                             nullptr);
        UndoController::Instance().push(command);
    }
    else
    {
        UniversalSelectableItem::setPos(newPos);
    }
}

void UniversalMovableItem::setPos(qreal x, qreal y)
{
    setPos(QPointF(x, y));
}

void UniversalMovableItem::moveBy(qreal dx, qreal dy)
{
    setPos(pos() + QPointF(dx, dy));
}

void UniversalMovableItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (nullptr != event && Qt::LeftButton == event->button() && isMovable_)
    {
        isPressed_ = true;
        posOnPressEvent_ = pos();
        isPosOnPressEventValid_ = true;
        UniversalSelectableItem::mousePressEvent(event);
        event->accept();
        return;
    }
    UniversalSelectableItem::mousePressEvent(event);
}

void UniversalMovableItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (nullptr != event && isPressed_ && isMovable_ && event->buttons().testFlag(Qt::LeftButton))
    {
        UniversalSelectableItem::setPos(mapToParent(event->pos()));
    }
    UniversalSelectableItem::mouseMoveEvent(event);
}

void UniversalMovableItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (nullptr == event || !isPressed_ || !isMovable_ || Qt::LeftButton != event->button())
    {
        return returnMouseReleaseEvent(event);
    }
    QPointF newPos = pos() + event->pos() - event->lastPos();
    if (newPos == posOnPressEvent_)
    {
        return returnMouseReleaseEvent(event);
    }
    auto scene = flowScene();
    if (nullptr != scene)
    {
        auto command = UndoController::Instance().factory()->createChangeItemPositionCommand(scene->id(), id(),
                                                                                             posOnPressEvent_, newPos,
                                                                                             nullptr);
        UndoController::Instance().push(command);
    }
    else
    {
        UniversalSelectableItem::setPos(newPos);
    }
    returnMouseReleaseEvent(event);
}

bool UniversalMovableItem::isPressed() const
{
    return isPressed_;
}

void UniversalMovableItem::makePosOnPressEventInvalid()
{
    if (isPosOnPressEventValid_)
    {
        posOnPressEvent_.setX(0);
        posOnPressEvent_.setY(0);
        isPosOnPressEventValid_ = false;
    }
}

void UniversalMovableItem::returnMouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    isPressed_ = false;
    makePosOnPressEventInvalid();
    UniversalSelectableItem::mouseReleaseEvent(event);
}
