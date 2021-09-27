#include "AbstractArrow.h"
#include "ArrowPosition.h"
#include "CurvedArrow.h"
#include "EdgePoint.h"
#include "EquilateralTriangle.h"
#include "LateralHorizontalArrow.h"
#include "LateralVerticalArrow.h"
#include "LineArrow.h"
#include "PolyLineArrow.h"
#include "TinyMath.h"
#include "TreeStyleHorizontalArrow.h"
#include "TreeStyleVerticalArrow.h"
#include "scenes/FlowScene.hpp"
#include "singletones/MenuBuilder.h"
#include "graphicsComposites/universalGraphicsObject/toolbar/ToolBarWidget.h"

#include <QJsonArray>
#include <QApplication>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <vector>

using namespace QtNodes;
using namespace QtNodes::Arrows;
using namespace QtNodes::GeometricShapes;
using namespace QtNodes::Item;

enum ArrowHeadPos : quint8
{
    Direct,
    Reverse
};

const quint8 AbstractArrow::maxArrowHeadsCount_ {2};

AbstractArrow::AbstractArrow(const QPointF &newStartPoint, const QPointF &newEndPoint, const QUuid &uid,
                             QGraphicsItem* parentItem, QObject* parentObject)
: UniversalSelectableItem {uid, parentItem, parentObject}
, needToBeUpdated_ {true}
, direction_ {Direction::NoDirection}
, defaultFont_ {}
, defaultColor_ {}
, defaultBackColor_ {"#9d84ff"}
, defaultBorderColor_ {}
, defaultColorInDrawingMode_ {"#AAAAC5"}
, defaultBorderWidth_ {5}
, arrowHeads_(maxArrowHeadsCount_)
, arrowHeadSize_ {10}
, startPoint_ {std::make_shared<GeometricShapes::EdgePoint>()}
, endPoint_ {std::make_shared<GeometricShapes::EdgePoint>()}
, pen_ {defaultBackColor_}
, brush_ {defaultBackColor_}
, width_ {defaultBorderWidth_}
, hoverWidthRatio_ {1.6}
, smartPointsEnabled_ {true}
, isHovered_ {false}
, isPressed_ {false}
, smartPointsRadius_ {startPoint_->outerRadius()}
{
    setFlag(ItemIsFocusable, true);
    setAcceptHoverEvents(false);
    setVisible(true);

    setArrowType(ArrowType::NoType);
    pen_.setWidth(0);
    startPoint_->setParentItem(this);
    endPoint_->setParentItem(this);
    startPoint_->setPos(mapFromScene(newStartPoint));
    endPoint_->setPos(mapFromScene(newEndPoint));
    connect(startPoint_.get(), &GeometricShapes::EdgePoint::positionChange, this,
            &AbstractArrow::startPointPositionChangeSlot);
    connect(endPoint_.get(), &GeometricShapes::EdgePoint::positionChange, this,
            &AbstractArrow::endPointPositionChangeSlot);
}

AbstractArrow::AbstractArrow(const AbstractArrow &another)
: UniversalSelectableItem {another}
, needToBeUpdated_ {another.needToBeUpdated_}
, direction_ {another.direction_}
, defaultFont_ {another.defaultFont_}
, defaultColor_ {another.defaultColor_}
, defaultBackColor_ {another.defaultBackColor_}
, defaultBorderColor_ {another.defaultBorderColor_}
, defaultColorInDrawingMode_ {another.defaultColorInDrawingMode_}
, defaultBorderWidth_ {another.defaultBorderWidth_}
, arrowHeads_(another.arrowHeads_)
, arrowHeadSize_ {another.arrowHeadSize_}
, startPoint_ {std::make_shared<GeometricShapes::EdgePoint>(*another.startPoint_.get())}
, endPoint_ {std::make_shared<GeometricShapes::EdgePoint>(*another.endPoint_.get())}
, pen_ {another.pen_}
, brush_ {another.brush_}
, width_ {defaultBorderWidth_}
, hoverWidthRatio_ {another.hoverWidthRatio_}
, smartPointsEnabled_ {another.smartPointsEnabled_}
, isHovered_ {false}
, isPressed_ {false}
, smartPointsRadius_ {another.smartPointsRadius_}
{
}

AbstractArrow::~AbstractArrow()
{
    if (isHovered_)
    {
        QApplication::restoreOverrideCursor();
    }
}

