#pragma once

#include "Corner.h"
#include "ResizableItem.h"
#include <QGraphicsItem>

namespace QtNodes
{
class Corner;

class ResizeBox : public QObject, public QGraphicsItem
{
Q_OBJECT

    enum ScaleSize
    {
        Small,
        Medium,
        Large
    };

public:
    ResizeBox(QGraphicsItem* parent = nullptr, int type = (int)QtNodes::Item::NodeType::OtherType);
    QRectF boundingRect() const override;
    void setContentSize(const QSizeF &getContentSize);
    QSizeF getContentSize() const;
    double margin() const;
    void setMinSize(const QSizeF &size);
    void setType(Item::NodeType type);
    void setCurrentScale(qreal scale);
    void setLocked(bool flag);
    void enableResizing(bool flag = true);
    void setViewMode(int mode);
    void setColor(const QColor &color);
    void setComponentResize(bool flag);
    void setTextResize(bool flag);
    void setCornerWidth(qreal width);
    qreal currentMargin() const;

signals:
    void geometryChangeFinished();
    void geometryChangeStarted();

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    void prepareCorners();
    void updateGeometry(Corner::Code cornerCode = Corner::Code::TopLeft, QPointF deltaPos = {});
    void updateCorners();
    QSizeF computeNewSize(int cornerCode, QPointF deltaPos);
    void correctDelta(int cornerCode, QPointF &deltaPos, QSizeF &newSize);
    QPointF computeNewPosition(Corner::Code cornerCode, const QPointF &deltaPos);
    void updateCornerVisibility();
    void setVisibleSideCorners(bool flag = true);
    void setVisibleAngleCorners(bool flag = true);
    void updateRect();
    void updatePath();
    void setSize(const QSizeF &size);
    void saveCurrentGeometry();
private:
    QRectF rect_;
    QPainterPath path_;
    QSizeF contentSize_;
    std::vector<Corner*> corners_;
    qreal margin_;
    qreal currentMargin_;
    qreal borderWidth_;
    qreal curBorderWidth_;
    bool leftHasPressedFlag_;
    bool geometryChangeHasStartedFlag_;
    QColor color_;
    QSizeF minSize_;
    Item::NodeType type_;
    ScaleSize curSizeScale_;
    int viewMode_;
    bool canSideResize_;
    bool canAngleResize_;
    bool pictureResizeFlag_;
    bool componentResizeFlag_;
    bool textResizeFlag_;
    QRectF savedRect_;
    QPointF savedPos_;
    QPixmap hatchPixmap_;
};
} // namespace QtNodes
