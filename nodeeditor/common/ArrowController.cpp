#include "ArrowController.h"

#include "graphicsComposites/universalGraphicsObject/UniversalConnectionableItem.h"
#include "arrows/AbstractArrow.h"
#include "arrows/ArrowPosition.h"
#include "arrows/CurvedArrow.h"
#include "arrows/SpecialStyleArrow.h"
#include "arrows/TinyMath.h"

#include "singletones/Facade.hpp"
#include "models/NodesScene.hpp"
#include "views/FlowView.hpp"

#include <QDebug>

using namespace QtNodes;
using namespace QtNodes::Arrows;

enum class GeometryChangingType
{
    Moving,
    Resizing
};

ArrowController::ArrowController(UniversalConnectionableItem* source, UniversalConnectionableItem* destination,
                                 Arrows::AbstractArrow* arrow)
: source_ {source}
, destination_ {destination}
, arrow_ {arrow}
{
    arrowPosition_ = new ArrowPosition;
    setArrowPositionToDefault();

    if (nullptr != arrow_)
    {
        arrow_->updateGeometry(source_, destination_, arrowPosition_);
        arrow_->setAcceptHoverEvents(true);
        if (ArrowType::Curved == arrow_->arrowType())
        {
            arrow_->resetLine();
        }
        flowViewScaleChangedSlot();
    }
    if (nullptr != source_)
    {
        previousSourceRect_ = source_->mapToScene(source_->boundingRect()).boundingRect();
    }
    if (nullptr != destination_)
    {
        previousDestRect_ = destination_->mapToScene(destination_->boundingRect()).boundingRect();
    }

    updateLastArrowPositionSlot();
    connectSource();
    connectDestination();
    connectArrow();
}

ArrowController::~ArrowController()
{
    delete arrowPosition_;
    arrowPosition_ = nullptr;
}

AbstractArrow* ArrowController::arrow() const
{
    return arrow_;
}

void ArrowController::update(UpdateGeometryMode mode)
{
    if (nullptr != arrow_)
    {
        arrow_->updateGeometry(source_, destination_, arrowPosition_, mode);
    }
}