void AbstractArrow::setPropertyCommand(const GraphicsItemProperties &property, const QVariant &value)
{
    switch (property)
    {
        case GraphicsItemProperties::BackColor:
        {
            if (value.canConvert<QColor>())
            {
                setColor(value.value<QColor>());
            }
            break;
        }
        case GraphicsItemProperties::BorderWidth:
        {
            if (value.canConvert<qint32>())
            {
                setWidth(value.toInt());
            }
            break;
        }
        case GraphicsItemProperties::ArrowDirection:
        {
            if (value.canConvert<qint32>())
            {
                const auto newDirection = static_cast<Direction>(value.toInt());
                setDirection(newDirection);
            }
            break;
        }
        default:
        {
            UniversalSelectableItem::setPropertyCommand(property, value);
            break;
        }
    }
}

std::pair<QLineF, QLineF> AbstractArrow::createPaintingLineEdges() const
{
    const qreal halfWidth {width() / 2.};

    QLineF firstEdge {reverseDirection()};
    firstEdge.setLength(firstEdge.length() - halfWidth);

    QLineF lastEdge {directDirection()};
    lastEdge.setLength(lastEdge.length() - halfWidth);

    if (arrowHeads().isEmpty())
    {
        return {firstEdge, lastEdge};
    }

    if (isDirectArrowHeadVisible())
    {
        lastEdge.setLength(lastEdge.length() - arrowHeads().first().altitude().length());
    }
    if (isReverseArrowHeadVisible())
    {
        firstEdge.setLength(firstEdge.length() - arrowHeads().last().altitude().length());
    }

    return {firstEdge, lastEdge};
}

AbstractArrow* AbstractArrow::toAnotherArrowType(const ArrowType newArrowType) const
{
    if (newArrowType == arrowType())
    {
        return clone();
    }

    AbstractArrow* ret = nullptr;
    switch (newArrowType)
    {
        case ArrowType::Line:
        {
            ret = new LineArrow(startPoint(), endPoint(), QUuid::createUuid(), parentItem(), parentObject());
            break;
        }
        case ArrowType::PolyLine:
        {
            ret = new PolyLineArrow(startPoint(), endPoint(), QUuid::createUuid(), parentItem(), parentObject());
            break;
        }
        case ArrowType::Curved:
        {
            ret = new CurvedArrow(startPoint(), endPoint(), QUuid::createUuid(), parentItem(), parentObject());
            break;
        }
        case ArrowType::LateralHorizontal:
        {
            ret = new LateralHorizontalArrow(startPoint(), endPoint(), QUuid::createUuid(), parentItem(),
                                             parentObject());
            break;
        }
        case ArrowType::LateralVertical:
        {
            ret = new LateralVerticalArrow(startPoint(), endPoint(), QUuid::createUuid(), parentItem(), parentObject());
            break;
        }
        case ArrowType::TreeStyleHorizontal:
        {
            ret = new TreeStyleHorizontalArrow(startPoint(), endPoint(), QUuid::createUuid(), parentItem(),
                                               parentObject());
            break;
        }
        case ArrowType::TreeStyleVertical:
        {
            ret = new TreeStyleVerticalArrow(startPoint(), endPoint(), QUuid::createUuid(), parentItem(),
                                             parentObject());
            break;
        }
        default:
        {
            break;
        }
    }

    if (nullptr != ret)
    {
        ret->setPen(pen());
        ret->setArrowHeadSize(arrowHeadSize_);
        ret->setDirection(direction_);
    }
    return ret;
}

bool AbstractArrow::setSmartPointsRadius(qreal newRadius)
{
    if (!smartPointsEnabled_)
    {
        return false;
    }

    bool ret = false;
    const auto &currentSmartPoints = smartPoints();

    for (const auto &currentSmartPoint : currentSmartPoints)
    {
        ret = setSmartPointRadius(currentSmartPoint, newRadius) || ret;
    }

    if (ret)
    {
        smartPointsRadius_ = newRadius;
    }
    return ret;
}

void AbstractArrow::setDirection(Direction newDirection)
{
    if (direction_ != newDirection)
    {
        direction_ = newDirection;
        const bool isDirectHeadVisible = direction_ == Direction::Direct || direction_ == Direction::Both;
        arrowHeads_[ArrowHeadPos::Direct].setVisible(isDirectHeadVisible);
        const bool isReverseHeadVisible = direction_ == Direction::Reverse || direction_ == Direction::Both;
        arrowHeads_[ArrowHeadPos::Reverse].setVisible(isReverseHeadVisible);
        synchronize();
    }
}

