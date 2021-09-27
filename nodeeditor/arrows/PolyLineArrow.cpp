#include "PolyLineArrow.h"
#include "EdgePoint.h"
#include "ArrowPosition.h"
#include "Circle.h"
#include "EquilateralTriangle.h"
#include "TinyMath.h"
#include "scenes/FlowScene.hpp"

#include <QGraphicsSceneMouseEvent>
#include <QJsonArray>

using namespace QtNodes::Arrows;
using namespace QtNodes::GeometricShapes;

PolyLineArrow::PolyLineArrow(const PolyLineArrow &another)
: AbstractArrow {another}
, mode_ {Mode::Default}
, maxTransitPointsCount_ {another.maxTransitPointsCount_}
, selectedLine_ {another.selectedLine_}
, line_ {another.line_}
{
    smartTransitPoints_.clear();
    for (const auto &smartTransitPoint : another.smartTransitPoints_)
    {
        if (nullptr != smartTransitPoint)
        {
            auto smartTransitPointCopy = std::make_shared<GeometricShapes::Annulus>(*smartTransitPoint);
            pushTransitPoint(smartTransitPointCopy);
        }
    }
    setMovable(true);
}

PolyLineArrow::PolyLineArrow(const QPointF &startPoint, const QPointF &endPoint, const QUuid &uuid,
                             QGraphicsItem* parentItem, QObject* parentObject)
: AbstractArrow {startPoint, endPoint, uuid, parentItem, parentObject}
, mode_ {Mode::Default}
, maxTransitPointsCount_ {30}
, selectedLine_({nullptr, nullptr})
, line_(TinyMath::minLinePointsCount())
{
    setArrowType(ArrowType::PolyLine);
    initializeLine();
    setMovable(true);
}

AbstractArrow* PolyLineArrow::clone() const
{
    AbstractArrow* ret = new PolyLineArrow(*this);
    ret->synchronize();
    return ret;
}

QPolygonF PolyLineArrow::line() const
{
    return line_;
}

bool PolyLineArrow::setLine(const QPolygonF &newLine)
{
    if (line_ == newLine)
    {
        return false;
    }
    const quint8 newTransitPointsCount = {static_cast<quint8>(newLine.count() - TinyMath::minLinePointsCount())};
    if (newTransitPointsCount >= maxTransitPointsCount_)
    {
        return false;
    }

    const qint8 countDiffrence = {static_cast<qint8>(newTransitPointsCount - transitPointsCount())};
    resizeTransitPoints(countDiffrence);

    setStartPoint(newLine.first());
    setEndPoint(newLine.last());
    AnnulusSPtr transitPoint {nullptr};
    for (quint8 newLinePointIndex {1}; newLinePointIndex <= newTransitPointsCount; ++newLinePointIndex)
    {
        transitPoint = smartTransitPoints_[newLinePointIndex];
        if (nullptr != transitPoint)
        {
            transitPoint->setPos(newLine[newLinePointIndex]);
        }
    }
    synchronize();
    return true;
}

bool PolyLineArrow::addPoint(const QPointF &newPoint, quint8 index)
{
    if (transitPointsCount() >= maxTransitPointsCount_ || index == 0 || index >= smartPointsCount() || !isEverySmartPointEnabled())
    {
        return false;
    }

    auto newSmartPoint = std::make_shared<GeometricShapes::Annulus>(mapFromScene(newPoint));
    setSmartPointRadius(newSmartPoint, smartPointsRadius());
    if (smartPointCollidesWithTransitPoints(newSmartPoint))
    {
        newSmartPoint->deleteLater();
        return false;
    }
    addTransitPointToTheFlowScene(newSmartPoint);
    connectTransitPoint(newSmartPoint);
    prepareGeometryChange();
    auto insertPositionIt = smartTransitPoints_.begin();
    std::advance(insertPositionIt, --index);
    smartTransitPoints_.insert(insertPositionIt, newSmartPoint);
    newSmartPoint->show();
    synchronize();
    return true;
}