QPointF ArrowController::calculateCurvedArrowNewEdgePointPos(const QRectF &rect, const QPointF &mouseOffset,
                                                             const QPointF &curEdgePointPos) const
{
    const qreal epsilon = {1e-4};
    const RectEdge rectEdge = TinyMath::determineRectEdge(rect.toRect(), curEdgePointPos.toPoint());
    QPointF newEdgePointPos = curEdgePointPos;
    switch (rectEdge)
    {
        case RectEdge::Left:
        case RectEdge::Right:
        {
            const qreal mouseOffsetY = {mouseOffset.y()};
            if (mouseOffsetY == 0)
            {
                break;
            }
            newEdgePointPos.setY(newEdgePointPos.y() + mouseOffsetY);
            if (!TinyMath::rectContainsPoint(rect, newEdgePointPos, epsilon))
            {
                const qreal newY = {mouseOffsetY < 0 ? rect.top() : rect.bottom()};
                newEdgePointPos.setY(newY);
            }
            break;
        }
        case RectEdge::Top:
        case RectEdge::Bottom:
        {
            const qreal mouseOffsetX = {mouseOffset.x()};
            if (mouseOffsetX == 0)
            {
                break;
            }
            newEdgePointPos.setX(newEdgePointPos.x() + mouseOffsetX);
            if (!TinyMath::rectContainsPoint(rect, newEdgePointPos, epsilon))
            {
                const qreal newX = {mouseOffsetX < 0 ? rect.left() : rect.right()};
                newEdgePointPos.setX(newX);
            }
            break;
        }
        case RectEdge::TopLeft:
        {
            const qreal mouseOffsetX = {mouseOffset.x()};
            if (mouseOffsetX > 0)
            {
                newEdgePointPos.setX(newEdgePointPos.x() + mouseOffsetX);
            }
            const qreal mouseOffsetY = {mouseOffset.y()};
            if (mouseOffsetY > 0)
            {
                newEdgePointPos.setY(newEdgePointPos.y() + mouseOffsetY);
            }
            if (!TinyMath::rectContainsPoint(rect, newEdgePointPos, epsilon))
            {
                newEdgePointPos = rect.topLeft();
            }
            break;
        }
        case RectEdge::TopRight:
        {
            const qreal mouseOffsetX = {mouseOffset.x()};
            if (mouseOffset.x() < 0)
            {
                newEdgePointPos.setX(newEdgePointPos.x() + mouseOffsetX);
            }
            const qreal mouseOffsetY = {mouseOffset.y()};
            if (mouseOffset.y() > 0)
            {
                newEdgePointPos.setY(newEdgePointPos.y() + mouseOffsetY);
            }
            if (!TinyMath::rectContainsPoint(rect, newEdgePointPos, epsilon))
            {
                newEdgePointPos = rect.topRight();
            }
            break;
        }
        case RectEdge::BottomLeft:
        {
            const qreal mouseOffsetX = {mouseOffset.x()};
            if (mouseOffset.x() > 0)
            {
                newEdgePointPos.setX(newEdgePointPos.x() + mouseOffsetX);
            }
            const qreal mouseOffsetY = {mouseOffset.y()};
            if (mouseOffset.y() < 0)
            {
                newEdgePointPos.setY(newEdgePointPos.y() + mouseOffsetY);
            }
            if (!TinyMath::rectContainsPoint(rect, newEdgePointPos, epsilon))
            {
                newEdgePointPos = rect.bottomLeft();
            }
            break;
        }
        case RectEdge::BottomRight:
        {
            const qreal mouseOffsetX = {mouseOffset.x()};
            if (mouseOffset.x() < 0)
            {
                newEdgePointPos.setX(newEdgePointPos.x() + mouseOffsetX);
            }
            const qreal mouseOffsetY = {mouseOffset.y()};
            if (mouseOffset.y() < 0)
            {
                newEdgePointPos.setY(newEdgePointPos.y() + mouseOffsetY);
            }
            if (!TinyMath::rectContainsPoint(rect, newEdgePointPos, epsilon))
            {
                newEdgePointPos = rect.bottomRight();
            }
            break;
        }
        default:
        {
            break;
        }
    }
    return newEdgePointPos;
}

void ArrowController::curvedArrowStartPointPosChangeOption(QRectF srcRect, const QPointF &curStartPointSrcPos,
                                                           const QPointF &mouseOffset, qreal epsilon)
{
    if (nullptr == source_ || nullptr == arrow_)
    {
        return;
    }

    const QPointF &newStartPointSrcPos = calculateCurvedArrowNewEdgePointPos(srcRect, mouseOffset, curStartPointSrcPos);

    if (newStartPointSrcPos != curStartPointSrcPos && TinyMath::rectContainsPoint(srcRect, newStartPointSrcPos,
                                                                                  epsilon))
    {
        arrow_->setStartPointForce(source_->mapToScene(newStartPointSrcPos));
        updateStartArrowPosition(newStartPointSrcPos);
    }
    else
    {
        arrow_->setSmartStartPointPositionMightBeChanged(false);
    }
}

void ArrowController::curvedArrowEndPointPosChangeOption(QRectF destRect, const QPointF &curEndPointDestPos,
                                                         const QPointF &mouseOffset, qreal epsilon)
{
    if (nullptr == destination_ || nullptr == arrow_)
    {
        return;
    }

    const QPointF &newEndPointDestPos = calculateCurvedArrowNewEdgePointPos(destRect, mouseOffset, curEndPointDestPos);

    if (newEndPointDestPos != curEndPointDestPos && TinyMath::rectContainsPoint(destRect, newEndPointDestPos, epsilon))
    {
        arrow_->setEndPointForce(destination_->mapToScene(newEndPointDestPos));
        updateEndArrowPosition(newEndPointDestPos);
    }
    else
    {
        arrow_->setSmartEndPointPositionMightBeChanged(false);
    }
}