void AbstractArrow::setColor(const QColor &newColor)
{
    if (brush_.color() != newColor)
    {
        pen_.setColor(newColor);
        brush_.setColor(newColor);
        synchronize();
    }
}

void AbstractArrow::setWidth(qint32 newWidth)
{
    if (newWidth != width_)
    {
        width_ = newWidth;
        synchronize();
    }
}

bool AbstractArrow::setStartPoint(const QPointF &newStartPoint)
{
    if (nullptr != startPoint_ && isStartPointMovable() && newStartPoint != startPoint())
    {
        const QPointF &newStartPointInArrowCoordinates = mapFromScene(newStartPoint);
        startPoint_->setPos(newStartPointInArrowCoordinates);
        synchronize();
        return true;
    }
    return false;
}

bool AbstractArrow::setEndPoint(const QPointF &newEndPoint)
{
    if (nullptr != endPoint_ && isEndPointMovable() && newEndPoint != endPoint())
    {
        const QPointF &newEndPointInArrowCoordinates = mapFromScene(newEndPoint);
        endPoint_->setPos(newEndPointInArrowCoordinates);
        synchronize();
        return true;
    }
    return false;
}

QRectF AbstractArrow::boundingRect() const
{
    return shape_.boundingRect();
}

QPainterPath AbstractArrow::shape() const
{
    return shape_;
}

void AbstractArrow::setPen(const QPen &pen)
{
    if (pen_ != pen)
    {
        pen_ = pen;
        pen_.setWidth(0);
        setColor(pen.color());
        synchronize();
    }
}

void AbstractArrow::setBrush(const QBrush &newBrush)
{
    if (brush_ != newBrush)
    {
        brush_ = newBrush;
        setColor(brush().color());
        synchronize();
    }
}

void AbstractArrow::setArrowHeadSize(quint8 newArrowHeadSize)
{
    if (arrowHeadSize_ != newArrowHeadSize)
    {
        arrowHeadSize_ = newArrowHeadSize;
        synchronize();
    }
}

void AbstractArrow::setSmartPointsEnabled(bool newSmartPointsEnabled)
{
    if (smartPointsEnabled_ != newSmartPointsEnabled)
    {
        smartPointsEnabled_ = newSmartPointsEnabled;
        if (!smartPointsEnabled_)
        {
            hideSmartPoints();
        }
    }
}

void AbstractArrow::setSmartStartPointPositionMightBeChanged(bool mightBeChanged)
{
    if (nullptr != startPoint_)
    {
        startPoint_->setPositionMightBeChanged(mightBeChanged);
    }
}

void AbstractArrow::setSmartEndPointPositionMightBeChanged(bool mightBeChanged)
{
    if (nullptr != endPoint_)
    {
        endPoint_->setPositionMightBeChanged(mightBeChanged);
    }
}

bool AbstractArrow::isEverySmartPointEnabled() const
{
    return smartPointsEnabled_;
}

AnnulusSPtrVector AbstractArrow::smartPoints() const
{
    const AnnulusSPtrVector &currentTransitPoints = transitPoints();
    AnnulusSPtrVector ret;
    ret.reserve(smartPointsCount());
    ret.push_back(startPoint_);
    ret.insert(ret.end(), currentTransitPoints.begin(), currentTransitPoints.end());
    ret.push_back(endPoint_);
    return ret;
}

Direction AbstractArrow::direction() const
{
    return direction_;
}

QColor AbstractArrow::color() const
{
    return brush_.color();
}

qint32 AbstractArrow::width() const
{
    return width_;
}

QPen AbstractArrow::pen() const
{
    return pen_;
}

QBrush AbstractArrow::brush() const
{
    return brush_;
}

quint8 AbstractArrow::arrowHeadSize() const
{
    return arrowHeadSize_;
}

QPointF AbstractArrow::startPoint() const
{
    return nullptr != startPoint_ ? startPoint_->scenePos() : QPointF {};
}

QPointF AbstractArrow::endPoint() const
{
    return nullptr != endPoint_ ? endPoint_->scenePos() : QPointF {};
}

bool AbstractArrow::isValid() const
{
    return nullptr != startPoint_ && nullptr != endPoint_;
}

bool AbstractArrow::isMovable() const
{
    for (auto smartPoint : smartPoints())
    {
        if (smartPoint->flags().testFlag(ItemIsMovable) || smartPoint->isMovable())
        {
            return true;
        }
    }

    return false;
}

bool AbstractArrow::isPressed() const
{
    return isPressed_;
}

