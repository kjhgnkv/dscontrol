#pragma once

#include "graphicsComposites/universalGraphicsObject/UniversalMovableItem.h"

#include <QPen>
#include <memory>

class QPainter;

namespace QtNodes
{
namespace GeometricShapes
{
class Circle;

/**
 * @brief This class describes a ring-shaped object, a region bounded by two concentric circles.
 **/
#pragma pack(push, 1)
class Annulus : public UniversalMovableItem
{
Q_OBJECT
public:
    Annulus(const QPointF &centerPoint = {}, const QUuid &id = QUuid::createUuid(), QGraphicsItem* parentItem = nullptr,
            QObject* parentObject = nullptr);

    Annulus(const Annulus &another);
    ~Annulus() override;

    qreal defaultRadiiRatio() const;
    qreal outerRadius() const;
    qreal innerRadius() const;
    QColor outerColor() const;
    QColor innerColor() const;
    const std::shared_ptr<Circle> outerCircle() const;
    const std::shared_ptr<Circle> innerCircle() const;
    bool setOuterRadius(qreal newOuterRarius);
    bool setInnerRadius(qreal newInnerRarius);
    void setOuterColor(const QColor &newOuterColor);
    void setInnerColor(const QColor &newInnerColor);

    QJsonObject save() const override;
    void restore(const QJsonObject &restoringObject) override;
    bool contains(const QPointF &point) const override final;
    QRectF boundingRect() const override final;
    QPainterPath shape() const override final;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

signals:
    void positionHasChanged();

private:
    const qreal defaultOuterRadius_;
    const qreal defaultInnerRadius_;
    const qreal defaultRadiiRatio_;
    std::shared_ptr<Circle> outerCircle_;
    std::shared_ptr<Circle> innerCircle_;
    QColor outerColor_;
    QColor innerColor_;
    bool isHovered_;
};
#pragma pack(pop)
} // namespace GeometricShapes
} // namespace QtNodes