UniversalConnectionableItem* ArrowController::source() const
{
    return source_;
}

UniversalConnectionableItem* ArrowController::destination() const
{
    return destination_;
}

bool ArrowController::operator==(const ArrowController &rhs) const
{
    return *this->arrow_ == *rhs.arrow();
}

bool ArrowController::operator!=(const ArrowController &rhs) const
{
    return *this->arrow_ != *rhs.arrow();
}

bool ArrowController::operator<=(const ArrowController &rhs) const
{
    return *this->arrow_ <= *rhs.arrow_;
}

bool ArrowController::operator>=(const ArrowController &rhs) const
{
    return *this->arrow_ >= *rhs.arrow();
}

bool ArrowController::operator>(const ArrowController &rhs) const
{
    return *this->arrow_ > *rhs.arrow();
}

bool ArrowController::operator<(const ArrowController &rhs) const
{
    return *this->arrow_ < *rhs.arrow();
}

bool ArrowController::setArrow(AbstractArrow* newArrow)
{
    if (newArrow == arrow_)
    {
        return false;
    }

    if (nullptr != arrow_)
    {
        arrow_->disconnect();
    }

    arrow_ = newArrow;

    if (nullptr != arrow_)
    {
        const auto specialStyleArrow = dynamic_cast<const SpecialStyleArrow*>(arrow_);
        if (nullptr != specialStyleArrow)
        {
            setArrowPositionToDefault();
        }

        connectArrow();
        arrow_->setStartPoint(source_->mapToScene(arrowPosition_->currentStartPos));
        arrow_->setEndPoint(destination_->mapToScene(arrowPosition_->currentEndPos));

        arrow_->updateGeometry(source_, destination_, arrowPosition_);
        arrow_->setAcceptHoverEvents(true);
        flowViewScaleChangedSlot();
    }
    return true;
}

void ArrowController::arrowHasDestroyedSlot()
{
    const QObject* const destroyedArrow = sender();
    if (nullptr != destroyedArrow && destroyedArrow == arrow_)
    {
        emit destroyMe();
    }
}

void ArrowController::destroyArrowSlot()
{
    const auto destroyingArrow = qobject_cast<const AbstractArrow*>(sender());
    if (nullptr == destroyingArrow || destroyingArrow != arrow_)
    {
        return;
    }

    const DataFlowModel* const dataFlowModel = Facade::Instance().model();
    if (nullptr == dataFlowModel)
    {
        return;
    }

    const auto connections = dataFlowModel->connections();
    const auto founded = std::find_if(connections.begin(), connections.end(), [this](const auto value)
    {
        return arrow_->id() == value.first.id_;
    });
    if (connections.end() == founded)
    {
        emit destroyMe();
    }

}

void ArrowController::arrowMovingSlot(const QPointF &offset)
{
    if (offset.isNull())
    {
        return;
    }

    const auto movingArrow = qobject_cast<const AbstractArrow*>(sender());
    if (nullptr == movingArrow || movingArrow != arrow_)
    {
        return;
    }

    const QPointF &newStartPos = arrow_->isStartPointMovable() ? arrowPosition_->currentStartPos + offset : arrowPosition_->currentStartPos;

    const QPointF &newEndPos = arrow_->isEndPointMovable() ? arrowPosition_->currentEndPos + offset : arrowPosition_->currentEndPos;

    const bool sourceContains = source_->contains(newStartPos);
    const bool destinationContains = destination_->contains(newEndPos);

    if (!sourceContains && !destinationContains)
    {
        return;
    }

    bool isUpdated = false;
    if (sourceContains)
    {
        isUpdated = updateStartArrowPosition(newStartPos);
    }
    if (destinationContains)
    {
        isUpdated = updateEndArrowPosition(newEndPos) || isUpdated;
    }
    if (isUpdated)
    {
        update(UpdateGeometryMode::UpdatedNoItems);
    }
}