void AbstractArrow::updateGeometry(const QGraphicsItem* startItem, const QGraphicsItem* endItem,
                                   const ArrowPosition* position, UpdateGeometryMode mode)
{
    if (!paramsFineForUpdateGeometry(startItem, endItem, position))
    {
        return;
    }

    const bool needToUpdateStartPoint = {
    mode == UpdateGeometryMode::UpdatedStartItem || mode == UpdateGeometryMode::UpdatedBothItems || isStartPointMovable()
    };
    const bool needToUpdateEndPoint = {
    mode == UpdateGeometryMode::UpdatedEndItem || mode == UpdateGeometryMode::UpdatedBothItems || isEndPointMovable()
    };

    if (!needToUpdateStartPoint && !needToUpdateEndPoint)
    {
        return;
    }

    ///< Reset the arrow updating while new start and end points are setting.
    const bool savedCurNeedToBeUpdated = needToBeUpdated_;
    needToBeUpdated_ = false;
    const QPointF &oldStartPoint = startPoint();
    const QPointF &oldEndPoint = endPoint();

    ///< Getting new start and end points in the local coordinate system
    const QPointF &newStartPoint = startItem->mapToItem(this, position->currentStartPos);
    const QPainterPath &srcShapeInSrcCoords = startItem->shape();
    const QPainterPath &srcShapeInArrowCoords = startItem->mapToItem(this, srcShapeInSrcCoords);

    if (needToUpdateStartPoint)
    {
        setStartPointForce(mapToScene(newStartPoint));
    }

    const QPointF &newEndPoint = endItem->mapToItem(this, position->currentEndPos);
    const QPainterPath &destShapeInDestCoords = endItem->shape();
    const QPainterPath &destShapeInArrowCoords = endItem->mapToItem(this, destShapeInDestCoords);

    if (needToUpdateEndPoint)
    {
        setEndPointForce(mapToScene(newEndPoint));
    }

    if (needToUpdateStartPoint || needToUpdateEndPoint)
    {
        needToBeUpdated_ = true;
        synchronize();
        needToBeUpdated_ = false;
    }

    const auto &foundedIntersectionPoints = findIntersectionPoints(newStartPoint, newEndPoint, srcShapeInArrowCoords,
                                                                   destShapeInArrowCoords);
    bool hasPointsUpdated = false;
    const qreal error = {1e-3};

    if (needToUpdateStartPoint)
    {
        updateStartPoint(foundedIntersectionPoints.first, oldStartPoint, startItem, hasPointsUpdated, error);
    }
    if (needToUpdateEndPoint)
    {
        updateEndPoint(foundedIntersectionPoints.second, oldEndPoint, endItem, hasPointsUpdated, error);
    }

    needToBeUpdated_ = savedCurNeedToBeUpdated;

    if (hasPointsUpdated)
    {
        synchronize();
    }
}

void AbstractArrow::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    if (nullptr == event)
    {
        return;
    }
    event->accept();
    UniversalSelectableItem::contextMenuEvent(event);
    const auto arrowMenu = MenuBuilder::arrowMenu(this);
    addToolBar(arrowMenu);
    if (nullptr == arrowMenu)
    {
        return;
    }
    const QPoint &cursorPos {QCursor::pos()};
    arrowMenu->exec(cursorPos);
    arrowMenu->deleteLater();
}

QVariant AbstractArrow::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    switch (change)
    {
        case ItemSelectedHasChanged:
        {
            if (!smartPointsEnabled_)
            {
                break;
            }
            if (value.toBool())
            {
                showSmartPoints();
                break;
            }
            if (!hasSelectedSmartPoints())
            {
                hideSmartPoints();
                break;
            }
            return QGraphicsItem::itemChange(change, true);
        }
        case ItemSceneHasChanged:
        {
            emit arrowNeedToUpdateGeometry();
            break;
        }
        case ItemFlagsChange:
        {
            GraphicsItemFlags newFlags {value.toInt()};
            newFlags.setFlag(ItemIsMovable, false);
            return UniversalSelectableItem::itemChange(change, static_cast<quint32>(newFlags));
        }
        default:
        {
            break;
        }
    }
    return UniversalSelectableItem::itemChange(change, value);
}

bool AbstractArrow::hasSelectedSmartPoints() const
{
    if (!smartPointsEnabled_)
    {
        return false;
    }
    const auto &currentSmartPoints = smartPoints();

    for (const auto &smartPoint : currentSmartPoints)
    {
        if (nullptr != smartPoint && smartPoint->isSelected())
        {
            return true;
        }
    }
    return false;
}

