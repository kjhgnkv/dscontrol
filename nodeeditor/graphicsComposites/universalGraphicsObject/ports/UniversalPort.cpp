#include "UniversalPort.h"
#include "arrows/TinyMath.h"
#include "UniversalPortBar.h"
#include "UniversalPortProjection.h"

#include <QApplication>
#include <QGraphicsView>

namespace QtNodes
{
UniversalPort::UniversalPort(int inOutType, int commId, UniversalPortProjection* portProjItem, int side, double width,
                             const QUuid &id, QGraphicsItem* parentItem, QObject* parentObject)
: UniversalConnectionableItem(id, parentItem, parentObject)
, inOutType_ {inOutType}
, commId_ {commId}
, portProjItem_ {portProjItem}
, width_ {width}
, rect_ {0, 0, width_, width_}
, leftPressedFlag_ {false}
, side_ {side}
, brashColor_ {"#ffffff"}
{
    setFlag(ItemSendsGeometryChanges, true);
    setFlag(ItemIsSelectable, false);
    setFlag(ItemIsMovable, true);
    setAcceptHoverEvents(true);

    portProjItem_->setParent(this);
    moveToSide();
    correctProjection();

    type_ = static_cast<int>(Item::NodeType::OtherType);

    if (inOutType == Out || inOutType == InOut)
        brashColor_ = "#f2c94c";
}

QRectF UniversalPort::boundingRect() const
{
    return rect_;
}

void UniversalPort::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    double penWidth = 1;

    painter->setPen(QPen(Qt::white, penWidth));
    painter->setBrush(QColor(brashColor_));

    QRectF rect = boundingRect();
    rect.translate(penWidth / 2, penWidth / 2);
    rect.setBottomRight(rect.bottomRight() - QPointF(penWidth, penWidth));

    painter->drawEllipse(rect);

    if (inOutType_ == InOut)
    {
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor("#ffffff"));
        painter->drawChord(rect, 45 * 16, 180 * 16);
    }

    Q_UNUSED(option);
    Q_UNUSED(widget);
}

void UniversalPort::moveToSide()
{
    QRectF parRect = parentItem()->boundingRect();

    if (side_ == Top)
        setPos(parRect.width() / 2 - width_ / 2, 0);
    else if (side_ == Bottom)
        setPos(parRect.width() / 2 - width_ / 2, parRect.height());
    else if (side_ == Left)
        setPos(0, parRect.height() / 2 - width_ / 2);
    else if (side_ == Right)
        setPos(parRect.width(), parRect.height() / 2 - width_ / 2);

    correctPosition();
}

UniversalConnectionableItem* UniversalPort::getPortProjectionItem()
{
    return portProjItem_;
}

bool UniversalPort::compare(int type, int commId)
{
    return inOutType_ == type && commId_ == commId;
}

QUuid UniversalPort::index() const
{
    return id();
}

int UniversalPort::communicationId() const
{
    return commId_;
}

QPainterPath UniversalPort::shape() const
{
    QPainterPath ret;
    ret.addEllipse(boundingRect());
    return ret;
}

void UniversalPort::correctPosition()
{
    UniversalPortBar* portBar = dynamic_cast<UniversalPortBar*>(parentItem());
    if (!portBar)
        return;

    QRectF barRect = portBar->boundingRect();

    qreal barWidth = barRect.width();
    qreal barHeight = barRect.height();

    // Correct port position for port bar rect
    if (x() < 0)
        setX(0);
    else if (x() + width_ > barWidth)
        setX(barWidth - width_);
    if (y() < 0)
        setY(0);
    else if (y() + width_ > barHeight)
        setY(barHeight - width_);

    // Attach to one of sides
    qreal dTop = y();
    qreal dBottom = barHeight - y() - width_;
    qreal dLeft = x();
    qreal dRight = barWidth - x() - width_;

    qreal min = std::min({dTop, dBottom, dLeft, dRight});

    if (min > 10)
    {
        if (side_ == Top)
            setY(0);
        else if (side_ == Bottom)
            setY(barHeight - width_);
        else if (side_ == Left)
            setX(0);
        else if (side_ == Right)
            setX(barWidth - width_);
    }
    else
    {
        if (TinyMath::almostEqual(min, dTop))
        {
            setY(0);
            side_ = Top;
        }
        else if (TinyMath::almostEqual(min, dBottom))
        {
            setY(barHeight - width_);
            side_ = Bottom;
        }
        else if (TinyMath::almostEqual(min, dLeft))
        {
            setX(0);
            side_ = Left;
        }
        else if (TinyMath::almostEqual(min, dRight))
        {
            setX(barWidth - width_);
            side_ = Right;
        }
    }

    correctProjection();
}