void ArrowController::arrowNeedToUpdateGeometrySlot(UpdateGeometryMode mode)
{
    if (qobject_cast<const AbstractArrow*>(sender()) == arrow_)
    {
        update(mode);
    }
}

void ArrowController::sourceItemGeometryChangedSlot()
{
    if (nullptr == arrow_ || nullptr == arrowPosition_ || nullptr == source_ || sender() != source_)
    {
        return;
    }

    const QRectF &curSrcRect = source_->boundingRect();
    const QRectF &curSrcSceneRect = source_->mapToScene(curSrcRect).boundingRect();

    if (curSrcSceneRect == previousSourceRect_)
    {
        return;
    }

    const QSizeF &prevSrcRectSize = previousSourceRect_.size();
    const QSizeF &curSrcRectSize = curSrcSceneRect.size();
    const GeometryChangingType geometryChangingType = prevSrcRectSize == curSrcRectSize ? GeometryChangingType::Moving : GeometryChangingType::Resizing;

    if (GeometryChangingType::Resizing == geometryChangingType)
    {
        srcResizedOption(curSrcRect, curSrcSceneRect);
    }

    previousSourceRect_ = curSrcSceneRect;
    const bool isSpecialStyleArrow = {nullptr != dynamic_cast<SpecialStyleArrow*>(arrow_)};
    const UpdateGeometryMode mode = isSpecialStyleArrow ? UpdateGeometryMode::UpdatedStartItem : determineMode();

    if (mode != UpdateGeometryMode::UpdatedStartItem && mode != UpdateGeometryMode::UpdatedBothItems && arrow_->scene() == source_->scene())
    {
        arrow_->setStartPointForce(source_->mapToScene(arrowPosition_->lastStartPos));
    }

    arrow_->updateGeometry(source_, destination_, arrowPosition_, mode);
    updateLastArrowPositionSlot();
}

void ArrowController::destItemGeometryChangedSlot()
{
    if (nullptr == arrow_ || nullptr == arrowPosition_ || nullptr == destination_ || sender() != destination_)
    {
        return;
    }

    const QRectF &curDestRect {destination_->boundingRect()};
    const QRectF &curDestSceneRect {destination_->mapToScene(curDestRect).boundingRect()};

    if (curDestSceneRect == previousDestRect_)
    {
        return;
    }

    const GeometryChangingType geometryChangingType = previousDestRect_.size() == curDestSceneRect.size() ? GeometryChangingType::Moving : GeometryChangingType::Resizing;

    if (GeometryChangingType::Resizing == geometryChangingType)
    {
        destResizedOption(curDestRect, curDestSceneRect);
    }

    previousDestRect_ = curDestSceneRect;
    const bool isSpecialStyleArrow = {nullptr != dynamic_cast<SpecialStyleArrow*>(arrow_)};
    const UpdateGeometryMode mode = isSpecialStyleArrow ? UpdateGeometryMode::UpdatedEndItem : determineMode();

    if (mode != UpdateGeometryMode::UpdatedEndItem && mode != UpdateGeometryMode::UpdatedBothItems && arrow_->scene() == destination_->scene())
    {
        arrow_->setEndPointForce(destination_->mapToScene(arrowPosition_->lastEndPos));
    }

    arrow_->updateGeometry(source_, destination_, arrowPosition_, mode);
    updateLastArrowPositionSlot();
}

void ArrowController::itemHasDestroyedSlot()
{
    const QObject* const destroyedItem = sender();
    if (nullptr != destroyedItem && (destroyedItem == source_ || destroyedItem == destination_))
    {
        emit destroyMe();
    }
}