bool AbstractArrow::isDirectArrowHeadVisible() const
{
    return arrowHeads_[ArrowHeadPos::Direct].isVisible();
}

bool AbstractArrow::isReverseArrowHeadVisible() const
{
    return arrowHeads_[ArrowHeadPos::Reverse].isVisible();
}

bool AbstractArrow::oneOfEdgePointsContains(const QPointF &point) const
{
    return startPointContains(point) || endPointContains(point);
}

bool AbstractArrow::startPointContains(const QPointF &point) const
{
    return nullptr != startPoint_ && startPoint_->isVisible() && startPoint_->contains(
    startPoint_->mapFromScene(point));
}

bool AbstractArrow::endPointContains(const QPointF &point) const
{
    return nullptr != endPoint_ && endPoint_->isVisible() && endPoint_->contains(endPoint_->mapFromScene(point));
}

void AbstractArrow::showSmartPoints()
{
    if (!smartPointsEnabled_)
    {
        return;
    }
    const auto &currentSmartPoints = smartPoints();
    for (auto smartPoint : currentSmartPoints)
    {
        if (nullptr != smartPoint)
        {
            smartPoint->setVisible(true);
        }
    }
}

void AbstractArrow::hideSmartPoints()
{
    if (!smartPointsEnabled_)
    {
        return;
    }
    const auto &currentSmartPoints = smartPoints();
    for (auto smartPoint : currentSmartPoints)
    {
        if (nullptr != smartPoint)
        {
            smartPoint->setVisible(false);
        }
    }
}

QPainterPath AbstractArrow::paintingArrow() const
{
    QPainterPath paintingArrow = paintingLine();
    for (const auto &arrowHead : arrowHeads())
    {
        if (arrowHead.isVisible())
        {
            paintingArrow.addPolygon(arrowHead.triangle());
            paintingArrow.closeSubpath();
        }
    }
    return paintingArrow;
}

EdgePointSPtr AbstractArrow::smartStartPoint() const
{
    return startPoint_;
}

EdgePointSPtr AbstractArrow::smartEndPoint() const
{
    return endPoint_;
}

EquilateralTriangleVector AbstractArrow::arrowHeads() const
{
    return arrowHeads_;
}

void AbstractArrow::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    isHovered_ = true;
    QApplication::setOverrideCursor(Qt::SizeAllCursor);
    UniversalSelectableItem::hoverEnterEvent(event);
}

void AbstractArrow::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    isHovered_ = false;
    QApplication::restoreOverrideCursor();
    UniversalSelectableItem::hoverLeaveEvent(event);
}

void AbstractArrow::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (nullptr == event)
    {
        return;
    }
    if (!isPressed_)
    {
        return;
    }
    if (!isMovable())
    {
        return;
    }

    setSelected(true);
    const QPointF &offset {event->pos() - event->lastPos()};
    const bool lastneedToBeUpdated_ = needToBeUpdated_;
    needToBeUpdated_ = false;
    emit arrowMoving(offset);
    needToBeUpdated_ = true;
    synchronize();
    needToBeUpdated_ = lastneedToBeUpdated_;
}

void AbstractArrow::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (nullptr == event)
    {
        return;
    }

    UniversalSelectableItem::mousePressEvent(event);
    if (Qt::RightButton != event->button() && !oneOfEdgePointsContains(event->scenePos()))
    {
        isPressed_ = true;
    }
    else if (!event->modifiers().testFlag(Qt::ControlModifier))
    {
        setSelected(true);
        event->accept();
    }
}

void AbstractArrow::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    isPressed_ = false;
    UniversalSelectableItem::mouseReleaseEvent(event);
}

QJsonObject AbstractArrow::save() const
{
    QJsonObject retval = UniversalSelectableItem::save();

    if (nullptr != startPoint_)
    {
        retval["startPoint"] = startPoint_->save();
    }
    if (nullptr != endPoint_)
    {
        retval["endPoint"] = endPoint_->save();
    }

    const QJsonObject &colorObject {
    {  "r", color().red()}
    , {"g", color().green()}
    , {"b", color().blue()}
    , {"a", color().alpha()}
    };

    retval["drawStyle"] = QJsonObject {
    {  "color"     , colorObject}
    , {"width"     , width()}
    , {"penStyle"  , pen_.style()}
    , {"brushStyle", brush_.style()}
    };
    retval["arrowHeadsSize"] = arrowHeadSize();
    retval["smartPointsEnabled"] = smartPointsEnabled_;
    retval["direction"] = static_cast<qint32>(direction_);
    retval["transitPoints"] = saveTransitPoints();
    return retval;
}

