#include "EquilateralTriangle.h"
#include <QJsonArray>

#include "TinyMath.h"

using namespace QtNodes::GeometricShapes;

enum VertexPos : quint8
{
    MainVertex,
    CounterClockwiseVertex,
    ClockwiseVertex
};

EquilateralTriangle::EquilateralTriangle(const QLineF &altitude, const QUuid &id)
: SerializableIdentityUnit {id}
, altitude_ {altitude}
, triangle_(TinyMath::triangleVerteciesCount())
, isVisible_ {false}
, hasSynchronized_ {false}
{
}

void EquilateralTriangle::synchronizeVertecies()
{
    synchronizeMainVertex();
    synchronizeSecondaryVertex(SecondaryVertexType::Clockwise);
    synchronizeSecondaryVertex(SecondaryVertexType::CounterClockwise);
}

void EquilateralTriangle::synchronizeMainVertex()
{
    mainVertex_ = altitude_.p2();
    triangle_[VertexPos::MainVertex] = mainVertex_;
}

void EquilateralTriangle::synchronizeSecondaryVertex(EquilateralTriangle::SecondaryVertexType vertexType)
{
    QLineF tmpAltitude = altitude_;
    const qreal halfLength = {sideLength_ / 2};
    tmpAltitude.setLength(halfLength);
    switch (vertexType)
    {
        case SecondaryVertexType::CounterClockwise:
        {
            tmpAltitude.setAngle(tmpAltitude.angle() + TinyMath::rightAngleInDegrees());
            counterClockwiseVertex_ = tmpAltitude.p2();
            triangle_[VertexPos::CounterClockwiseVertex] = counterClockwiseVertex_;
            break;
        }
        case SecondaryVertexType::Clockwise:
        {
            tmpAltitude.setAngle(tmpAltitude.angle() - TinyMath::rightAngleInDegrees());
            clockwiseVertex_ = tmpAltitude.p2();
            triangle_[VertexPos::ClockwiseVertex] = clockwiseVertex_;
            break;
        }
    }
}

void EquilateralTriangle::synchronize()
{
    if (!hasSynchronized_)
    {
        synchronizeSideLength();
        synchronizeVertecies();
        hasSynchronized_ = true;
    }
}

QJsonObject EquilateralTriangle::save() const
{
    const QPointF &altitudeP1 {altitude_.p1()};
    const QJsonObject &altitudeP1Obj = {{  "x", altitudeP1.x()}
                                        , {"y", altitudeP1.y()}
    };
    const QPointF &altitudeP2 {altitude_.p2()};
    const QJsonObject &altitudeP2Obj = {{  "x", altitudeP2.x()}
                                        , {"y", altitudeP2.y()}
    };
    const QJsonObject &altitudeObj {
    {  "p1", altitudeP1Obj}
    , {"p2", altitudeP2Obj}
    };
    return QJsonObject {
    {  "altitude" , altitudeObj}
    , {"isVisible", isVisible_}
    };
}

void EquilateralTriangle::restore(const QJsonObject &restoringObject)
{
    const QJsonObject &altitudeObj = restoringObject["altitude"].toObject();
    const QJsonObject &altitudeP1Obj = altitudeObj["p1"].toObject();
    const QJsonObject &altitudeP2Obj = altitudeObj["p2"].toObject();
    altitude_.setP1({altitudeP1Obj["x"].toDouble(), altitudeP1Obj["y"].toDouble()});
    altitude_.setP2({altitudeP2Obj["x"].toDouble(), altitudeP2Obj["y"].toDouble()});
    const bool isVisible = restoringObject["isVisible"].toBool();
    hasSynchronized_ = false;
    setVisible(isVisible);
}

void EquilateralTriangle::synchronizeSideLength()
{
    sideLength_ = TinyMath::determineTringleSideLength(altitude_.length());
}

QLineF EquilateralTriangle::altitude() const
{
    return altitude_;
}

QPointF EquilateralTriangle::mainVertex() const
{
    return mainVertex_;
}

QPointF EquilateralTriangle::counterClockwiseVertex() const
{
    return counterClockwiseVertex_;
}

QPointF EquilateralTriangle::clockwiseVertex() const
{
    return clockwiseVertex_;
}

QPolygonF EquilateralTriangle::triangle() const
{
    return triangle_;
}

void EquilateralTriangle::setAltitude(const QLineF &newAltitude)
{
    if (newAltitude == altitude_)
    {
        return;
    }

    altitude_ = newAltitude;
    hasSynchronized_ = false;
    if (isVisible_)
    {
        synchronize();
    }
}

qreal EquilateralTriangle::sideLength() const
{
    return sideLength_;
}

bool EquilateralTriangle::isVisible() const
{
    return isVisible_;
}

void EquilateralTriangle::setVisible(bool newVisible)
{
    if (newVisible == isVisible_)
    {
        return;
    }

    isVisible_ = newVisible;
    if (isVisible_)
    {
        synchronize();
    }
}
