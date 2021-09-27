#pragma once

#include "ConnectionArrow.h"
#include <QGraphicsSvgItem>

namespace QtNodes
{
class SpecialTextItem;
namespace Arrows
{
class ConnectionArrow::ArrowPicture final : public QGraphicsSvgItem, public SerializableIdentityUnit
{
public:
    ArrowPicture(const QString &filename = {}, QGraphicsItem* parent = nullptr, const QUuid &id = QUuid::createUuid());

    ArrowPicture(const QString &fileName, const QString &caption, QGraphicsItem* parent = nullptr,
                 const QUuid &id = QUuid::createUuid());

    QString fileName() const;
    QString caption() const;
    void setCaption(const QString &newCaption);
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    QJsonObject save() const override;
    void restore(const QJsonObject &restoringObject) override;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private:
    QString filename_;
    SpecialTextItem* title_;
    const QRectF rect_;
};
}
}
