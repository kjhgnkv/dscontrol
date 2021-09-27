// FlowViewPainter.cpp

#include "views/FlowViewPainter.hpp"
#include "styles/StyleCollection.hpp"
#include "views/FlowView.hpp"

#include <QFile>
#include <QGraphicsScene>
#include <QPainter>
#include <QRectF>
#include <QSvgRenderer>
#include <cmath>

static QSvgRenderer svgRenderer;

namespace QtNodes
{
void FlowViewPainter::drawGrid(FlowView* view, QPainter* painter, const QRectF & /*rect*/)
{
    static auto drawLyambda = [](FlowView* viewA, QPainter* painterA, double gridStep)
    {
        QRect windowRect = viewA->rect();
        QPointF tl = viewA->mapToScene(windowRect.topLeft());
        QPointF br = viewA->mapToScene(windowRect.bottomRight());

        double left = std::floor(tl.x() / gridStep - 0.5);
        double right = std::floor(br.x() / gridStep + 1.0);
        double bottom = std::floor(tl.y() / gridStep - 0.5);
        double top = std::floor(br.y() / gridStep + 1.0);

        // vertical lines
        for (int xi = int(left); xi <= int(right); ++xi)
        {
            QLineF line(xi * gridStep, bottom * gridStep, xi * gridStep, top * gridStep);

            painterA->drawLine(line);
        }

        // horizontal lines
        for (int yi = int(bottom); yi <= int(top); ++yi)
        {
            QLineF line(left * gridStep, yi * gridStep, right * gridStep, yi * gridStep);
            painterA->drawLine(line);
        }
    };
    if (StyleCollection::flowViewStyle().drawGrid())
    {
        QPen pfine(StyleCollection::flowViewStyle().fineGridColor, 1.0);

        painter->setPen(pfine);
        drawLyambda(view, painter, 15);

        QPen p(StyleCollection::flowViewStyle().coarseGridColor, 1.0);

        painter->setPen(p);
        drawLyambda(view, painter, 150);
    }
}

void FlowViewPainter::drawPixmap(FlowView* /*view*/, QPainter* painter, const QRectF &rect)
{
    painter->setClipRect(rect);
    painter->fillRect(rect, StyleCollection::flowViewStyle().backgroundPixmap);
}

void FlowViewPainter::drawSvg(FlowView* view, QPainter* painter, const QRectF & /*rect*/)
{
    painter->setClipRect(view->QGraphicsView::scene()->sceneRect());
    svgRenderer.load(StyleCollection::flowViewStyle().svgImage);
    svgRenderer.render(painter, view->QGraphicsView::scene()->sceneRect());
}

void FlowViewPainter::drawSimple(FlowView* /*view*/, QPainter* painter, const QRectF &rect)
{
    painter->fillRect(rect, StyleCollection::flowViewStyle().backgroundColor);
}

void FlowViewPainter::draw(FlowView* view, QPainter* painter, const QRectF &rect)
{
    if (!StyleCollection::flowViewStyle().svgImage.isEmpty() && QFile::exists(
    StyleCollection::flowViewStyle().svgImage))
    {
        drawSvg(view, painter, rect);
    }
    else if (!StyleCollection::flowViewStyle().backgroundPixmap.isNull())
    {
        drawPixmap(view, painter, rect);
    }
    else
    {
        drawSimple(view, painter, rect);
    }
    drawGrid(view, painter, rect);
}
} // namespace QtNodes
