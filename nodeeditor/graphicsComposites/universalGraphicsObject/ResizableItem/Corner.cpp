#include "Corner.h"

#include <QApplication>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

namespace QtNodes
{
Corner::Corner(Code code, QGraphicsItem* parent)
: QObject {nullptr}
, QGraphicsItem {parent}
, width_ {8}
, curWidth_ {width_}
, rect_ {0, 0, width_, width_}
, code_ {code}
, color_ {"#ffffff"}
, hoverFlag_ {false}
, startMovingPos_ {}
{
    setFlags(ItemIsMovable | ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);
}

Corner::~Corner()
{
    restoreCursor();
}

double Corner::width() const
{
    return curWidth_;
}

QRectF Corner::boundingRect() const
{
    return rect_;
}

void Corner::setCurrentScale(qreal scale)
{
    scale_ = scale;
    updateGeometry();
}

void Corner::setWidth(qreal width)
{
    width_ = width;
    updateGeometry();
}

void Corner::setPos(const QPointF &point, bool silent)
{
    if (silent && (flags() | ItemSendsGeometryChanges))
    {
        setFlag(ItemSendsGeometryChanges, false);
        QGraphicsItem::setPos(point);
        setFlag(ItemSendsGeometryChanges, true);
    }
    else
    {
        QGraphicsItem::setPos(point);
    }
}

void Corner::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    if (color_ == QString("transparent"))
        painter->setPen(color_);
    else
        painter->setPen("#3b3e3d");
    painter->setBrush(QColor(color_));

    painter->drawRect(rect_);
}

QVariant Corner::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change)
    {
        case ItemPositionChange:
        {
            QPointF deltaPos = value.toPointF() - startMovingPos_;
            emit positionHasChanged(code_, deltaPos);

            return QGraphicsItem::itemChange(change, pos());
        }
        case ItemVisibleHasChanged:
        {
            if (hoverFlag_)
                hoverLeaveEvent(nullptr);
            break;
        }
        default:
            break;
    }

    return QGraphicsItem::itemChange(change, value);
}

void Corner::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        leftPressedFlag_ = true;
        startMovingPos_ = pos();
        emit movingHasStarted();
    }

    QGraphicsItem::mousePressEvent(event);
}

void Corner::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        startMovingPos_ = {};
        emit movingHasFinished();
    }

    QGraphicsItem::mouseReleaseEvent(event);
}

void Corner::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    hoverFlag_ = true;

    if (flags() & ItemIsMovable)
    {
        QCursor cursor = Qt::SizeFDiagCursor;

        switch (code_)
        {
            case TopRight:
            case BottomLeft:
            {
                cursor = Qt::SizeBDiagCursor;
                break;
            }
            case Top:
            case Bottom:
            {
                cursor = Qt::SizeVerCursor;
                break;
            }
            case Left:
            case Right:
            {
                cursor = Qt::SizeHorCursor;
                break;
            }
            default:
                break;
        }
        QApplication::setOverrideCursor(cursor);
    }

    QGraphicsItem::hoverEnterEvent(event);
}

void Corner::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    restoreCursor();
    QGraphicsItem::hoverLeaveEvent(event);
}

void Corner::restoreCursor()
{
    hoverFlag_ = false;

    if (flags() & ItemIsMovable)
        QApplication::restoreOverrideCursor();
}

void Corner::updateGeometry()
{
    curWidth_ = (scale_ > 100) ? width_ : (100 / scale_ * width_);

    prepareGeometryChange();
    rect_.setSize({curWidth_, curWidth_});
}
} // namespace QtNodes
