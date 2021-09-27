#pragma once

#include "../graphicsComposites/universalGraphicsObject/UniversalSelectableItem.h"
#include "nodeeditor/nodeeditor_export.h"

#include <QFont>
#include <QPen>
#include <memory>

namespace QtNodes
{
namespace GeometricShapes
{
class Annulus;
class EdgePoint;
class EquilateralTriangle;
} // namespace GeometricShapes

namespace Arrows
{
using AnnulusSPtr = std::shared_ptr<GeometricShapes::Annulus>;
using EdgePointSPtr = std::shared_ptr<GeometricShapes::EdgePoint>;
using AnnulusSPtrVector = std::vector<AnnulusSPtr>;
using EquilateralTriangleVector = QVector<GeometricShapes::EquilateralTriangle>;

struct ArrowPosition;
enum class ArrowType;
class LineArrow;
class PolyLineArrow;
class CurvedArrow;
class LateralVerticalArrow;
class LateralHorizontalArrow;
class TreeStyleHorizontalArrow;
class TreeStyleVerticalArrow;

enum class Direction
{
    Direct,
    Reverse,
    Both,
    NoDirection
};

enum class UpdateGeometryMode
{
    UpdatedStartItem,
    UpdatedEndItem,
    UpdatedBothItems,
    UpdatedNoItems
};

class NODEEDITOR_EXPORT AbstractArrow : public UniversalSelectableItem
{
Q_OBJECT
public:
    AbstractArrow(const AbstractArrow &another);

    /**
     * @param startPoint: A starting point of an arrow's shaft in the scene's
     *coordinate system.
     * @param endPoint: A ending point of an arrow's shaft in the scene's coordinate system.
     **/
    AbstractArrow(const QPointF &startPoint = {}, const QPointF &endPoint = {}, const QUuid &uid = QUuid::createUuid(),
                  QGraphicsItem* parentItem = nullptr, QObject* parentObject = nullptr);
    ~AbstractArrow() override;

private:
    AbstractArrow(AbstractArrow &&) = delete;
    AbstractArrow &operator=(const AbstractArrow &) = delete;
    AbstractArrow &operator=(AbstractArrow &&) = delete;

public:
    QJsonObject save() const override;
    void restore(const QJsonObject &restoringObject) override;
    void restoreTransitPoints(const QJsonObject &restoringObject);
    QVariant property(const Item::GraphicsItemProperties &property) const override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override final;
    QRectF boundingRect() const override final;
    QPainterPath shape() const override final;

    bool setSmartPointsRadius(qreal newRadius);
    bool setStartPoint(const QPointF &newStartPoint);
    bool setEndPoint(const QPointF &newEndPoint);

    void setHoverWidthRatio(qreal newHoverWidthRatio);
    void setDirection(Direction newDirection);
    void setColor(const QColor &newColor);
    void setWidth(qint32 newWidth);
    void setPen(const QPen &newPen);
    void setBrush(const QBrush &newBrush);
    void setArrowHeadSize(quint8 newArrowHeadSize);
    void setSmartPointsEnabled(bool newSmartPointsEnabled);
    void setSmartStartPointPositionMightBeChanged(bool mightBeChanged);
    void setSmartEndPointPositionMightBeChanged(bool mightBeChanged);
    void setStartPointMovable(bool newMovable);
    void setEndPointMovable(bool newMovable);

    bool setStartPointForce(const QPointF &newStartPoint);
    bool setEndPointForce(const QPointF &newEndPoint);

    void setMovable(bool newIsMovable);
    void resetDrawingMode();
    void setDrawingMode();

    ArrowType arrowType() const;
    Direction direction() const;
    QColor color() const;
    qint32 width() const;
    QPen pen() const;
    QBrush brush() const;
    quint8 arrowHeadSize() const;
    QPointF startPoint() const;
    QPointF endPoint() const;
    qreal smartPointsRadius() const;
    quint8 smartPointsCount() const;
    quint8 transitPointsCount() const;

    bool isEverySmartPointEnabled() const;
    bool isValid() const;
    bool isMovable() const;
    bool isPressed() const;
    bool isStartPointMovable() const;
    bool isEndPointMovable() const;

    virtual AbstractArrow* clone() const = 0;
    virtual void resetLine() = 0;

    virtual void updateGeometry(const QGraphicsItem* startItem, const QGraphicsItem* endItem,
                                const ArrowPosition* position,
                                UpdateGeometryMode mode = UpdateGeometryMode::UpdatedBothItems);

