// FlowViewPainter.hpp

#pragma once

class QPainter;
class QRectF;

namespace QtNodes
{
class FlowView;

class FlowViewPainter
{
public:
    static void draw(FlowView* view, QPainter* painter, const QRectF &rect);

    static void drawSvg(FlowView* view, QPainter* painter, const QRectF &rect);

    static void drawPixmap(FlowView* view, QPainter* painter, const QRectF &rect);

    static void drawSimple(FlowView* view, QPainter* painter, const QRectF &rect);

    static void drawGrid(FlowView* view, QPainter* painter, const QRectF &rect);
};
} // namespace QtNodes
