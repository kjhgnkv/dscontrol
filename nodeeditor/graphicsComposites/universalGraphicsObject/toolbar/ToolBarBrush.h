#pragma once

#include <QGraphicsItem>
#include <QObject>

namespace QtNodes
{
class ToolBarBrush : public QObject, public QGraphicsItem
{
Q_OBJECT

public:
    ToolBarBrush(QGraphicsItem* parent = nullptr);
    QRectF boundingRect() const override;

signals:
    void clicked();

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    QRectF rect_;
    QPixmap pixmap_;
};
} // namespace QtNodes
