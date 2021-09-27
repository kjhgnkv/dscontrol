#include "LockSignItem.h"

#include <QPainter>
#include <QSvgRenderer>

using namespace QtNodes;

LockSignItem::LockSignItem(QGraphicsItem* parentItem)
: QGraphicsItem {parentItem}
, rect_ {0, 0, 16, 16}
, isLocked_ {false}
{
}

QRectF LockSignItem::boundingRect() const
{
    return rect_;
}

void LockSignItem::setLocked(bool isLocked)
{
    isLocked_ = isLocked;
}

void LockSignItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    if (!isLocked_)
    {
        return;
    }

    QSvgRenderer renderer {QString {":/icons/lock.svg"}};
    QPixmap pixmap {rect_.size().toSize()};
    pixmap.fill(Qt::transparent);
    QPainter pixmapPainter {&pixmap};
    renderer.render(&pixmapPainter, pixmap.rect());
    painter->drawPixmap(rect_, pixmap, pixmap.rect());
}
