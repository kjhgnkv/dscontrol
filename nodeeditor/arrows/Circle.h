#pragma once

#include "common/SerializableIdentityUnit.h"
#include "TinyMath.h"

namespace QtNodes
{
namespace GeometricShapes
{
/**
 * @brief This class encapsulates the circle as a geometric shape.
 **/
class Circle : public SerializableIdentityUnit
{
public:
    Circle(qreal radius = TinyMath::minRadiusEnabled(), const QPointF &center = {},
           const QUuid &id = QUuid::createUuid());

    const QPointF center() const;
    qreal radius() const;
    bool setCenter(const QPointF &newCenter);
    bool setRadius(qreal newRadius);
    bool contains(const QPointF &checkPoint);
    bool intersects(const Circle &anotherCircle);
    QJsonObject save() const override final;
    void restore(const QJsonObject &restoringObject) override final;

private:
    QPointF center_;
    qreal radius_;
};
} // namespace GeometricShapes
} // namespace QtNodes
