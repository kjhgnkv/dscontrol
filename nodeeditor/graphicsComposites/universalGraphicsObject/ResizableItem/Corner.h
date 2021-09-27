#pragma once

#include <QGraphicsItem>

namespace QtNodes
{
class Corner : public QObject, public QGraphicsItem
{
Q_OBJECT

public:
    enum Code
    {
        TopLeft,
        Top,
        TopRight,
        Right,
        BottomRight,
        Bottom,
        BottomLeft,
        Left
    };

public:
    Corner(Code code, QGraphicsItem* parent = nullptr);
    ~Corner() override;
    double width() const;
    QRectF boundingRect() const override;
    void setCurrentScale(qreal scale);
    void setWidth(qreal width);
    void setPos(const QPointF &point, bool silent = false);

signals:
    void positionHasChanged(Code code, const QPointF &deltaPos);
    void movingHasStarted();
    void movingHasFinished();

    // QGraphicsItem interface
protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

private:
    void restoreCursor();
    void updateGeometry();

private:
    qreal width_;
    qreal curWidth_;
    QRectF rect_;
    Code code_;
    QPointF deltaPos_;
    bool leftPressedFlag_;
    QColor color_;
    bool hoverFlag_;
    qreal scale_;
    QPointF startMovingPos_;
};
} // namespace QtNodes
