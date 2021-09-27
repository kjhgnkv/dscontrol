#include "UniversalPortProjection.h"

#include <QJsonArray>
#include <QPainter>

namespace QtNodes
{
UniversalPortProjection::UniversalPortProjection(const QUuid &id, QGraphicsItem* parentItem, QObject* parentObject)
: UniversalConnectionableItem(id, parentItem, parentObject)
, size_ {15, 15}
, rect_ {QPointF(), size_}
{
}

QRectF UniversalPortProjection::boundingRect() const
{
    return rect_;
}

void UniversalPortProjection::paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*)
{
}

QPainterPath UniversalPortProjection::shape() const
{
    QPainterPath ret;
    ret.addEllipse(boundingRect());
    return ret;
}

QJsonObject UniversalPortProjection::save() const
{
    QJsonObject retval = UniversalConnectionableItem::save();

    QJsonObject obj = {
    {  "x", pos().x()}
    , {"y", pos().y()}
    };
    retval["pos"] = obj;

    obj = {
    {  "width" , size_.width()}
    , {"height", size_.height()}
    };
    retval["size"] = obj;

    return retval;
}

void UniversalPortProjection::restore(const QJsonObject &p)
{
    UniversalConnectionableItem::restore(p);

    QJsonObject obj = p["pos"].toObject();
    setPos(obj["x"].toDouble(), obj["y"].toDouble());

    obj = p["size"].toObject();
    size_ = {obj["x"].toDouble(), obj["y"].toDouble()};
}

bool UniversalPortProjection::setSize(const QSizeF &newSize)
{
    bool ret = false;
    if (newSize != size_ && newSize.isValid())
    {
        prepareGeometryChange();
        size_ = newSize;
        rect_.setSize(size_);
        ret = true;
        emit geometryChanged();
    }
    return ret;
}
} // namespace QtNodes