void ArrowController::arrowStartPointPositionChangeSlot(const QPointF &, const QPointF &mouseOffset)
{
    if (sender() != arrow_ || nullptr == arrow_ || nullptr == source_)
    {
        return;
    }

    const QGraphicsScene* const srcScene = source_->scene();
    const QGraphicsScene* const arrowScene = arrow_->scene();

    if (srcScene != arrowScene || nullptr == srcScene)
    {
        return;
    }
    if (mouseOffset.isNull())
    {
        arrow_->setSmartStartPointPositionMightBeChanged(false);
        return;
    }

    const QPointF &curStartPointScenePos = arrow_->startPoint();
    const QPointF &curStartPointSrcPos = source_->mapFromScene(curStartPointScenePos);
    const QRectF &srcRect = source_->boundingRect();
    const qreal epsilon = {1e-4};
    const bool srcContainsCur = TinyMath::rectContainsPoint(srcRect, curStartPointSrcPos, epsilon);

    if (!srcContainsCur)
    {
        arrow_->setSmartStartPointPositionMightBeChanged(false);
        return;
    }

    if (ArrowType::Curved == arrow_->arrowType())
    {
        return curvedArrowStartPointPosChangeOption(srcRect, curStartPointSrcPos, mouseOffset, epsilon);
    }

    QPointF newStartPointSrcPos = curStartPointSrcPos + mouseOffset;
    bool srcContainsNew = TinyMath::rectContainsPoint(srcRect, newStartPointSrcPos, epsilon);

    for (int i {0}; !srcContainsNew && i < 2; i++)
    {
        newStartPointSrcPos = curStartPointSrcPos;
        switch (i)
        {
            case 0:
            {
                newStartPointSrcPos.setX(curStartPointSrcPos.x() + mouseOffset.x());
                break;
            }
            case 1:
            {
                newStartPointSrcPos.setY(curStartPointSrcPos.y() + mouseOffset.y());
                break;
            }
            default:
            {
                break;
            }
        }
        srcContainsNew = TinyMath::rectContainsPoint(srcRect, newStartPointSrcPos, epsilon);
    }

    if (!srcContainsNew)
    {
        arrow_->setSmartStartPointPositionMightBeChanged(false);
        return;
    }

    if (updateStartArrowPosition(newStartPointSrcPos))
    {
        update(UpdateGeometryMode::UpdatedNoItems);
    }
}

void ArrowController::arrowEndPointPositionChangeSlot(const QPointF &, const QPointF &mouseOffset)
{
    if (sender() != arrow_ || nullptr == arrow_ || nullptr == destination_)
    {
        return;
    }

    const QGraphicsScene* const destScene = destination_->scene();
    const QGraphicsScene* const arrowScene = arrow_->scene();

    if (destScene != arrowScene || nullptr == destScene)
    {
        return;
    }
    if (mouseOffset.isNull())
    {
        arrow_->setSmartEndPointPositionMightBeChanged(false);
        return;
    }

    const QPointF &curEndPointScenePos = arrow_->endPoint();
    const QPointF &curEndPointDestPos = destination_->mapFromScene(curEndPointScenePos);
    const QRectF &destRect = destination_->boundingRect();
    const qreal epsilon = {1e-4};
    const bool srcContainsCur = TinyMath::rectContainsPoint(destRect, curEndPointDestPos, epsilon);

    if (!srcContainsCur)
    {
        arrow_->setSmartEndPointPositionMightBeChanged(false);
        return;
    }

    if (ArrowType::Curved == arrow_->arrowType())
    {
        return curvedArrowEndPointPosChangeOption(destRect, curEndPointDestPos, mouseOffset, epsilon);
    }

    QPointF newEndPointDestPos = curEndPointDestPos + mouseOffset;
    bool destContainsNew = TinyMath::rectContainsPoint(destRect, newEndPointDestPos, epsilon);

    for (int i {0}; !destContainsNew && i < 2; i++)
    {
        newEndPointDestPos = curEndPointDestPos;
        switch (i)
        {
            case 0:
            {
                newEndPointDestPos.setX(curEndPointDestPos.x() + mouseOffset.x());
                break;
            }
            case 1:
            {
                newEndPointDestPos.setY(curEndPointDestPos.y() + mouseOffset.y());
                break;
            }
            default:
            {
                break;
            }
        }
        destContainsNew = TinyMath::rectContainsPoint(destRect, newEndPointDestPos, epsilon);
    }

    if (!destContainsNew)
    {
        arrow_->setSmartEndPointPositionMightBeChanged(false);
        return;
    }

    if (updateEndArrowPosition(newEndPointDestPos))
    {
        update(UpdateGeometryMode::UpdatedNoItems);
    }
}

