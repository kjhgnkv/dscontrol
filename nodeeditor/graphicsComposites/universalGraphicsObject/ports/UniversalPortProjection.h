#pragma once

#include "graphicsComposites/universalGraphicsObject/UniversalConnectionableItem.h"
#include <QGraphicsItem>

namespace QtNodes
{
/*!
\brief Class represents projection of item port on inner scene.

This item is located "under" the port but on inner scene and "moves with". Used
to emulate connection of this port with inner scene item port
*/
#pragma pack(push, 1)
class UniversalPortProjection : public UniversalConnectionableItem
{
public:
    UniversalPortProjection(const QUuid &id = QUuid::createUuid(), QGraphicsItem* parentItem = nullptr,
                            QObject* parentObject = nullptr);

    ~UniversalPortProjection() override = default;

    QRectF boundingRect() const override;
    void paint(QPainter*, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    QPainterPath shape() const override;
    QJsonObject save() const override;
    void restore(const QJsonObject &p) override;

    bool setSize(const QSizeF &newSize);

private:
    QSizeF size_;
    QRectF rect_;
};
#pragma pack(pop)
} // namespace QtNodes
