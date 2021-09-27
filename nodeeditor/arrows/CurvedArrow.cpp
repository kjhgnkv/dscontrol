#include "CurvedArrow.h"

#include "ArrowPosition.h"
#include "EquilateralTriangle.h"
#include "PolyLineArrow.h"
#include "TinyMath.h"
#include "scenes/FlowScene.hpp"
#include "EdgePoint.h"

#include <QGraphicsSceneMouseEvent>
#include <QJsonArray>
#include <QPainter>

inline uint qHash(const QPointF &key)
{
    return qHash(QPair<qreal, qreal>(key.x(), key.y()));
}

using namespace QtNodes::Arrows;
using namespace QtNodes::GeometricShapes;

CurvedArrow::CurvedArrow(const QPointF &startPoint, const QPointF &endPoint, const QUuid &uid,
                         QGraphicsItem* parentItem, QObject* parentObject)
: AbstractArrow {startPoint, endPoint, uid, parentItem, parentObject}
, controlPointsCount_ {2}
, controlPoints_(controlPointsCount_)
{
    setArrowType(Arrows::ArrowType::Curved);
    createControlPoints();
    inizializeControlPoints();
    setFlag(ItemSendsGeometryChanges, true);
    setMovable(true);
}

QJsonArray CurvedArrow::saveTransitPoints() const
{
    QJsonArray savingControlPoints;
    for (const auto &controlPoint : controlPoints_)
    {
        if (nullptr != controlPoint)
        {
            savingControlPoints.push_back(controlPoint->save());
        }
    }
    return savingControlPoints;
}

void CurvedArrow::restoreTransitPoints(const QJsonArray &restoringTransitPoints)
{
    if (restoringTransitPoints.size() != controlPointsCount_)
    {
        return;
    }

    AnnulusSPtr currentControlPoint {nullptr};
    for (quint8 pointIndex {0}; pointIndex < controlPointsCount_; ++pointIndex)
    {
        currentControlPoint = controlPoints_.at(pointIndex);
        if (nullptr != currentControlPoint)
        {
            const QJsonObject &controlPointObj = restoringTransitPoints.at(pointIndex).toObject();
            currentControlPoint->restore(controlPointObj);
            currentControlPoint = nullptr;
        }
    }
    synchronize();
}

QPainterPath CurvedArrow::createTinyShape(qreal shapeWidth) const
{
    QPainterPath tinyShape = shape();
    TinyMath::setPathWidth(tinyShape, shapeWidth);
    return tinyShape;
}

QPointF CurvedArrow::findStartIntersectionPoint(const QPointF &newStartPoint, const QPainterPath &startShape) const
{
    const auto &startIntersectionPoints = findIntersectionPoints(startShape);
    return TinyMath::minDistancePoint(newStartPoint, startIntersectionPoints);
}

QPointF CurvedArrow::findEndIntersectionPoint(const QPointF &newEndPoint, const QPainterPath &endShape) const
{
    const auto &endIntersectionPoints = findIntersectionPoints(endShape);
    return TinyMath::minDistancePoint(newEndPoint, endIntersectionPoints);
}

AbstractArrow* CurvedArrow::clone() const
{
    auto ret = new CurvedArrow(*this);
    ret->synchronize();
    return ret;
}

void CurvedArrow::moveControlPoints(const QPointF &offset)
{
    if (offset.isNull())
    {
        return;
    }
    bool hasBeenMoved = false;
    needToBeUpdated_ = false;
    for (auto controlPoint : controlPoints_)
    {
        if (nullptr != controlPoint && controlPoint->flags().testFlag(ItemIsMovable))
        {
            hasBeenMoved = true;
            controlPoint->moveBy(offset.x(), offset.y());
        }
    }
    needToBeUpdated_ = true;

    if (hasBeenMoved)
    {
        synchronize();
    }
}

void CurvedArrow::resetLine()
{
    setControlPointsPosToDefault();
}

QLineF CurvedArrow::directDirection() const
{
    const auto &lastSmartPoint = controlPoints_.back();
    if (nullptr == lastSmartPoint)
    {
        return {};
    }

    return QLineF {lastSmartPoint->pos(), mapFromScene(endPoint())};
}

QLineF CurvedArrow::reverseDirection() const
{
    const auto &firstSmartPoint = controlPoints_.front();
    if (nullptr == firstSmartPoint)
    {
        return {};
    }
    return QLineF {firstSmartPoint->pos(), mapFromScene(startPoint())};
}

QPainterPath CurvedArrow::paintingLine() const
{
    const auto &firstSmartPoint = controlPoints_.front();
    if (nullptr == firstSmartPoint)
    {
        return {};
    }

    const auto &secondSmartPoint = controlPoints_.back();
    if (nullptr == secondSmartPoint)
    {
        return {};
    }

    const auto &paintingLineEdges = createPaintingLineEdges();

    QPainterPath ret;
    ret.moveTo(paintingLineEdges.first.p2());
    ret.cubicTo(firstSmartPoint->pos(), secondSmartPoint->pos(), paintingLineEdges.second.p2());
    ret.setFillRule(Qt::FillRule::WindingFill);
    TinyMath::setPathWidth(ret, width());

    return ret;
}