void ArrowController::connectArrow()
{
    if (nullptr == arrow_)
    {
        return;
    }

    connect(arrow_, &AbstractArrow::arrowMoving, this, &ArrowController::arrowMovingSlot);
    connect(arrow_, &AbstractArrow::destroyed, this, &ArrowController::arrowHasDestroyedSlot);
    connect(arrow_, &AbstractArrow::destroyMe, this, &ArrowController::destroyArrowSlot);
    connect(arrow_, &AbstractArrow::arrowNeedToUpdateGeometry, this, &ArrowController::arrowNeedToUpdateGeometrySlot);
    connect(arrow_, &AbstractArrow::startPointPositionChange, this,
            &ArrowController::arrowStartPointPositionChangeSlot);
    connect(arrow_, &AbstractArrow::endPointPositionChange, this, &ArrowController::arrowEndPointPositionChangeSlot);
    connect(arrow_, &AbstractArrow::needToUpdateLastArrowPosition, this, &ArrowController::updateLastArrowPositionSlot);

    const NodesScene* const sceneModel = Facade::Instance().sceneModel();
    if (nullptr == sceneModel)
    {
        return;
    }

    const FlowView* const topView = sceneModel->getTopView();
    if (nullptr == topView)
    {
        return;
    }
    connect(topView, &FlowView::scaleChanged, this, &ArrowController::flowViewScaleChangedSlot);
}

void ArrowController::connectSource()
{
    if (nullptr != source_)
    {
        connect(source_, &UniversalConnectionableItem::geometryChanged, this,
                &ArrowController::sourceItemGeometryChangedSlot);
        connect(source_, &UniversalConnectionableItem::destroyed, this, &ArrowController::itemHasDestroyedSlot);
        connect(source_, &UniversalConnectionableItem::sceneHasChanged, this, &ArrowController::destroyMe);
    }
}

void ArrowController::connectDestination()
{
    if (nullptr != destination_)
    {
        connect(destination_, &UniversalConnectionableItem::geometryChanged, this,
                &ArrowController::destItemGeometryChangedSlot);
        connect(destination_, &UniversalConnectionableItem::destroyed, this, &ArrowController::itemHasDestroyedSlot);
        connect(destination_, &UniversalConnectionableItem::sceneHasChanged, this, &ArrowController::destroyMe);
    }
}

void ArrowController::setArrowPositionToDefault()
{
    if (nullptr == arrowPosition_)
    {
        arrowPosition_ = new ArrowPosition;
    }

    if (nullptr != source_)
    {
        arrowPosition_->currentStartPos = source_->boundingRect().center();
    }
    else
    {
        arrowPosition_->currentStartPos = {};
    }

    if (nullptr != destination_)
    {
        arrowPosition_->currentEndPos = destination_->boundingRect().center();
    }
    else
    {
        arrowPosition_->currentEndPos = {};
    }

    const QPointF &defaultRatio {0.5, 0.5};
    arrowPosition_->endRelativeRatio = defaultRatio;
    arrowPosition_->startRelativeRatio = defaultRatio;
    arrowPosition_->lastStartPos = arrowPosition_->currentStartPos;
    arrowPosition_->lastEndPos = arrowPosition_->currentEndPos;
}

bool ArrowController::updateStartArrowPosition(const QPointF &newStartArrowPosition)
{
    bool ret = false;

    if (nullptr == arrowPosition_)
    {
        return ret;
    }
    if (arrowPosition_->currentStartPos != newStartArrowPosition)
    {
        ret = true;
        arrowPosition_->currentStartPos = newStartArrowPosition;
    }

    const QRectF &srcRect = source_->boundingRect();
    const qreal newStartRelativeRatioX = {(arrowPosition_->currentStartPos.x() - srcRect.x()) / srcRect.width()};
    const qreal newStartRelativeRatioY = {(arrowPosition_->currentStartPos.y() - srcRect.y()) / srcRect.height()};
    const QPointF &newStartRelativeRatioPoint {newStartRelativeRatioX, newStartRelativeRatioY};

    if (arrowPosition_->startRelativeRatio != newStartRelativeRatioPoint)
    {
        ret = true;
        arrowPosition_->startRelativeRatio = newStartRelativeRatioPoint;
    }

    return ret;
}