void PolyLineArrow::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (!isEachParamFineForMousePressEvent(event))
    {
        return;
    }

    const QPointF &eventPos = event->pos();
    if (event->modifiers().testFlag(Qt::ShiftModifier))
    {
        pointAddintOption(eventPos);
    }
    else
    {
        lineMovingOption(eventPos);
    }
    return AbstractArrow::mousePressEvent(event);
}

void PolyLineArrow::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (nullptr == event || !isMovable() || Mode::LineMoving != mode_ || nullptr == selectedLine_.first || nullptr == selectedLine_.second)
    {
        return;
    }
    setSelected(true);
    if (smartTransitPoints_.empty())
    {
        return AbstractArrow::mouseMoveEvent(event);
    }

    const QPointF &mouseOffset = event->scenePos() - event->lastScenePos();
    const QPointF &curSceneStartPoint = startPoint();
    const QPointF &curArrowStartPoint = mapFromScene(curSceneStartPoint);
    const bool needToMoveStartPoint = selectedLine_.first->pos() == curArrowStartPoint && isStartPointMovable();
    if (needToMoveStartPoint)
    {
        emit startPointPositionChange(curSceneStartPoint + mouseOffset, mouseOffset);
        selectedLine_.second->moveBy(mouseOffset.x(), mouseOffset.y(), false);
        emit arrowNeedToUpdateGeometry();
        return;
    }

    const QPointF &curSceneEndPoint = endPoint();
    const QPointF &curArrowEndPoint = mapFromScene(curSceneEndPoint);
    const bool needToMoveEndPoint = selectedLine_.second->pos() == curArrowEndPoint && isEndPointMovable();
    if (needToMoveEndPoint)
    {
        selectedLine_.first->moveBy(mouseOffset.x(), mouseOffset.y(), false);
        emit endPointPositionChange(curSceneEndPoint + mouseOffset, mouseOffset);
        emit arrowNeedToUpdateGeometry();
        return;
    }

    selectedLine_.first->moveBy(mouseOffset.x(), mouseOffset.y(), false);
    selectedLine_.second->moveBy(mouseOffset.x(), mouseOffset.y(), false);
}

void PolyLineArrow::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (Mode::LineMoving == mode_)
    {
        synchronize();
    }
    mode_ = Mode::Default;
    selectedLine_ = {nullptr, nullptr};
    AbstractArrow::mouseReleaseEvent(event);
}

std::pair<QPointF, QPointF> PolyLineArrow::findIntersectionPoints(const QPointF &newStartPoint,
                                                                  const QPointF &newEndPoint,
                                                                  const QPainterPath &startShape,
                                                                  const QPainterPath &endShape) const
{
    const quint8 currentLinePointsCount = {static_cast<quint8>(line_.count())};
    if (currentLinePointsCount < TinyMath::minLinePointsCount())
    {
        return {};
    }

    const QPointF &newStartIntersectionPoint = findStartIntersectionPoint(newStartPoint, newEndPoint, startShape,
                                                                          currentLinePointsCount);
    const QPointF &newEndIntersectionPoint = findEndIntersectionPoint(newStartPoint, newEndPoint, endShape,
                                                                      currentLinePointsCount);

    return {newStartIntersectionPoint, newEndIntersectionPoint};
}

void PolyLineArrow::pushTransitPoint(AnnulusSPtr newTransitPoint)
{
    if (nullptr == newTransitPoint || transitPointsCount() >= maxTransitPointsCount_)
    {
        return;
    }
    smartTransitPoints_.push_back(newTransitPoint);
    addTransitPointToTheFlowScene(newTransitPoint);
    setSmartPointRadius(newTransitPoint, smartPointsRadius());
    synchronizeLine();
    connectTransitPoint(newTransitPoint);
}

void PolyLineArrow::resizeTransitPoints(quint8 pointsCount)
{
    const qint8 newCount = {static_cast<qint8>(transitPointsCount() + pointsCount)};
    if (newCount < 0 || newCount >= maxTransitPointsCount_)
    {
        return;
    }
    if (pointsCount < 0)
    {
        smartTransitPoints_.resize(newCount);
        return;
    }
    for (quint8 pointCounter {0}; pointCounter < pointsCount; ++pointCounter)
    {
        pushTransitPoint(std::make_shared<Annulus>());
    }
}

