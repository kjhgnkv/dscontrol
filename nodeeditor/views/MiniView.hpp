// MiniView.hpp

#pragma once

#include "nodeeditor/nodeeditor_export.h"
#include <QGraphicsView>

namespace QtNodes
{
class FlowScene;
class FlowView;

class NODEEDITOR_EXPORT MiniView final : public QGraphicsView
{
Q_OBJECT

public:
    explicit MiniView(QWidget* parent = nullptr);
    ~MiniView();

    void setFlowScene(FlowScene* scene);

    /**\brief set view wich will be managed by the MiniView
     */
    void setOriginalView(FlowView* view);

protected slots:
    /**\brief set visualized area the view to scene sceneRect
     */
    void setFlowSceneRect(const QRectF &sceneRect);

    void repaint();

signals:
    /**\brief emit when we TRY change position of originalViewportRect. Tell to
     * originalView to change visualization area (centered to centeredPos)
     * \warning this only please original view about change
     */
    void changeOiginalVisualizedArea(const QPointF &centeredPos);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void drawBackground(QPainter* painter, const QRectF &rect) override;
    void drawForeground(QPainter* painter, const QRectF &rect) override;
    // FIXME is obsolete method
    void drawItems(QPainter* painter, int numItems, QGraphicsItem** items,
                   const QStyleOptionGraphicsItem* options) override;

    // hide this
    using QGraphicsView::scene;
    using QGraphicsView::setScene;

private:
    FlowScene* scene_;
    FlowView* originalView_;
};
} // namespace QtNodes