bool ArrowController::updateEndArrowPosition(const QPointF &newEndArrowPosition)
{
    bool ret = false;

    if (nullptr == arrowPosition_)
    {
        return ret;
    }
    if (arrowPosition_->currentEndPos != newEndArrowPosition)
    {
        ret = true;
        arrowPosition_->currentEndPos = newEndArrowPosition;
    }

    const QRectF &destRect = destination_->boundingRect();
    const qreal newEndRelativeRatioX = (arrowPosition_->currentEndPos.x() - destRect.x()) / destRect.width();
    const qreal newEndRelativeRatioY = (arrowPosition_->currentEndPos.y() - destRect.y()) / destRect.height();
    const QPointF &newEndRelativeRatioPoint {newEndRelativeRatioX, newEndRelativeRatioY};

    if (arrowPosition_->endRelativeRatio != newEndRelativeRatioPoint)
    {
        ret = true;
        arrowPosition_->endRelativeRatio = newEndRelativeRatioPoint;
    }

    return ret;
}

void ArrowController::updateLastArrowPositionSlot()
{
    if (nullptr == arrowPosition_ || nullptr == arrow_)
    {
        return;
    }

    const QGraphicsScene* const arrowScene = arrow_->scene();

    if (nullptr != source_ && arrowScene == source_->scene())
    {
        arrowPosition_->lastStartPos = source_->mapFromScene(arrow_->startPoint());
    }
    if (nullptr != destination_ && arrowScene == destination_->scene())
    {
        arrowPosition_->lastEndPos = destination_->mapFromScene(arrow_->endPoint());
    }
}

void ArrowController::srcResizedOption(const QRectF &curSrcRect, const QRectF &curSrcSceneRect)
{
    const QSizeF &prevSrcRectSize = previousSourceRect_.size();
    const QSizeF &curSrcRectSize = curSrcSceneRect.size();
    const qreal newStartPosX = {curSrcRect.x() + curSrcRect.width() * arrowPosition_->startRelativeRatio.x()};
    const qreal newStartPosY = {curSrcRect.y() + curSrcRect.height() * arrowPosition_->startRelativeRatio.y()};
    arrowPosition_->currentStartPos = {newStartPosX, newStartPosY};
    const qreal lastStartPosX = {arrowPosition_->lastStartPos.x()};
    const qreal lastStartPosY = {arrowPosition_->lastStartPos.y()};
    const qreal prevSrcRectSizeHeight = {prevSrcRectSize.height()};
    const qreal curSrcRectSizeHeight = {curSrcRectSize.height()};

    if (prevSrcRectSizeHeight != curSrcRectSizeHeight)
    {
        arrowPosition_->lastStartPos.setY(lastStartPosY * (curSrcRectSizeHeight / prevSrcRectSizeHeight));
    }

    const qreal prevSrcRectSizeWidth = {prevSrcRectSize.width()};
    const qreal curSrcRectSizeWidth = {curSrcRectSize.width()};

    if (prevSrcRectSizeWidth != curSrcRectSizeWidth)
    {
        arrowPosition_->lastStartPos.setX(lastStartPosX * (curSrcRectSizeWidth / prevSrcRectSizeWidth));
    }
}