void PolyLineArrow::addTransitPointToTheFlowScene(AnnulusSPtr transitPoint)
{
    if (nullptr == transitPoint)
    {
        return;
    }
    transitPoint->setParentItem(this);

    const auto scene = flowScene();
    if (nullptr != scene)
    {
        scene->addGraphicsItem(transitPoint.get(), transitPoint->pos());
    }
}

void PolyLineArrow::connectTransitPoint(AnnulusSPtr transitPoint) const
{
    if (nullptr != transitPoint)
    {
        connect(transitPoint.get(), &GeometricShapes::Annulus::positionHasChanged, this, &PolyLineArrow::synchronize);
        connect(transitPoint.get(), &GeometricShapes::Annulus::positionHasChanged, this,
                &PolyLineArrow::smartPointPositionHasChangedSlot);
    }
}

qint16 PolyLineArrow::findPointIndexByPos(const QPointF &pos) const
{
    const auto &allSmartPoints = smartPoints();
    const auto &foundedPoint = std::find_if(allSmartPoints.begin(), allSmartPoints.end(), [pos](const auto &targetPoint)
    {
        return nullptr != targetPoint ? (targetPoint->pos() == pos) : false;
    });

    const qint16 errorIndex {-1};
    return foundedPoint == allSmartPoints.end() ? errorIndex : std::distance(allSmartPoints.begin(), foundedPoint);
}

QPolygonF PolyLineArrow::createPaintingLinePolygon(const std::pair<QLineF, QLineF> &paintingLineEdges) const
{
    QPolygonF ret;

    ret << paintingLineEdges.first.p2();
    for (auto paintingTransitPoint : smartTransitPoints_)
    {
        if (nullptr != paintingTransitPoint && paintingTransitPoint->isEnabled())
        {
            ret << paintingTransitPoint->pos();
        }
    }
    ret << paintingLineEdges.second.p2();

    return ret;
}

bool PolyLineArrow::smartPointCollidesWithTransitPoints(const AnnulusSPtr &targetSmartPoint) const
{
    for (const auto &smartPoint : smartTransitPoints_)
    {
        if (nullptr != smartPoint && smartPoint->collidesWithItem(targetSmartPoint.get()))
        {
            return true;
        }
    }
    return false;
}

void PolyLineArrow::resetLine()
{
    smartTransitPoints_.clear();
    synchronize();
}

QPointF PolyLineArrow::findStartIntersectionPoint(const QPointF &newStartPoint, const QPointF &newEndPoint,
                                                  const QPainterPath &startShape, quint8 currentLinePointsCount) const
{
    const quint8 secondPointIndex = 1;
    const QPointF &secondPoint = currentLinePointsCount == TinyMath::minLinePointsCount() ? newEndPoint : line_[secondPointIndex];
    const QLineF &newStartLine {newStartPoint, secondPoint};
    const auto &startIntersectionPoints = TinyMath::intersectionPoints(newStartLine, startShape);
    return TinyMath::minDistancePoint(secondPoint, startIntersectionPoints);
}

QPointF PolyLineArrow::findEndIntersectionPoint(const QPointF &newStartPoint, const QPointF &newEndPoint,
                                                const QPainterPath &endShape, quint8 currentLinePointsCount) const
{
    const quint8 penultimatePointIndex = currentLinePointsCount - TinyMath::minLinePointsCount();
    const QPointF &penultimatePoint = currentLinePointsCount == TinyMath::minLinePointsCount() ? newStartPoint : line_[penultimatePointIndex];
    const QLineF &newEndLine {newEndPoint, penultimatePoint};
    const auto &endIntersectionPoints = TinyMath::intersectionPoints(newEndLine, endShape);
    return TinyMath::minDistancePoint(penultimatePoint, endIntersectionPoints);
}