    AbstractArrow* toAnotherArrowType(const ArrowType newArrowType) const;
    virtual void synchronize();

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void setPropertyCommand(const Item::GraphicsItemProperties &property, const QVariant &value) override;
    std::pair<QLineF, QLineF> createPaintingLineEdges() const;
    bool paramsFineForUpdateGeometry(const QGraphicsItem* startItem, const QGraphicsItem* endItem,
                                     const ArrowPosition* position) const;

    EdgePointSPtr smartStartPoint() const;
    EdgePointSPtr smartEndPoint() const;
    EquilateralTriangleVector arrowHeads() const;
    QPainterPath paintingArrow() const;
    bool hasSelectedSmartPoints() const;
    bool isDirectArrowHeadVisible() const;
    bool isReverseArrowHeadVisible() const;
    bool oneOfEdgePointsContains(const QPointF &point) const;
    bool startPointContains(const QPointF &point) const;
    bool endPointContains(const QPointF &point) const;
    AnnulusSPtrVector smartPoints() const;

    bool setSmartPointRadius(AnnulusSPtr smartPoint, qreal newRadius);
    void setArrowType(ArrowType type);
    void showSmartPoints();
    void hideSmartPoints();

    void addToolBar(QMenu* menu); ///< Added by Sudnik Alexander.

    /**
     * @brief Searching the intersection points between arrow shaft and start
     * and end shapes.
     * @param newStartPoint: New start point of the arrow shaft which is in local coordinates.
     * @param newEndPoint: New end point of the arrow shaft which is in local coordinates.
     * @param startShape: The given start shape which is in local coordinates.
     * @param endShape: The given end shape which is in local coordinates.
     * @return The intersection points which are in local coordinates.
     */
    virtual std::pair<QPointF, QPointF> findIntersectionPoints(const QPointF &newStartPoint, const QPointF &newEndPoint,
                                                               const QPainterPath &startShape,
                                                               const QPainterPath &endShape) const = 0;

    virtual AnnulusSPtrVector transitPoints() const = 0;
    virtual void restoreTransitPoints(const QJsonArray &restoringTransitPoints) = 0;
    virtual QPainterPath paintingLine() const = 0;

private:
    virtual QJsonArray saveTransitPoints() const = 0;
    virtual QLineF directDirection() const = 0;
    virtual QLineF reverseDirection() const = 0;
    void updateArrowHeads();
    void updateStartPoint(const QPointF &startIntersectionPoint, const QPointF &oldStartPoint,
                          const QGraphicsItem* startItem, bool &hasPointsUpdated, qreal error);

    void updateEndPoint(const QPointF &endIntersectionPoint, const QPointF &oldEndPoint, const QGraphicsItem* endItem,
                        bool &hasPointsUpdated, qreal error);

    QLineF createAltitude(const QLineF &directionLine) const;
    void setDrawingStyle(QPainter* painter) const;

protected slots:
    virtual void toolBarWidgetPropertyChangedSlot(const Item::GraphicsItemProperties &property, const QVariant &value);

private slots:
    void startPointPositionChangeSlot(const QPointF &newPosInSceneCoords, const QPointF &mouseOffset);
    void endPointPositionChangeSlot(const QPointF &newPosInSceneCoords, const QPointF &mouseOffset);

signals:
    void startPointPositionChange(const QPointF &newPosInSceneCoords, const QPointF &mouseOffset);
    void endPointPositionChange(const QPointF &newPosInSceneCoords, const QPointF &mouseOffset);
    void arrowNeedToUpdateGeometry(Arrows::UpdateGeometryMode mode = UpdateGeometryMode::UpdatedNoItems);
    void destroyMe();
    void arrowMoving(const QPointF &offset);
    void arrowChangingType(QtNodes::Arrows::ArrowType newType);
    void needToUpdateLastArrowPosition();

protected:
    bool needToBeUpdated_;
    Direction direction_;

private:
    static const quint8 maxArrowHeadsCount_;
    const QFont defaultFont_;
    const QColor defaultColor_;
    const QColor defaultBackColor_;
    const QColor defaultBorderColor_;
    const QColor defaultColorInDrawingMode_;
    const qint32 defaultBorderWidth_;

    EquilateralTriangleVector arrowHeads_;
    quint8 arrowHeadSize_;
    EdgePointSPtr startPoint_;
    EdgePointSPtr endPoint_;

    QPen pen_;
    QBrush brush_;
    qint32 width_;
    qreal hoverWidthRatio_;
    bool smartPointsEnabled_;

    bool isHovered_;
    bool isPressed_;

    QPainterPath shape_;
    qreal smartPointsRadius_;
};
} // namespace Arrows
} // namespace QtNodes
