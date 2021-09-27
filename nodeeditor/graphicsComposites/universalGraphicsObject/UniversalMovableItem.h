#ifndef UNIVERSALMOVABLEITEM_H
#define UNIVERSALMOVABLEITEM_H

#include "graphicsComposites/universalGraphicsObject/UniversalSelectableItem.h"

namespace QtNodes
{
class UniversalMovableItem : public UniversalSelectableItem
{
public:
    UniversalMovableItem(const QUuid &id = QUuid::createUuid(), QGraphicsItem* parentItem = nullptr,
                         QObject* parentObject = nullptr);

    UniversalMovableItem(const UniversalMovableItem &another);
    ~UniversalMovableItem() override = default;

    QJsonObject save() const override;
    void restore(const QJsonObject &restoringObject) override;

    bool isMovable() const;
    void setMovable(bool movable);
    void setUndoRedoPositionEnable(bool enable);
    bool undoRedoPositionEnable() const;

    void setPos(const QPointF &newPos, bool undoRedoPositionEnable);
    void setPos(qreal x, qreal y, bool undoRedoPositionEnable);
    void moveBy(qreal dx, qreal dy, bool undoRedoPositionEnable);

    void setPos(const QPointF &newPos);
    void setPos(qreal x, qreal y);
    void moveBy(qreal dx, qreal dy);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    bool isPressed() const;

private:
    void makePosOnPressEventInvalid();
    void returnMouseReleaseEvent(QGraphicsSceneMouseEvent* event);

private:
    bool isPressed_;
    bool isMovable_;
    bool isPosOnPressEventValid_;
    bool undoRedoPositionEnable_;
    QPointF posOnPressEvent_;
};
} // namespace QtNodes

#endif // UNIVERSALMOVABLEITEM_H