bool PolyLineArrow::isEachParamFineForMousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (nullptr == event)
    {
        return false;
    }
    if (Qt::LeftButton != event->button() || oneOfEdgePointsContains(event->scenePos()))
    {
        AbstractArrow::mousePressEvent(event);
        return false;
    }
    return true;
}

void PolyLineArrow::pointAddintOption(const QPointF &clickPos)
{
    mode_ = Mode::PointAdding;
    const auto &minLineProjectionsPoint = minProjectionsPoint(clickPos);
    if (nullptr != minLineProjectionsPoint.first)
    {
        addPoint(*minLineProjectionsPoint.first, minLineProjectionsPoint.second + 1);
        delete minLineProjectionsPoint.first;
    }
}

void PolyLineArrow::lineMovingOption(const QPointF &clickPos)
{
    if (!isMovable())
    {
        return;
    }
    mode_ = Mode::LineMoving;
    if (smartTransitPoints_.empty())
    {
        selectedLine_.first = smartStartPoint();
        selectedLine_.second = smartEndPoint();
        return;
    }

    auto minLineProjectionsPoint = minProjectionsPoint(clickPos);
    if (nullptr == minLineProjectionsPoint.first)
    {
        return;
    }
    delete minLineProjectionsPoint.first;
    minLineProjectionsPoint.first = nullptr;

    const quint8 index = {minLineProjectionsPoint.second};
    if (index + 1 < smartPointsCount())
    {
        const auto &allSmartPoints = smartPoints();
        selectedLine_.first = allSmartPoints[index];
        selectedLine_.second = allSmartPoints[index + 1];
    }
}

QLineF PolyLineArrow::directDirection() const
{
    const QVector<QPointF> &enabledTransitPoints = this->enabledTransitPoints();
    const QPointF &firstPoint = enabledTransitPoints.isEmpty() ? mapFromScene(
    startPoint()) : enabledTransitPoints.last();
    const QPointF &secondPoint = mapFromScene(endPoint());
    return QLineF {firstPoint, secondPoint};
}

QLineF PolyLineArrow::reverseDirection() const
{
    const QVector<QPointF> &enabledTransitPoints = this->enabledTransitPoints();
    const QPointF &firstPoint = enabledTransitPoints.isEmpty() ? mapFromScene(
    endPoint()) : enabledTransitPoints.first();
    const QPointF &secondPoint = mapFromScene(startPoint());
    return QLineF {firstPoint, secondPoint};
}

QVector<QPointF> PolyLineArrow::enabledTransitPoints() const
{
    QVector<QPointF> ret = line_;
    ret.pop_front();
    ret.pop_back();
    return ret;
}

void PolyLineArrow::initializeLine()
{
    line_.first() = smartStartPoint()->pos();
    line_.last() = smartEndPoint()->pos();
}

void PolyLineArrow::synchronizeLine()
{
    const auto &curSmartPoints = smartPoints();
    const quint8 curSmartPointsCount {smartPointsCount()};
    AnnulusSPtr smartPoint {nullptr};

    prepareGeometryChange();
    line_.resize(curSmartPointsCount);
    for (quint8 smartPointIndex {0}; smartPointIndex < curSmartPointsCount; ++smartPointIndex)
    {
        smartPoint = curSmartPoints[smartPointIndex];
        if (nullptr != smartPoint && smartPoint->isEnabled())
        {
            line_[smartPointIndex] = smartPoint->pos();
        }
        smartPoint = nullptr;
    }
}

