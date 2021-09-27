#include "GraphicsTextItem.h"
#include <QGraphicsSceneContextMenuEvent>

GraphicsTextItem::GraphicsTextItem(QGraphicsItem* parent)
: QGraphicsTextItem {parent}
{
}

void GraphicsTextItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    event->ignore();
}
