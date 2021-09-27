#pragma once

#include "common/SerializableIdentityUnit.h"
#include <QGraphicsTextItem>

class QGraphicsLineItem;

namespace QtNodes
{
/**
 * @brief Provides a text text item which can be added to the QGraphicsScene and connected with parent item by the line.
 * @note Line is represented by QGraphicsLineItem instance.
 **/
class SpecialTextItem final : public QGraphicsTextItem, public SerializableIdentityUnit
{
public:
    SpecialTextItem(const QString &text, QGraphicsItem* parent, const QUuid &id = QUuid::createUuid());

    QJsonObject save() const override;
    void restore(const QJsonObject &restoringObject) override;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    QGraphicsLineItem* line_;
};
} // namespace QtNodes