std::pair<QPointF*, quint8> PolyLineArrow::minProjectionsPoint(const QPointF &targetPoint) const
{
    //TODO: test. refactoring because of:warning C4996: 'QVector<QPointF > ::toStdVector': Use std::vector<T>(vector.begin(), vector.end()) instead.
    const auto &lineAsVector = std::vector<QPointF> (line_.begin(), line_.end());
    //const std::vector<QPointF> &lineAsVector = line_.toStdVector();
    QPointF currentLinePoint = lineAsVector.front();
    QPointF* minLineProjectionPoint = new QPointF;
    qreal minLength = {TinyMath::distance(targetPoint, *minLineProjectionPoint)};
    qreal currentLength = minLength;

    for (auto point = lineAsVector.begin(); point != lineAsVector.end() - 1; ++point)
    {
        QPointF newLineProjectionPoint;
        const QLineF &currentLine {*point, *(point + 1)};
        const bool isOnLine = TinyMath::lineProjection(currentLine, targetPoint, &newLineProjectionPoint);
        currentLength = {TinyMath::distance(targetPoint, newLineProjectionPoint)};
        if (isOnLine && (minLineProjectionPoint->isNull() || currentLength < minLength))
        {
            currentLinePoint = *point;
            *minLineProjectionPoint = newLineProjectionPoint;
            minLength = {TinyMath::distance(targetPoint, *minLineProjectionPoint)};
        }
    }

    const quint8 index = {static_cast<quint8>(line_.indexOf(currentLinePoint))};
    return {minLineProjectionPoint, index};
}

void PolyLineArrow::smartPointPositionHasChangedSlot()
{
    const auto &senderSmartPoint = dynamic_cast<GeometricShapes::Annulus*>(sender());
    if (nullptr == senderSmartPoint)
    {
        return;
    }
    if ((senderSmartPoint != smartTransitPoints_.front().get() || !isStartPointMovable()) && (senderSmartPoint != smartTransitPoints_.back().get() || !isEndPointMovable()))
    {
        synchronize();
        return;
    }
    emit arrowNeedToUpdateGeometry();
}

void PolyLineArrow::restoreTransitPoints(const QJsonArray &restoringTransitPoints)
{
    smartTransitPoints_.clear();
    for (auto point : restoringTransitPoints)
    {
        auto smartTransitPoint = std::make_shared<Annulus>();
        smartTransitPoint->setParentItem(this);
        smartTransitPoint->restore(point.toObject());
        pushTransitPoint(smartTransitPoint);
        emit arrowNeedToUpdateGeometry(UpdateGeometryMode::UpdatedBothItems);
    }
    synchronize();
}

AnnulusSPtrVector PolyLineArrow::transitPoints() const
{
    return smartTransitPoints_;
}

QPainterPath PolyLineArrow::paintingLine() const
{
    const auto &paintingLineEdges = createPaintingLineEdges();
    const QPolygonF &paintingLinePolygon = createPaintingLinePolygon(paintingLineEdges);

    QPainterPath ret;
    ret.addPolygon(paintingLinePolygon);
    TinyMath::setPathWidth(ret, width());

    return ret;
}

QJsonObject PolyLineArrow::save() const
{
    QJsonObject retval = AbstractArrow::save();
    retval["maxTransitPointsCount"] = maxTransitPointsCount_;
    return retval;
}

QJsonArray PolyLineArrow::saveTransitPoints() const
{
    QJsonArray transitPoints;
    for (const auto &transitPoint : smartTransitPoints_)
    {
        transitPoints.push_back(transitPoint->save());
    }
    return transitPoints;
}

void PolyLineArrow::restore(const QJsonObject &restoringObject)
{
    setMaxTransitPointsCount(restoringObject["maxTransitPointsCount"].toInt());
    AbstractArrow::restore(restoringObject);
}

void PolyLineArrow::synchronize()
{
    if (needToBeUpdated_)
    {
        synchronizeLine();
        AbstractArrow::synchronize();
    }
}

void PolyLineArrow::setMaxTransitPointsCount(quint8 newMaxTransitPointsCount)
{
    if (newMaxTransitPointsCount < transitPointsCount())
    {
        resizeTransitPoints(newMaxTransitPointsCount);
    }
    maxTransitPointsCount_ = newMaxTransitPointsCount;
}

quint8 PolyLineArrow::maxTransitPointsCount() const
{
    return maxTransitPointsCount_;
}

quint8 PolyLineArrow::maxPointsCount() const
{
    return maxTransitPointsCount() + TinyMath::minLinePointsCount();
}
