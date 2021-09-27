#pragma once

#include <QObject>
#include <QRectF>

#include "nodeeditor/nodeeditor_export.h"
#include "nodes/Serializable.hpp"

namespace QtNodes
{
class UniversalConnectionableItem;
namespace Arrows
{
enum class UpdateGeometryMode;
struct ArrowPosition;
class AbstractArrow;
} // namespace Arrows

class NODEEDITOR_EXPORT ArrowController final : public QObject, public Serializable
{
Q_OBJECT

public:
    ArrowController(UniversalConnectionableItem* source, UniversalConnectionableItem* destination,
                    Arrows::AbstractArrow* arrow);
    ~ArrowController() override;

private:
    ArrowController() = delete;
    ArrowController(const ArrowController &) = delete;

public:
    bool operator==(const ArrowController &rhs) const;
    bool operator!=(const ArrowController &rhs) const;
    bool operator>=(const ArrowController &rhs) const;
    bool operator<=(const ArrowController &rhs) const;
    bool operator>(const ArrowController &rhs) const;
    bool operator<(const ArrowController &rhs) const;
    QJsonObject save() const override;
    void restore(const QJsonObject &restoringObject) override;
    Arrows::AbstractArrow* arrow() const;
    UniversalConnectionableItem* source() const;
    UniversalConnectionableItem* destination() const;
    bool setArrow(Arrows::AbstractArrow* newArrow);
    void update(Arrows::UpdateGeometryMode mode);

private:
    QPointF calculateCurvedArrowNewEdgePointPos(const QRectF &rect, const QPointF &curEdgePointPos,
                                                const QPointF &mouseOffset) const;

    void curvedArrowStartPointPosChangeOption(QRectF srcRect, const QPointF &curStartPointSrcPos,
                                              const QPointF &mouseOffset, qreal epsilon);

    void curvedArrowEndPointPosChangeOption(QRectF destRect, const QPointF &curEndPointDestPos,
                                            const QPointF &mouseOffset, qreal epsilon);

    void connectArrow();
    void connectSource();
    void connectDestination();
    void setArrowPositionToDefault();
    bool updateStartArrowPosition(const QPointF &newStartArrowPosition);
    bool updateEndArrowPosition(const QPointF &newEndArrowPosition);
    void srcResizedOption(const QRectF &curSrcRect, const QRectF &curSrcSceneRect);
    void destResizedOption(const QRectF &curDestRect, const QRectF &curDestSceneRect);
    Arrows::UpdateGeometryMode determineMode() const;

signals:
    void destroyMe();

private slots:
    void arrowHasDestroyedSlot();
    void destroyArrowSlot();
    void arrowMovingSlot(const QPointF &offset);
    void arrowNeedToUpdateGeometrySlot(Arrows::UpdateGeometryMode mode);
    void sourceItemGeometryChangedSlot();
    void destItemGeometryChangedSlot();
    void itemHasDestroyedSlot();
    void updateLastArrowPositionSlot();
    void arrowStartPointPositionChangeSlot(const QPointF &newPosInSceneCoords, const QPointF &mouseOffset);
    void arrowEndPointPositionChangeSlot(const QPointF &newPosInSceneCoords, const QPointF &mouseOffset);
    void flowViewScaleChangedSlot();

private:
    UniversalConnectionableItem* source_;
    UniversalConnectionableItem* destination_;
    Arrows::AbstractArrow* arrow_;
    Arrows::ArrowPosition* arrowPosition_;

    QRectF previousSourceRect_;
    QRectF previousDestRect_;
};
} // namespace QtNodes
