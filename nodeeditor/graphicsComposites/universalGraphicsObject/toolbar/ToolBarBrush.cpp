#include "ToolBarBrush.h"

#include <QApplication>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

namespace QtNodes
{
ToolBarBrush::ToolBarBrush(QGraphicsItem* parent)
: QObject {nullptr}
, QGraphicsItem {parent}
, rect_ {-4, -4, 30, 30}
, pixmap_ {":/toolbar/toolBarBrush.png"}
{
    setAcceptHoverEvents(true);
}

QRectF ToolBarBrush::boundingRect() const
{
    return rect_;
}

void ToolBarBrush::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    // makes this item hidden on pan and zoom view, but only on the top
    // view :( remove this condition if this behavior is unnecessary
    /*auto lod = QStyleOptionGraphicsItem::levelOfDetailFromTransform(
        painter->worldTransform());
    if (lod > 0.9)*/
    painter->drawPixmap(pixmap_.rect(), pixmap_, pixmap_.rect());

    Q_UNUSED(option);
    Q_UNUSED(widget);
}

void ToolBarBrush::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    QApplication::setOverrideCursor(Qt::PointingHandCursor);
    QGraphicsItem::hoverEnterEvent(event);
}

void ToolBarBrush::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    QApplication::restoreOverrideCursor();
    QGraphicsItem::hoverLeaveEvent(event);
}

void ToolBarBrush::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    qDebug() << "ToolBarBrush: mousePressEvent";
    QGraphicsItem::mousePressEvent(event);
    if (nullptr != event && Qt::LeftButton == event->button())
    {
        emit clicked();
        event->accept();
    }
}

void ToolBarBrush::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    qDebug() << "ToolBarBrush: mouseReleaseEvent";
    QGraphicsItem::mouseReleaseEvent(event);
}
} // namespace QtNodes
