#pragma once

#include <QGraphicsTextItem>

class GraphicsTextItem : public QGraphicsTextItem
{
public:
    GraphicsTextItem(QGraphicsItem* parent = nullptr);

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
};
