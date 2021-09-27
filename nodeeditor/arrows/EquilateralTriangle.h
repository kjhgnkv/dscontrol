#pragma once

#include "common/SerializableIdentityUnit.h"

#include <QPainter>
#include <cmath>

namespace QtNodes
{
namespace GeometricShapes
{
/**
 * @brief This class defines an equilateral triangle as a geometryc shape.
 **/
class EquilateralTriangle final : public SerializableIdentityUnit
{
public:
    EquilateralTriangle(const QLineF &altitude = {}, const QUuid &id = QUuid::createUuid());

    QLineF altitude() const;
    QPointF mainVertex() const;
    QPointF counterClockwiseVertex() const;
    QPointF clockwiseVertex() const;
    QPolygonF triangle() const;
    qreal sideLength() const;
    bool isVisible() const;

    void setVisible(bool newVisible);
    void setAltitude(const QLineF &newAltitude);
    void synchronize();

    QJsonObject save() const override;
    void restore(const QJsonObject &restoringObject) override;

private:
    enum class SecondaryVertexType
    {
        Clockwise,
        CounterClockwise
    };

private:
    void synchronizeSideLength();
    void synchronizeVertecies();
    void synchronizeMainVertex();
    void synchronizeSecondaryVertex(SecondaryVertexType vertexType);

private:
    QLineF altitude_;
    qreal sideLength_;
    QPolygonF triangle_;
    QPointF mainVertex_;
    QPointF counterClockwiseVertex_;
    QPointF clockwiseVertex_;
    bool isVisible_;
    bool hasSynchronized_;
};
} // namespace GeometricShapes
} // namespace QtNodes
