#include "ToolBarItem.h"

#include "ToolBarWidget.h"
#include <QDebug>
#include <QGraphicsProxyWidget>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

namespace QtNodes
{
ToolBarItem::ToolBarItem(QGraphicsItem* parent)
: QObject(nullptr)
, QGraphicsItem(parent)
, rect_ {-40, -40, 312, 112}
, toolBarWidget_ {nullptr}
, isFixedOnScene_ {false}
{
    setAcceptHoverEvents(true);
    setFlags(ItemIgnoresTransformations);

    auto proxy = new QGraphicsProxyWidget(this);
    toolBarWidget_ = new ToolBarWidget;
    proxy->setWidget(toolBarWidget_);

    connect(toolBarWidget_, &ToolBarWidget::interactionHasStarted, [=]
    {
        isFixedOnScene_ = true;
    });
    connect(toolBarWidget_, &ToolBarWidget::interactionHasFinished, [=]
    {
        isFixedOnScene_ = false;
    });
}

ToolBarItem::~ToolBarItem()
{
    disconnect();
    toolBarWidget_->disconnect();
}

void ToolBarItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    if (!isFixedOnScene_)
    {
        hide();
        emit hasHidden();
    }
    QGraphicsItem::hoverLeaveEvent(event);
}

QRectF ToolBarItem::boundingRect() const
{
    return rect_;
}

ToolBarWidget* ToolBarItem::getToolBarWidget()
{
    return toolBarWidget_;
}

void ToolBarItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(painter)
    Q_UNUSED(option)
    Q_UNUSED(widget)
}
} // namespace QtNodes
