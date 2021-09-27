#pragma once

#include "nodes/Serializable.hpp"
#include <QPointF>

namespace QtNodes
{
namespace Arrows
{
struct ArrowPosition : Serializable
{
public:
    QJsonObject save() const override;
    void restore(const QJsonObject &restoringObject) override;

public:
    QPointF lastEndPos;
    QPointF lastStartPos;
    QPointF currentEndPos;
    QPointF currentStartPos;
    QPointF endRelativeRatio;
    QPointF startRelativeRatio;
};
} // namespace Arrows
} // namespace QtNodes
