#pragma once

#include "nodeeditor/nodeeditor_export.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <limits>
#include <type_traits>
#include <QLineF>
#include <QPainterPath>
#include <QRectF>
#include <QVector>

namespace QtNodes
{
enum class RectEdge
{
    Left = 0,
    Right,
    Top,
    Bottom,
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight,
    None
};

class NODEEDITOR_EXPORT TinyMath
{
public:
    static RectEdge determineRectEdge(const QRect &rect, const QPoint &pos);
    static QVector<QPointF> intersectionPoints(QLineF line, QRectF rect);
    static QVector<QPointF> intersectionPoints(const QLineF &line, const QPainterPath &shape);
    static QVector<QLineF> getRectSides(const QRectF &rect);
    static bool rectContainsPoint(QRectF rect, const QPointF &point, qreal error);
    static bool isErrorCurrect(qreal error);
    static bool isPointOnLineSegment(const QLineF &lineSegment, const QPointF &targetPoint);
    static qreal determineTringleSideLength(qreal altitudeLength);
    static qreal rightAngleInDegrees();
    static qreal scalarProduct(const QLineF &firstLine, const QLineF &secondLine);
    static qreal pseudoScalarProduct(const QLineF &firstLine, const QLineF &secondLine);
    static qreal distance(const QPointF &firstPoint, const QPointF &secondPoint);
    static quint8 minLinePointsCount();
    static quint8 triangleVerteciesCount();
    static void setPathWidth(QPainterPath &path, qreal width);
    static void increaseRect(QRectF &rect, qreal epsilon);
    static void increaseLine(QLineF &line, qreal epsilon);
    static qreal minRadiusEnabled();

    /**
     * @brief Determines the point from points with minimal distance to target.
     **/
    static QPointF minDistancePoint(const QPointF &target, const QVector<QPointF> &points);

    /**
     * @brief Determines the point from points with maximum distance to target.
     **/
    static QPointF maxDistancePoint(const QPointF &target, const QVector<QPointF> &points);

    /**
     * @brief Calculates the point of projection of the target point on the line segment.
     * @param [in] The line segment on which the projection is calculated.
     * @param [in] The target point whose projection is calculated.
     * @param [out] A calculated projection point.
     * @return True if calculated projection point is lying on the line segment or false otherwise.
     **/
    static bool lineProjection(QLineF line, const QPointF &targetPoint, QPointF* projectionPoint);

    template <class T>
    static typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type almostEqual(T x, T y)
    {
        /** The machine epsilon has to be scaled to the magnitude of the values
         * used **/
        return std::fabs(x - y) <= std::numeric_limits<T>::epsilon() * std::fabs(x + y)
        /// unless the result is subnormal
        || std::fabs(x - y) < std::numeric_limits<T>::min();
    }

    static bool isRectValid(const QRectF &rect);

private:
    TinyMath() = delete;
    ~TinyMath() = delete;

private:
    static const quint8 minLinePointsCount_;
    static const quint8 triangleVerteciesCount_;
    static const qreal minRadiusEnabled_;
    static const qreal rightAngleInDegrees_;
    static const qreal minErrorLimit_;
    static const qreal maxErrorLimit_;
};
} // namespace QtNodes