void AbstractArrow::restore(const QJsonObject &restoringObject)
{
    UniversalSelectableItem::restore(restoringObject);
    needToBeUpdated_ = false;

    setDirection(static_cast<Direction>(restoringObject["direction"].toInt()));

    if (nullptr != startPoint_)
    {
        const QJsonObject &startPointObject = restoringObject["startPoint"].toObject();
        startPoint_->restore(startPointObject);
    }

    if (nullptr != endPoint_)
    {
        const QJsonObject &endPointObject = restoringObject["endPoint"].toObject();
        endPoint_->restore(endPointObject);
    }

    const QJsonObject &drawStyleObject = restoringObject["drawStyle"].toObject();

    const QJsonObject &colorObject = drawStyleObject["color"].toObject();
    const QColor &color = {
    colorObject["r"].toInt(), colorObject["g"].toInt(), colorObject["b"].toInt(), colorObject["a"].toInt()
    };
    setColor(color);

    const qint32 width = {drawStyleObject["width"].toInt()};
    setWidth(width);

    const auto penStyle = static_cast<Qt::PenStyle>(drawStyleObject["penStyle"].toInt());
    pen_.setStyle(penStyle);

    const auto brushStyle = static_cast<Qt::BrushStyle>(drawStyleObject["brushStyle"].toInt());
    brush_.setStyle(brushStyle);

    const bool smartPointsEnabled = restoringObject["smartPointsEnabled"].toBool();
    setSmartPointsEnabled(smartPointsEnabled);

    const quint8 arrowHeadSize {static_cast<quint8>(restoringObject["arrowHeadsSize"].toInt())};
    setArrowHeadSize(arrowHeadSize);

    restoreTransitPoints(restoringObject);

    needToBeUpdated_ = true;
    synchronize();
}

void AbstractArrow::restoreTransitPoints(const QJsonObject &restoringObject)
{
    restoreTransitPoints(restoringObject["transitPoints"].toArray());
}

void AbstractArrow::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    if (nullptr == painter)
    {
        return;
    }
    if (!isValid())
    {
        return;
    }

    painter->save();
    setDrawingStyle(painter);
    painter->drawPath(paintingArrow());
    painter->restore();
}

void AbstractArrow::setHoverWidthRatio(qreal newHoverWidthRatio)
{
    const qreal ratioError {1e-1};
    if (newHoverWidthRatio > 0 && qAbs(newHoverWidthRatio - hoverWidthRatio_) > ratioError)
    {
        hoverWidthRatio_ = newHoverWidthRatio;
        synchronize();
    }
}

void AbstractArrow::setMovable(bool newIsMovable)
{
    for (auto smartPoint : smartPoints())
    {
        if (nullptr != smartPoint)
        {
            smartPoint->setFlag(ItemIsMovable, newIsMovable);
        }
    }
}

void AbstractArrow::resetDrawingMode()
{
    setColor(defaultBackColor_);
    setFlag(ItemIsSelectable, true);
    setSelected(true);
}

void AbstractArrow::setDrawingMode()
{
    setColor(defaultColorInDrawingMode_);
    setFlag(ItemIsSelectable, false);
}

ArrowType AbstractArrow::arrowType() const
{
    return static_cast<ArrowType>(type());
}

void AbstractArrow::addToolBar(QMenu* menu)
{
    ToolBarWidget* toolBar = new ToolBarWidget(menu);

    toolBar->setProperties(defaultFont_, defaultColor_, color(), defaultBorderColor_, width() - 1);
    toolBar->setMode(ToolBarWidget::Line);
    connect(toolBar, &ToolBarWidget::itemPropertyChanged, this, &AbstractArrow::toolBarWidgetPropertyChangedSlot);

    connect(toolBar, &ToolBarWidget::reseted, [this, toolBar]
    {
        toolBar->setProperties(defaultFont_, defaultColor_, defaultBackColor_, defaultBorderColor_,
                               defaultBorderWidth_ - 1);
        setColor(defaultBackColor_);
        setWidth(defaultBorderWidth_);
    });

    /** Some magic numbers in here **/
    const QRect &toolBarGeometry {{10, 0}, toolBar->size()};
    const quint8 menuWidthOffset {20};
    const QMargins &newContentsMargins {0, toolBar->height() - 4, 0, 0};

    toolBar->setGeometry(toolBarGeometry);
    menu->setFixedWidth(toolBar->width() + menuWidthOffset);
    menu->setContentsMargins(newContentsMargins);

    MenuBuilder::setMenuEffect(menu, true);

    toolBar->show();
}