std::pair<QPointF, QPointF> CurvedArrow::findIntersectionPoints(const QPointF &newStartPoint,
                                                                const QPointF &newEndPoint,
                                                                const QPainterPath &startShape,
                                                                const QPainterPath &endShape) const
{
    const QPointF &startIntersectionPoint = findStartIntersectionPoint(newStartPoint, startShape);
    const QPointF &endIntersectionPoint = findEndIntersectionPoint(newEndPoint, endShape);

    return {startIntersectionPoint, endIntersectionPoint};
}

void CurvedArrow::createControlPoints()
{
    if (controlPoints_.size() != controlPointsCount_)
    {
        controlPoints_.resize(controlPointsCount_);
    }

    for (auto &controlPoint : controlPoints_)
    {
        if (nullptr == controlPoint)
        {
            controlPoint = std::make_shared<Annulus>();
        }
    }
}

void CurvedArrow::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (nullptr != event && isPressed())
    {
        setSelected(true);
        const QPointF &offset = event->pos() - event->lastPos();
        moveControlPoints(offset);
    }
}

AnnulusSPtrVector CurvedArrow::transitPoints() const
{
    return controlPoints_;
}

void CurvedArrow::inizializeControlPoints()
{
    for (const auto &controlPoint : controlPoints_)
    {
        controlPoint->setMovable(false);
        controlPoint->setFlag(ItemIsMovable, true);
        controlPoint->setUndoRedoPositionEnable(false);
        controlPoint->setParentItem(this);
        controlPoint->setEnabled(false);
    }
    setSmartPointsRadius(smartPointsRadius());
    setControlPointsPosToDefault();
}

void CurvedArrow::setControlPointsPosToDefault()
{
    const QPointF &startPointParentPos = mapToParent(mapFromScene(startPoint()));
    const QPointF &endPointParentPos = mapToParent(mapFromScene(endPoint()));
    const QPointF &centerPoint = QLineF {startPointParentPos, endPointParentPos}.center();

    setFirstControlPointPosToDefault(startPointParentPos, centerPoint);
    setSecondControlPointPosToDefault(endPointParentPos, centerPoint);
    synchronize();
}

void CurvedArrow::setFirstControlPointPosToDefault(const QPointF &startPointParentPos, const QPointF &centerPoint)
{
    const auto &firstControlPoint = controlPoints_.front();
    if (nullptr != firstControlPoint)
    {
        QLineF firstHalfLine {startPointParentPos, centerPoint};
        const QPointF &firstHalfLineCenter = firstHalfLine.center();
        firstHalfLine.setP1(firstHalfLineCenter);
        firstHalfLine.setAngle(firstHalfLine.angle() - TinyMath::rightAngleInDegrees());
        firstControlPoint->setPos(firstHalfLine.p2());
    }
}

void CurvedArrow::setSecondControlPointPosToDefault(const QPointF &endPointParentPos, const QPointF &centerPoint)
{
    const auto &secondControlPoint = controlPoints_.back();
    if (nullptr != secondControlPoint)
    {
        QLineF secondHalfLine {centerPoint, endPointParentPos};
        const QPointF &secondHalfLineCenter = secondHalfLine.center();
        secondHalfLine.setP1(secondHalfLineCenter);
        secondHalfLine.setAngle(secondHalfLine.angle() + TinyMath::rightAngleInDegrees());
        secondControlPoint->setPos(secondHalfLine.p2());
    }
}

QVector<QPointF> CurvedArrow::findIntersectionPoints(const QPainterPath &path) const
{
    const qreal tinyShapeWidth {5e-2};
    const QPainterPath &tinyShape = createTinyShape(tinyShapeWidth);
    const QPolygonF &intersectedPolygon = tinyShape.intersected(path).toFillPolygon();

    QRectF pathRect = path.boundingRect();
    const qreal searchingError {1e-3};
    TinyMath::increaseRect(pathRect, -searchingError);

    QList<QPointF> foundedIntersectionPoints;
    for (auto point : intersectedPolygon)
    {
        if (!pathRect.contains(point))
        {
            foundedIntersectionPoints << point;
        }
    }
    //TODO: test. refactoring because of: warning C4996: 'QList<T>::toSet': Use QSet<T>(list.begin(), list.end()) instead.
    auto set = QSet<QPointF>(foundedIntersectionPoints.begin(), foundedIntersectionPoints.end());
    return set.values().toVector();
    //return foundedIntersectionPoints.toSet().values().toVector();
}
