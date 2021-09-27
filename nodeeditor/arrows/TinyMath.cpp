#include "TinyMath.h"

#include <QDebug>
#include <QtMath>

using namespace QtNodes;

const quint8 TinyMath::triangleVerteciesCount_ {3};
const quint8 TinyMath::minLinePointsCount_ {2};
const qreal TinyMath::minRadiusEnabled_ {0};
const qreal TinyMath::rightAngleInDegrees_ {90};
const qreal TinyMath::minErrorLimit_ {0};
const qreal TinyMath::maxErrorLimit_ {1};

QVector<QPointF> TinyMath::intersectionPoints(QLineF line, QRectF rect)
{
    const qreal rectEpsilon {1e-3};
    increaseRect(rect, -rectEpsilon);
    const auto &rectSides = getRectSides(rect);
    const qreal lineEpsilon {1e7};
    increaseLine(line, lineEpsilon);

    QVector<QPointF> ret;
    QPointF intersectionPoint;
    for (const auto &side : rectSides)
    {
        if (line.intersects(side, &intersectionPoint) == QLineF::IntersectType::BoundedIntersection)
        {
            ret << intersectionPoint;
        }
    }
    return ret;
}

QVector<QPointF> TinyMath::intersectionPoints(const QLineF &line, const QPainterPath &shape)
{
    /** Creating a tiny linear PainterPath from a line **/
    const qreal lineWidth {1e-6};
    const QVector<QPointF> &polyLine {line.p1(), line.p2()};
    QPainterPath linePath;
    linePath.addPolygon(polyLine);
    setPathWidth(linePath, lineWidth);

    /** Searching the intersection path between the line and the shape **/
    const QPainterPath &intersectionPath = shape.intersected(linePath);

    return intersectionPath.isEmpty() ? intersectionPoints(line,
                                                           shape.boundingRect()) : intersectionPath.toFillPolygon();
}

QVector<QLineF> TinyMath::getRectSides(const QRectF &rect)
{
    const QPointF &topLeft = rect.topLeft();
    const QPointF &topRight = rect.topRight();
    const QPointF &bottomRight = rect.bottomRight();
    const QPointF &bottomLeft = rect.bottomLeft();

    return QVector<QLineF> {
    QLineF {topLeft, topRight}, QLineF {topRight, bottomRight}, QLineF {bottomRight, bottomLeft}, QLineF {
    bottomLeft, topLeft
    }
    };
}

bool TinyMath::rectContainsPoint(QRectF rect, const QPointF &point, qreal error)
{
    if (isErrorCurrect(error))
    {
        increaseRect(rect, error);
        return rect.contains(point);
    }
    return false;
}

bool TinyMath::isErrorCurrect(qreal error)
{
    return (minErrorLimit_ < error && error < maxErrorLimit_);
}

RectEdge TinyMath::determineRectEdge(const QRect &rect, const QPoint &pos)
{
    const QPoint &rectTopLeft = rect.topLeft();
    if (pos == rectTopLeft)
    {
        return RectEdge::TopLeft;
    }

    const qint32 rectLeft = {rect.left()};
    const qint32 rectRight = {rectLeft + rect.width()};
    const qint32 rectTop = {rect.top()};

    const QPoint &rectTopRight {rectRight, rectTop};
    if (pos == rectTopRight)
    {
        return RectEdge::TopRight;
    }

    const qint32 rectBottom = {rectTop + rect.height()};

    const QPoint &rectBottomLeft {rectLeft, rectBottom};
    if (pos == rectBottomLeft)
    {
        return RectEdge::BottomLeft;
    }

    const QPoint &rectBottomRight {rectRight, rectBottom};
    if (pos == rectBottomRight)
    {
        return RectEdge::BottomRight;
    }

    const qint32 x = {pos.x()};

    if (x == rectLeft)
    {
        return RectEdge::Left;
    }

    if (x == rectRight)
    {
        return RectEdge::Right;
    }

    const qint32 y = {pos.y()};

    if (y == rectTop)
    {
        return RectEdge::Top;
    }

    if (y == rectBottom)
    {
        return RectEdge::Bottom;
    }

    return RectEdge::None;
}

qreal TinyMath::determineTringleSideLength(qreal altitudeLength)
{
    return altitudeLength > 0 ? 2 * altitudeLength / sqrt(3) : altitudeLength;
}

qreal TinyMath::rightAngleInDegrees()
{
    return rightAngleInDegrees_;
}

quint8 TinyMath::minLinePointsCount()
{
    return minLinePointsCount_;
}

quint8 TinyMath::triangleVerteciesCount()
{
    return triangleVerteciesCount_;
}