void AbstractArrow::updateArrowHeads()
{
    if (!needToBeUpdated_)
    {
        return;
    }

    if (isDirectArrowHeadVisible())
    {
        const QLineF &directAltitude = createAltitude(directDirection());
        arrowHeads_[ArrowHeadPos::Direct].setAltitude(directAltitude);
    }

    if (isReverseArrowHeadVisible())
    {
        const QLineF &reverseAltitude = createAltitude(reverseDirection());
        arrowHeads_[ArrowHeadPos::Reverse].setAltitude(reverseAltitude);
    }
}

void AbstractArrow::updateStartPoint(const QPointF &startIntersectionPoint, const QPointF &oldStartPoint,
                                     const QGraphicsItem* startItem, bool &hasPointsUpdated, qreal error)
{
    if (nullptr == startItem)
    {
        return;
    }
    if (startIntersectionPoint.isNull())
    {
        setStartPointForce(oldStartPoint);
        return;
    }

    const QPointF &foundedSrcIntersectionPoint = mapToItem(startItem, startIntersectionPoint);
    const QRectF &srcRect = startItem->boundingRect();

    if (TinyMath::rectContainsPoint(srcRect, foundedSrcIntersectionPoint, error))
    {
        hasPointsUpdated = setStartPointForce(mapToScene(startIntersectionPoint)) || hasPointsUpdated;
    }
    else
    {
        setStartPointForce(oldStartPoint);
    }
}

void AbstractArrow::updateEndPoint(const QPointF &endIntersectionPoint, const QPointF &oldEndPoint,
                                   const QGraphicsItem* endItem, bool &hasPointsUpdated, qreal error)
{
    if (nullptr == endItem)
    {
        return;
    }
    if (endIntersectionPoint.isNull())
    {
        setEndPointForce(oldEndPoint);
        return;
    }

    const QPointF &foundedDestIntersectionPoint = mapToItem(endItem, endIntersectionPoint);
    const QRectF &destRect = endItem->boundingRect();

    if (TinyMath::rectContainsPoint(destRect, foundedDestIntersectionPoint, error))
    {
        hasPointsUpdated = setEndPointForce(mapToScene(endIntersectionPoint)) || hasPointsUpdated;
    }
    else
    {
        setEndPointForce(oldEndPoint);
    }
}

QLineF AbstractArrow::createAltitude(const QLineF &directionLine) const
{
    QLineF changingLengthLine = {directionLine.p2(), directionLine.p1()};
    changingLengthLine.setLength(arrowHeadSize());
    return QLineF {changingLengthLine.p2(), changingLengthLine.p1()};
}

void AbstractArrow::setDrawingStyle(QPainter* painter) const
{
    if (nullptr == painter)
    {
        return;
    }

    painter->setPen(Qt::PenStyle::NoPen);
    painter->setBrush(QBrush {color()});
}

bool AbstractArrow::setStartPointForce(const QPointF &newStartPoint)
{
    if (nullptr == startPoint_)
    {
        return false;
    }

    const bool sendsGeometryChanges = startPoint_->flags().testFlag(ItemSendsGeometryChanges);
    const bool hasSignalsBlocked = signalsBlocked();
    const bool isPointMovable = isStartPointMovable();

    startPoint_->setFlag(ItemSendsGeometryChanges, false);
    blockSignals(true);
    setStartPointMovable(true);

    const bool ret = setStartPoint(newStartPoint);

    setStartPointMovable(isPointMovable);
    blockSignals(hasSignalsBlocked);
    startPoint_->setFlag(ItemSendsGeometryChanges, sendsGeometryChanges);

    return ret;
}

bool AbstractArrow::setEndPointForce(const QPointF &newEndPoint)
{
    if (nullptr == endPoint_)
    {
        return false;
    }

    const bool sendsGeometryChanges = endPoint_->flags().testFlag(ItemSendsGeometryChanges);
    const bool hasSignalsBlocked = signalsBlocked();
    const bool isPointMovable = isEndPointMovable();

    endPoint_->setFlag(ItemSendsGeometryChanges, false);
    blockSignals(true);
    setEndPointMovable(true);

    const bool ret = setEndPoint(newEndPoint);

    setEndPointMovable(isPointMovable);
    blockSignals(hasSignalsBlocked);
    endPoint_->setFlag(ItemSendsGeometryChanges, sendsGeometryChanges);

    return ret;
}

