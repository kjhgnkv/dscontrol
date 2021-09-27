#pragma once

#include "PolyLineArrow.h"
#include "nodeeditor/nodeeditor_export.h"

namespace QtNodes
{
namespace Arrows
{
class NODEEDITOR_EXPORT LineArrow : public PolyLineArrow
{
Q_OBJECT
public:
    LineArrow(const QPointF &startPoint = {}, const QPointF &endPoint = {}, const QUuid &uid = QUuid::createUuid(),
              QGraphicsItem* parentItem = nullptr, QObject* parentObject = nullptr);

    virtual AbstractArrow* clone() const override;

private:
    void setMaxTransitPointsCount(quint8 newMaxTransitPointsCount) override final;
};
} // namespace Arrows
} // namespace QtNodes
