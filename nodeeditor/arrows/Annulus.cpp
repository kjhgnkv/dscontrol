#include "Annulus.h"

#include "factories/CommandFactory.hpp"
#include "utils/undo/ChangeItemPositionCommand.hpp"
#include "Circle.h"
#include "models/NodesScene.hpp"
#include "singletones/Facade.hpp"
#include "singletones/UndoController.hpp"

#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QMetaMethod>
#include <QPainter>

using namespace QtNodes;
using namespace QtNodes::GeometricShapes;

Annulus::Annulus(const QPointF &centerPoint, const QUuid &id, QGraphicsItem* parentItem, QObject* parentObject)
: UniversalMovableItem {id, parentItem, parentObject}
, defaultOuterRadius_ {4.0}
, defaultInnerRadius_ {3.0}
, defaultRadiiRatio_ {defaultInnerRadius_ / defaultOuterRadius_}
, outerCircle_ {std::make_shared<Circle>(defaultOuterRadius_)}
, innerCircle_ {std::make_shared<Circle>(defaultInnerRadius_)}
, outerColor_ {"#3B3E3D"}
, innerColor_ {"#FFFFFF"}
, isHovered_ {false}
{
    type_ = static_cast<qint32>(ShapeType::Annulus);
    setPos(centerPoint);
    setUndoRedoPositionEnable(true);
    setFlag(ItemIsSelectable, false);
    setFlag(ItemSendsGeometryChanges, true);
    setAcceptHoverEvents(true);
}

Annulus::Annulus(const Annulus &another)
: UniversalMovableItem {another}
, defaultOuterRadius_ {another.defaultOuterRadius_}
, defaultInnerRadius_ {another.defaultInnerRadius_}
, defaultRadiiRatio_ {another.defaultRadiiRatio_}
, outerCircle_ {another.outerCircle_}
, innerCircle_ {another.innerCircle_}
, outerColor_ {another.outerColor_}
, innerColor_ {another.innerColor_}
, isHovered_ {another.isHovered_}
{
}

Annulus::~Annulus()
{
    if (isHovered_)
    {
        QApplication::restoreOverrideCursor();
    }
}

qreal Annulus::outerRadius() const
{
    if (nullptr != outerCircle_)
    {
        return outerCircle_->radius();
    }
    return 0;
}

qreal Annulus::innerRadius() const
{
    if (nullptr != innerCircle_)
    {
        return innerCircle_->radius();
    }
    return 0;
}

QColor Annulus::outerColor() const
{
    return outerColor_;
}

QColor Annulus::innerColor() const
{
    return innerColor_;
}

const std::shared_ptr<Circle> Annulus::outerCircle() const
{
    return outerCircle_;
}

const std::shared_ptr<Circle> Annulus::innerCircle() const
{
    return innerCircle_;
}

bool Annulus::setOuterRadius(qreal newOuterRarius)
{
    prepareGeometryChange();
    if (nullptr != outerCircle_)
    {
        return outerCircle_->setRadius(newOuterRarius);
    }
    outerCircle_ = std::make_shared<Circle>(newOuterRarius, pos());
    return true;
}

void Annulus::setOuterColor(const QColor &newOuterColor)
{
    outerColor_ = newOuterColor;
}

void Annulus::setInnerColor(const QColor &newInnerColor)
{
    innerColor_ = newInnerColor;
}

bool Annulus::setInnerRadius(double newInnerRarius)
{
    prepareGeometryChange();
    if (nullptr != innerCircle_)
    {
        return innerCircle_->setRadius(newInnerRarius);
    }
    innerCircle_ = std::make_shared<Circle>(newInnerRarius, pos());
    return true;
}

QJsonObject Annulus::save() const
{
    QJsonObject retval = UniversalMovableItem::save();
    if (nullptr != outerCircle_)
    {
        retval["outerCircle"] = outerCircle_->save();
    }
    if (nullptr != innerCircle_)
    {
        retval["innerCircle"] = innerCircle_->save();;
    }
    return retval;
}

void Annulus::restore(const QJsonObject &restoringObject)
{
    UniversalMovableItem::restore(restoringObject);
    if (nullptr == outerCircle_)
    {
        outerCircle_ = std::make_shared<Circle>();
    }
    outerCircle_->restore(restoringObject["outerCircle"].toObject());

    if (nullptr == innerCircle_)
    {
        innerCircle_ = std::make_shared<Circle>();
    }
    innerCircle_->restore(restoringObject["innerCircle"].toObject());
    setVisible(false);
}

void Annulus::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if (nullptr == painter)
    {
        return;
    }
    if (nullptr == outerCircle_)
    {
        return;
    }
    if (nullptr == innerCircle_)
    {
        return;
    }

    painter->save();
    QPen pen {outerColor_};
    const qreal outRadius = {outerRadius()};
    const qreal inRadius = {innerRadius()};
    const qreal penWidth = {outRadius - inRadius};
    pen.setWidthF(penWidth);
    painter->setPen(pen);
    painter->setBrush({innerColor_, Qt::BrushStyle::SolidPattern});
    const QPointF &centerInLocalCoordinates {0, 0};
    painter->drawEllipse(centerInLocalCoordinates, inRadius, inRadius);
    painter->restore();
}

QRectF Annulus::boundingRect() const
{
    return shape().boundingRect();
}

QPainterPath Annulus::shape() const
{
    QPainterPath ret;
    const QPointF &centerInLocalCoordinates {0, 0};
    const qreal outRadius = outerRadius();
    ret.addEllipse(centerInLocalCoordinates, outRadius, outRadius);
    return ret;
}

bool Annulus::contains(const QPointF &point) const
{
    if (nullptr != outerCircle_)
    {
        return outerCircle_->contains(point);
    }
    if (nullptr != innerCircle_)
    {
        return innerCircle_->contains(point);
    }
    return false;
}

qreal Annulus::defaultRadiiRatio() const
{
    return defaultRadiiRatio_;
}

QVariant Annulus::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    switch (change)
    {
        case ItemPositionHasChanged:
        {
            emit positionHasChanged();
            break;
        }
        case ItemVisibleChange:
        {
            const bool newVisible = value.toBool() && isEnabled();
            return UniversalSelectableItem::itemChange(change, newVisible);
        }
        case ItemVisibleHasChanged:
        {
            if (!value.toBool() && isHovered_)
            {
                QApplication::restoreOverrideCursor();
                isHovered_ = false;
            }
            break;
        }
        case ItemEnabledHasChanged:
        {
            if (!value.toBool())
            {
                setVisible(false);
            }
            break;
        }
        case ItemParentHasChanged:
        {
            const QGraphicsItem* const parent = parentItem();
            bool newIsVisible = false;
            if (nullptr != parent)
            {
                newIsVisible = parent->isSelected();
            }
            setVisible(newIsVisible);
            break;
        }
        default:
        {
            break;
        }
    }
    return UniversalSelectableItem::itemChange(change, value);
}

void Annulus::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    isHovered_ = true;
    QApplication::setOverrideCursor(Qt::PointingHandCursor);
    UniversalSelectableItem::hoverEnterEvent(event);
}

void Annulus::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    isHovered_ = false;
    QApplication::restoreOverrideCursor();
    UniversalSelectableItem::hoverLeaveEvent(event);
}