void TinyMath::setPathWidth(QPainterPath &path, qreal width)
{
    QPainterPathStroker stroker;
    stroker.setWidth(width);
    path = stroker.createStroke(path);
}

QPointF TinyMath::minDistancePoint(const QPointF &target, const QVector<QPointF> &points)
{
    if (points.empty())
    {
        return {};
    }

    if (points.count() == 1)
    {
        return points.first();
    }

    QPointF retPoint = points.first();
    QLineF pLine;
    QLineF retLine;
    for (const auto &point : points)
    {
        pLine.setLine(target.x(), target.y(), point.x(), point.y());
        retLine.setLine(target.x(), target.y(), retPoint.x(), retPoint.y());
        if (pLine.length() < retLine.length())
        {
            retPoint = point;
        }
    }
    return retPoint;
}

QPointF TinyMath::maxDistancePoint(const QPointF &target, const QVector<QPointF> &points)
{
    if (points.empty())
    {
        return {};
    }

    QPointF retPoint = points.first();
    for (const auto &point : points)
    {
        if (distance(target, point) > distance(target, retPoint))
        {
            retPoint = point;
        }
    }
    return retPoint;
}

bool TinyMath::lineProjection(QLineF line, const QPointF &targetPoint, QPointF* projectionPoint)
{
    if (nullptr == projectionPoint)
    {
        return false;
    }

    const QPointF &previousEndPoint = line.p2();
    const qreal previousAngle = line.angle();
    line.setP2(targetPoint);
    const qreal angleOffset = line.angle() - previousAngle;
    const qreal newLength = line.length() * qCos(qDegreesToRadians(angleOffset));
    line.setAngle(previousAngle);
    line.setLength(newLength);
    *projectionPoint = line.p2();
    line.setP2(previousEndPoint);
    return isPointOnLineSegment(line, *projectionPoint);
}

bool TinyMath::isPointOnLineSegment(const QLineF &lineSegment, const QPointF &targetPoint)
{
    const qreal epsilon {1e-3};
    const QPointF &lineSegmentP1 = lineSegment.p1();
    const QLineF &firstPointToTarget {lineSegmentP1, targetPoint};
    const QLineF &targetToFirstPoint {targetPoint, lineSegmentP1};
    const QLineF &targetToSecondPoint {targetPoint, lineSegment.p2()};
    const qreal pseudoScalar = {qAbs(pseudoScalarProduct(lineSegment, firstPointToTarget))};
    const qreal scalar = {scalarProduct(targetToFirstPoint, targetToSecondPoint)};
    return pseudoScalar < epsilon && scalar <= 0;
}

qreal TinyMath::scalarProduct(const QLineF &firstLine, const QLineF &secondLine)
{
    const qreal angleInRadians = {qDegreesToRadians(secondLine.angle() - firstLine.angle())};
    return firstLine.length() * secondLine.length() * qCos(angleInRadians);
}

qreal TinyMath::pseudoScalarProduct(const QLineF &firstLine, const QLineF &secondLine)
{
    const qreal angleInRadians = {qDegreesToRadians(secondLine.angle() - firstLine.angle())};
    return firstLine.length() * secondLine.length() * qSin(angleInRadians);
}

void TinyMath::increaseRect(QRectF &rect, qreal epsilon)
{
    rect.translate(-epsilon, -epsilon);
    rect.setSize(rect.size() + 2 * QSizeF {epsilon, epsilon});
}

void TinyMath::increaseLine(QLineF &line, qreal epsilon)
{
    if (epsilon > 0)
    {
        const qreal currentLength = {line.length()};
        const qreal newLength = {epsilon * currentLength};
        const qreal halfLengthDiff = {(newLength - currentLength) / 2};

        line.setLength(currentLength + halfLengthDiff);
        QLineF tmp {line.p2(), line.p1()};
        tmp.setLength(tmp.length() + halfLengthDiff);

        line = tmp;
    }
}

qreal TinyMath::minRadiusEnabled()
{
    return minRadiusEnabled_;
}

qreal TinyMath::distance(const QPointF &firstPoint, const QPointF &secondPoint)
{
    return QLineF {firstPoint, secondPoint}.length();
}

bool TinyMath::isRectValid(const QRectF &rect)
{
    auto x = rect.x();
    auto y = rect.y();
    auto w = rect.width();
    auto h = rect.height();

    return !qIsNaN(x) && !qIsNaN(y) && !qIsNaN(w) && !qIsNaN(h) && !qIsInf(x) && !qIsInf(y) && !qIsInf(w) && !qIsInf(h);
}