void AbstractArrow::toolBarWidgetPropertyChangedSlot(const GraphicsItemProperties &property, const QVariant &value)
{
    setProperty(property, value);
}

void AbstractArrow::startPointPositionChangeSlot(const QPointF &newPosInSceneCoords, const QPointF &mouseOffset)
{
    if (isStartPointMovable())
    {
        emit startPointPositionChange(newPosInSceneCoords, mouseOffset);
    }
}

void AbstractArrow::endPointPositionChangeSlot(const QPointF &newPosInSceneCoords, const QPointF &mouseOffset)
{
    if (isEndPointMovable())
    {
        emit endPointPositionChange(newPosInSceneCoords, mouseOffset);
    }
}

QVariant AbstractArrow::property(const GraphicsItemProperties &property) const
{
    switch (property)
    {
        case GraphicsItemProperties::BackColor:
        {
            return color();
        }
        case GraphicsItemProperties::BorderWidth:
        {
            return width();
        }
        case GraphicsItemProperties::ArrowDirection:
        {
            return static_cast<qint32>(direction());
        }
        default:
        {
            break;
        }
    }
    return UniversalSelectableItem::property(property);
}

qreal AbstractArrow::smartPointsRadius() const
{
    return smartPointsRadius_;
}

quint8 AbstractArrow::smartPointsCount() const
{
    return transitPointsCount() + TinyMath::minLinePointsCount();
}

quint8 AbstractArrow::transitPointsCount() const
{
    return static_cast<quint8>(transitPoints().size());
}

void AbstractArrow::synchronize()
{
    if (needToBeUpdated_)
    {
        prepareGeometryChange();
        updateArrowHeads();
        shape_ = paintingArrow();

        const qreal hoverWidth = {width_ * hoverWidthRatio_};
        TinyMath::setPathWidth(shape_, hoverWidth);
    }
}

void AbstractArrow::setStartPointMovable(bool newMovable)
{
    if (nullptr == startPoint_)
    {
        return;
    }

    startPoint_->setFlag(ItemIsMovable, newMovable);
    if (startPoint_->flags().testFlag(ItemIsMovable))
    {
        emit arrowNeedToUpdateGeometry(UpdateGeometryMode::UpdatedStartItem);
    }
}

void AbstractArrow::setEndPointMovable(bool newMovable)
{
    if (nullptr == endPoint_)
    {
        return;
    }

    endPoint_->setFlag(ItemIsMovable, newMovable);
    if (startPoint_->flags().testFlag(ItemIsMovable))
    {
        emit arrowNeedToUpdateGeometry(UpdateGeometryMode::UpdatedEndItem);
    }
}

bool AbstractArrow::isStartPointMovable() const
{
    return nullptr != startPoint_ ? startPoint_->flags().testFlag(ItemIsMovable) : false;
}

bool AbstractArrow::isEndPointMovable() const
{
    return nullptr != endPoint_ ? endPoint_->flags().testFlag(ItemIsMovable) : false;
}

bool AbstractArrow::paramsFineForUpdateGeometry(const QGraphicsItem* startItem, const QGraphicsItem* endItem,
                                                const ArrowPosition* position) const
{
    if (nullptr == startItem)
    {
        return false;
    }
    if (nullptr == endItem)
    {
        return false;
    }
    if (nullptr == position)
    {
        return false;
    }
    if (!isValid())
    {
        return false;
    }

    const QGraphicsScene* const startItemScene = startItem->scene();
    const QGraphicsScene* const endItemScene = endItem->scene();
    const QGraphicsScene* const arrowScene = scene();

    if (startItemScene != endItemScene)
    {
        return false;
    }
    if (startItemScene != arrowScene)
    {
        return false;
    }
    if (endItemScene != arrowScene)
    {
        return false;
    }
    if (nullptr == arrowScene)
    {
        return false;
    }
    return true;
}

bool AbstractArrow::setSmartPointRadius(AnnulusSPtr smartPoint, qreal newRadius)
{
    if (nullptr != smartPoint)
    {
        const qreal newOuterRadius = {newRadius};
        const qreal newInnerRadius = {smartPoint->defaultRadiiRatio() * newOuterRadius};
        return (smartPoint->setOuterRadius(newOuterRadius) && smartPoint->setInnerRadius(newInnerRadius));
    }

    return false;
}

void AbstractArrow::setArrowType(ArrowType type)
{
    type_ = static_cast<qint32>(type);
}