void UniversalPort::correctProjection()
{
    if (!portProjItem_)
    {
        return;
    }

    QGraphicsView* view = nullptr;
    auto views = portProjItem_->scene()->views();

    for (auto v : views)
    {
        if (v)
        {
            if (v->isVisible())
            {
                view = v;
                break;
            }
        }
    }

    if (!view)
    {
        return;
    }

    QRect viewRect = view->rect();
    QRect viewGeometryRect = view->geometry();
    QRectF portRect = boundingRect();
    QRectF workRect = view->mapToScene(viewRect).boundingRect();

    QPointF workTopLeft = workRect.topLeft();
    QPointF viewDelta = viewGeometryRect.topLeft();
    QPointF portSizeDelta(portRect.width(), portRect.height());
    QPointF newPos = pos() + workTopLeft - viewDelta - portSizeDelta;

    portProjItem_->setPos(newPos);
    portProjItem_->setSize(portRect.size());
}

QVariant UniversalPort::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionHasChanged)
    {
        correctPosition();
    }
    return UniversalConnectionableItem::itemChange(change, value);
}

void UniversalPort::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    QApplication::setOverrideCursor(Qt::PointingHandCursor);
    QGraphicsItem::hoverEnterEvent(event);
}

void UniversalPort::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    QApplication::restoreOverrideCursor();
    QGraphicsItem::hoverLeaveEvent(event);
}

QJsonObject UniversalPort::save() const
{
    QJsonObject retval = UniversalConnectionableItem::save();

    retval["inOutType"] = inOutType_;
    retval["commId"] = commId_;
    retval["projection"] = portProjItem_->save();

    QJsonObject obj = {
    {  "width" , width_}
    , {"height", width_}
    };
    retval["size"] = obj;

    obj = {
    {  "x", pos().x()}
    , {"y", pos().y()}
    };
    retval["pos"] = obj;

    retval["leftPressedFlags"] = leftPressedFlag_;

    obj = {
    {  "x", leftPressedPos_.x()}
    , {"y", leftPressedPos_.y()}
    };
    retval["leftPressedPos"] = obj;

    retval["side"] = side_;

    return retval;
}

void UniversalPort::restore(const QJsonObject &p)
{
    UniversalConnectionableItem::restore(p);

    commId_ = p["commId"].toInt();
    inOutType_ = p["inOutType"].toInt();

    // portProjItem_->restore(p["projection"].toObject());

    QJsonObject obj = p["size"].toObject();
    width_ = obj["width"].toDouble();
    rect_.setSize({width_, width_});

    obj = p["pos"].toObject();
    setPos({obj["x"].toDouble(), obj["y"].toDouble()});

    leftPressedFlag_ = p["lastPressedFlag"].toBool();

    obj = p["leftPressedPos"].toObject();
    leftPressedPos_ = {obj["x"].toDouble(), obj["y"].toDouble()};

    side_ = p["side"].toInt();
}

void UniversalPort::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsScene* currentScene = scene();
    if (nullptr != currentScene)
    {
        currentScene->clearSelection();
    }
    emit movingHasStarted();
    UniversalConnectionableItem::mousePressEvent(event);
}

void UniversalPort::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    emit movingHasFinished();

    UniversalConnectionableItem::mouseReleaseEvent(event);
}
} // namespace QtNodes
