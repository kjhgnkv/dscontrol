#pragma once

#include "AbstractArrow.h"
#include "nodeeditor/nodeeditor_export.h"

#include <memory>

namespace QtNodes
{
namespace Arrows
{
class NODEEDITOR_EXPORT PolyLineArrow : public AbstractArrow
{
Q_OBJECT

private:
    PolyLineArrow(PolyLineArrow &&) = delete;
    PolyLineArrow &operator=(const PolyLineArrow &) = delete;
    PolyLineArrow &operator=(PolyLineArrow &&) = delete;

public:
    PolyLineArrow(const PolyLineArrow &another);
    PolyLineArrow(const QPointF &startPoint = {}, const QPointF &endPoint = {}, const QUuid &uuid = QUuid::createUuid(),
                  QGraphicsItem* parentItem = nullptr, QObject* parentObject = nullptr);

    AbstractArrow* clone() const override;
    QJsonObject save() const override;
    void restore(const QJsonObject &restoringObject) override;
    void synchronize() override final;

    QPolygonF line() const;
    quint8 maxTransitPointsCount() const;
    quint8 maxPointsCount() const;

    virtual bool setLine(const QPolygonF &newLine);
    virtual void setMaxTransitPointsCount(quint8 newMaxTransitPointsCount);
    bool addPoint(const QPointF &newPoint, quint8 index);

protected:
    std::pair<QPointF*, quint8> minProjectionsPoint(const QPointF &targetPoint) const;

    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    void restoreTransitPoints(const QJsonArray &restoringTransitPoints) override;
    AnnulusSPtrVector transitPoints() const override final;
    QPainterPath paintingLine() const override final;
    std::pair<QPointF, QPointF> findIntersectionPoints(const QPointF &newStartPoint, const QPointF &newEndPoint,
                                                       const QPainterPath &startShape,
                                                       const QPainterPath &endShape) const override;

    void pushTransitPoint(AnnulusSPtr newTransitPoint);
    void resizeTransitPoints(quint8 pointsCount);

private:
    void addTransitPointToTheFlowScene(AnnulusSPtr transitPoint);
    void connectTransitPoint(AnnulusSPtr transitPoint) const;
    qint16 findPointIndexByPos(const QPointF &pos) const;
    QPolygonF createPaintingLinePolygon(const std::pair<QLineF, QLineF> &paintingLineEdges) const;
    QJsonArray saveTransitPoints() const override final;
    QLineF directDirection() const override final;
    QLineF reverseDirection() const override final;
    void resetLine() override final;

    QPointF findStartIntersectionPoint(const QPointF &newStartPoint, const QPointF &newEndPoint,
                                       const QPainterPath &startShape, quint8 currentLinePointsCount) const;

    QPointF findEndIntersectionPoint(const QPointF &newStartPoint, const QPointF &newEndPoint,
                                     const QPainterPath &endShape, quint8 currentLinePointsCount) const;

    bool smartPointCollidesWithTransitPoints(const AnnulusSPtr &targetSmartPoint) const;
    QVector<QPointF> enabledTransitPoints() const;
    QLineF findSelectedLine() const;
    bool isEachParamFineForMousePressEvent(QGraphicsSceneMouseEvent* event);
    void initializeLine();
    void synchronizeLine();
    void pointAddintOption(const QPointF &clickPos);
    void lineMovingOption(const QPointF &clickPos);

private slots:
    void smartPointPositionHasChangedSlot();

private:
    enum class Mode
    {
        LineMoving,
        PointAdding,
        Default,
    };

    Mode mode_;
    quint8 maxTransitPointsCount_;
    std::pair<AnnulusSPtr, AnnulusSPtr> selectedLine_;

    AnnulusSPtrVector smartTransitPoints_;
    QPolygonF line_;
};
} // namespace Arrows
} // namespace QtNodes
