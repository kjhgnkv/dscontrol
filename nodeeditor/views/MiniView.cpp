// MiniView.cpp

#include "views/MiniView.hpp"
#include "scenes/FlowScene.hpp"
#include "styles/StyleCollection.hpp"
#include "views/FlowView.hpp"

#include <QDebug>
#include <QMouseEvent>
#include <QStyleOptionGraphicsItem>
#include "graphicsComposites/universalGraphicsObject/toolbar/ToolBarBrush.h"

namespace QtNodes
{
MiniView::MiniView(QWidget* parent)
: QGraphicsView {parent}
, scene_ {nullptr}
, originalView_ {nullptr}
{
    setRenderHint(QPainter::Antialiasing);
    setCacheMode(QGraphicsView::CacheBackground);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setInteractive(false);

    setOptimizationFlag(QGraphicsView::IndirectPainting);
}

MiniView::~MiniView()
{
}

void MiniView::setFlowScene(FlowScene* scene)
{
    if (scene_)
    {
        disconnect(scene_, nullptr, this, nullptr);
        disconnect(this, nullptr, scene_, nullptr);
    }
    scene_ = scene;
    QGraphicsView::setScene(scene_);
    if (scene)
    {
        connect(scene_, &QGraphicsScene::sceneRectChanged, this, &QtNodes::MiniView::setFlowSceneRect);
        connect(scene_, &QObject::destroyed, this, [this]()
        {
            scene_ = nullptr;
        });
        setFlowSceneRect(scene_->sceneRect());
    }
}

void MiniView::setOriginalView(FlowView* view)
{
    if (originalView_)
    {
        disconnect(originalView_, nullptr, this, nullptr);
        disconnect(this, nullptr, originalView_, nullptr);
    }
    originalView_ = view;
    if (originalView_)
    {
        connect(originalView_, &QtNodes::FlowView::visualizedRectChanged, this, &QtNodes::MiniView::repaint);
        connect(originalView_, &QObject::destroyed, this, [this]()
        {
            originalView_ = nullptr;
        });
    }
}

void MiniView::setFlowSceneRect(const QRectF &sceneRect)
{
    repaint();
    fitInView(sceneRect, Qt::KeepAspectRatio);
}

void MiniView::mousePressEvent(QMouseEvent* event)
{
    mouseMoveEvent(event);
}

void MiniView::mouseMoveEvent(QMouseEvent* event)
{
    // TODO here miniview emit signal of originalView, but this is not good.
    // Maybe is better way
    if (scene_ && originalView_ && (event->buttons() & Qt::MouseButton::LeftButton))
    {
        originalView_->centerOn(mapToScene(event->pos()));
        emit originalView_->visualizedRectChanged();
    }
}

void MiniView::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
}

void MiniView::resizeEvent(QResizeEvent*)
{
    if (scene_)
    {
        fitInView(sceneRect(), Qt::KeepAspectRatio);
    }
}

void MiniView::drawBackground(QPainter* painter, const QRectF & /*rect*/)
{
    if (scene_)
    {
        painter->setClipRect(sceneRect());
        painter->fillRect(sceneRect(), QColor("#AAAAC5"));
    }
}

void MiniView::drawForeground(QPainter* painter, const QRectF & /*rect*/)
{
    if (scene_ && originalView_)
    {
        painter->setClipRect(sceneRect());

        QPainterPath allScreen;
        allScreen.addRect(sceneRect());
        allScreen.addRect(originalView_->mapToScene(originalView_->viewport()->geometry()).boundingRect());

        painter->fillPath(allScreen, QColor(96, 95, 110, 100));
        painter->drawPath(allScreen);
    }
}

void MiniView::drawItems(QPainter* painter, int numItems, QGraphicsItem** items,
                         const QStyleOptionGraphicsItem* options)
{
    for (auto i = 0; i < numItems; i++)
    {
        auto item = items[i];
        if (item && !dynamic_cast<ToolBarBrush*>(item))
        {
            painter->save();
            painter->setTransform(item->sceneTransform(), true);
            item->paint(painter, &options[i], {});
            painter->restore();
        }
    }
    repaint();
}

void MiniView::repaint()
{
    viewport()->update();
}

void MiniView::wheelEvent(QWheelEvent* event)
{
    if (scene_ && originalView_)
    {
        originalView_->wheelEvent(event);
    }
}
} // namespace QtNodes
