#pragma once

#include "nodeeditor/nodeeditor_export.h"
#include <QGraphicsView>
#include <QScrollBar>
#include "common/nodeeditor_globals.hpp"

class QAction;
class QRubberBand;

#define V_SCROLL_BAR 0x01
#define H_SCROLL_BAR 0x02
#define DRAG_SCROLL 0x04

namespace QtNodes
{
class FlowScene;
class MiniView;
class FlowViewPainter;

class UniversalGraphicsItem;

class NODEEDITOR_EXPORT FlowView final : public QGraphicsView
{
Q_OBJECT

    friend MiniView;
    friend FlowViewPainter;

public:
    explicit FlowView(QWidget* parent = Q_NULLPTR);
    ~FlowView() override;
    FlowView(const FlowView &) = delete;
    FlowView operator=(const FlowView &) = delete;
    QAction* deleteSelectionAction() const;
    QAction* normalizeSelectionAction() const;
    void setFlowScene(FlowScene* scene);
    FlowScene* flowScene() const;
    Item::NodeType getType(UniversalGraphicsItem* item) const;
    int getCurrentScale() const;
    void changeScaleForItemToolBar();
    void changeScaleForResizableItems(double scale);
    void showTopLevelConnections(bool flag = true) const;
    void changeTextItemsVisibility(double scale);
    void setBackColor(const QColor &color);
    QColor getBackColor() const;
    void setCutterMode(bool b);

signals:
    /**\brief the signal emit when visualized rect changed. Needed for MiniView
     */
    void visualizedRectChanged();

    void scaleChanged(qreal curScale);
    void itemsUnselected(qreal curScale);

    void areaCutted(const QRect &area);

public slots:
    void scaleUp();
    void scaleDown();
    void setScale(int scale);

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void hideEvent(QHideEvent* event) override;

    // hide this methods
    using QGraphicsView::scene;
    using QGraphicsView::setScene;

private:
    bool cutterMode_;
    int currentScale_;
    QPointF clickPos_;
    QPoint cutterOrigin_;
    QRectF grabItemOldRect_;
    FlowScene* scene_;
    QAction* deleteSelectionAction_;
    QAction* normalizeSelectionAction_;
    QColor backColor_;
    QRubberBand* rubberBand_;
};
} // namespace QtNodes
