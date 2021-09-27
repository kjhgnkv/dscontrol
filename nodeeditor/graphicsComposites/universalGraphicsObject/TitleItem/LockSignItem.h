#pragma once

#include <QGraphicsItem>

namespace QtNodes
{
class LockSignItem : public QGraphicsItem
{
public:
    LockSignItem(QGraphicsItem* parentItem);

    QRectF boundingRect() const override;
    void setLocked(bool isLocked);

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    QRectF rect_;
    bool isLocked_;
};
}