void ArrowController::destResizedOption(const QRectF &curDestRect, const QRectF &curDestSceneRect)
{
    const QSizeF &prevDestRectSize = previousDestRect_.size();
    const QSizeF &curDestRectSize = curDestSceneRect.size();
    const qreal newEndPosX = {curDestRect.x() + curDestRect.width() * arrowPosition_->endRelativeRatio.x()};
    const qreal newEndPosY = {curDestRect.y() + curDestRect.height() * arrowPosition_->endRelativeRatio.y()};
    arrowPosition_->currentEndPos = {newEndPosX, newEndPosY};
    const qreal lastEndPosX = {arrowPosition_->lastEndPos.x()};
    const qreal lastEndPosY = {arrowPosition_->lastEndPos.y()};
    const qreal prevDestRectSizeHeight = {prevDestRectSize.height()};
    const qreal curDestRectSizeHeight = {curDestRectSize.height()};

    if (prevDestRectSizeHeight != curDestRectSizeHeight)
    {
        arrowPosition_->lastEndPos.setY(lastEndPosY * (curDestRectSizeHeight / prevDestRectSizeHeight));
    }

    const qreal prevDestRectSizeWidth = {prevDestRectSize.width()};
    const qreal curDestRectSizeWidth = {curDestRectSize.width()};
    if (prevDestRectSizeWidth != curDestRectSizeWidth)
    {
        arrowPosition_->lastEndPos.setX(lastEndPosX * (curDestRectSizeWidth / prevDestRectSizeWidth));
    }
}

UpdateGeometryMode ArrowController::determineMode() const
{
    UpdateGeometryMode mode = UpdateGeometryMode::UpdatedNoItems;
    bool isStartPointMovable = false;
    bool isEndPointMovable = false;

    if (nullptr != arrow_)
    {
        isStartPointMovable = arrow_->isStartPointMovable();
        isEndPointMovable = arrow_->isEndPointMovable();
    }

    if (isStartPointMovable)
    {
        mode = UpdateGeometryMode::UpdatedStartItem;
    }
    if (isEndPointMovable)
    {
        mode = {
        mode == UpdateGeometryMode::UpdatedNoItems ? UpdateGeometryMode::UpdatedEndItem : UpdateGeometryMode::UpdatedBothItems
        };
    }

    return mode;
}

void ArrowController::flowViewScaleChangedSlot()
{
    const auto* signalSenderView = dynamic_cast<FlowView*>(sender());
    if (nullptr == signalSenderView)
    {
        const NodesScene* const sceneModel = Facade::Instance().sceneModel();
        signalSenderView = sceneModel->getTopView();
    }

    if (nullptr == arrow_)
    {
        return;
    }

    if (nullptr == signalSenderView)
    {
        return;
    }

    const int currentViewScale = {signalSenderView->getCurrentScale()};
    qreal newPointsRadius = {6.0};

    if (currentViewScale < 100)
    {
        const int scaleFactor = {currentViewScale > 30 ? 100 : 80};
        newPointsRadius = {newPointsRadius * scaleFactor / currentViewScale};
    }

    qreal newHoverWidthRatio = {1.6};

    if (currentViewScale <= 30)
    {
        newHoverWidthRatio = 8;
    }

    arrow_->setHoverWidthRatio(newHoverWidthRatio);
    arrow_->setSmartPointsRadius(newPointsRadius);
}

QJsonObject ArrowController::save() const
{
    QJsonObject retval {};
    QJsonObject obj {};
    if (nullptr != source_)
    {
        retval["source"] = source_->save();
    }

    if (nullptr != destination_)
    {
        retval["dest"] = destination_->save();
    }

    if (nullptr != arrow_)
    {
        retval["arrow"] = arrow_->save();
    }

    if (nullptr != arrowPosition_)
    {
        retval["arrowPosition"] = arrowPosition_->save();
    }

    return retval;
}

void ArrowController::restore(const QJsonObject &restoringObject)
{
    if (nullptr != arrowPosition_)
    {
        arrowPosition_->restore(restoringObject["arrowPosition"].toObject());
    }
    if (nullptr != arrow_)
    {
        arrow_->restore(restoringObject["arrow"].toObject());
        flowViewScaleChangedSlot();
        arrow_->updateGeometry(source_, destination_, arrowPosition_);
    }
}
