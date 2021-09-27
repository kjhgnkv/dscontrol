#include "Circle.h"

#include <QLineF>
#include <QtMath>

using namespace QtNodes::GeometricShapes;

Circle::Circle(qreal radius, const QPointF &center, const QUuid &id)
: SerializableIdentityUnit {id}
, center_ {center}
, radius_ {radius}
{
    const qreal minRadiusEnabled {TinyMath::minRadiusEnabled()};
    if (radius_ < minRadiusEnabled)
    {
        radius = minRadiusEnabled;
    }
}

const QPointF Circle::center() const
{
    return center_;
}

qreal Circle::radius() const
{
    return radius_;
}

bool Circle::setCenter(const QPointF &newCenter)
{
    if (center_ != newCenter)
    {
        center_ = newCenter;
        return true;
    }
    return false;
}

bool Circle::setRadius(qreal newRadius)
{
    if (newRadius >= TinyMath::minRadiusEnabled())
    {
        radius_ = newRadius;
        return true;
    }
    return false;
}

bool Circle::contains(const QPointF &checkPoint)
{
    return pow(radius_, 2) >= pow(checkPoint.x() - center_.x(), 2) + pow(checkPoint.y() - center_.y(), 2);
}

bool Circle::intersects(const Circle &anotherCircle)
{
    const qreal r1 = {radius()};
    const qreal r2 = {anotherCircle.radius()};
    const QPointF &c1 = center();
    const QPointF &c2 = anotherCircle.center();
    const qreal centersDistance = {sqrt(pow(c1.x() - c2.x(), 2) + pow(c1.y() - c2.y(), 2))};

    return (centersDistance >= qAbs(r1 - r2)) && (centersDistance <= r1 + r2);
}

QJsonObject Circle::save() const
{
    QJsonObject retval = SerializableIdentityUnit::save();
    const QJsonObject centerObj = {{  "x", center_.x()}
                                   , {"y", center_.y()}
    };
    retval["center"] = centerObj;
    retval["radius"] = radius_;
    return retval;
}

void Circle::restore(const QJsonObject &restoringObject)
{
    SerializableIdentityUnit::restore(restoringObject);
    const QJsonObject &centerObj = restoringObject["center"].toObject();
    const QPointF &center = {centerObj["x"].toDouble(), centerObj["y"].toDouble()};
    const qreal radius = {restoringObject["radius"].toDouble()};
    setCenter(center);
    setRadius(radius);
}
