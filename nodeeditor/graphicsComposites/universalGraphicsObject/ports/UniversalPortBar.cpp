#include "UniversalPortBar.h"

#include "QJsonArray"
#include "graphicsComposites/universalGraphicsObject/UniversalGraphicsItem.h"
#include "UniversalPortProjection.h"
#include <QDebug>
#include <QPainter>
#include <QUuid>

namespace QtNodes
{
UniversalPortBar::UniversalPortBar(QGraphicsItem* parent, double portWidth)
: QObject {}
, QtNodes::Serializable {}
, QGraphicsItem {parent}
, width_ {portWidth}
, halfWidth_ {portWidth / 2}
, rect_ {{}, QSizeF {}}
{
    setFlag(ItemHasNoContents, true);
}

void UniversalPortBar::setPortWidth(double width)
{
    width_ = width;
    halfWidth_ = width / 2;
}

void UniversalPortBar::setColor(const QColor &newColor)
{
    color_ = newColor;
}

QRectF UniversalPortBar::boundingRect() const
{
    return rect_;
}

void UniversalPortBar::paintBar(QPainter* painter)
{
    painter->setPen(QPen(color_, width_ /*, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin*/));
    QRectF portBarRect = rect_.translated(halfWidth_, halfWidth_);
    portBarRect.setSize({portBarRect.width() - width_, portBarRect.height() - width_});

    painter->setBrush(Qt::black);
    painter->drawRoundedRect(portBarRect, 6, 6);
}

void UniversalPortBar::paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*)
{
}

void UniversalPortBar::setGeometry(QRectF rect)
{
    rect_ = rect;

    for (auto it : ports_)
    {
        UniversalPort* port = it.second;
        if (port)
        {
            emit port->geometryChanged();
            port->correctPosition();
        }
    }
}

QUuid UniversalPortBar::addPort(int type, int commId, UniversalPortProjection* portProjItem, int side)
{
    QUuid uid = QUuid::createUuid();

    UniversalPort* port = new UniversalPort(type, commId, portProjItem, side, width_, uid, this);

    ports_.insert_or_assign(uid, port);

    connect(port, &UniversalPort::movingHasStarted, this, &UniversalPortBar::portMovingHasStarted);
    connect(port, &UniversalPort::movingHasFinished, this, &UniversalPortBar::portMovingHasFinished);

    return uid;
}

UniversalPort* UniversalPortBar::getPort(QUuid uid)
{
    auto it = ports_.find(uid);
    if (it == ports_.end())
        return nullptr;

    return it->second;
}

std::map<QUuid, UniversalPort*> UniversalPortBar::ports()
{
    return ports_;
}

void UniversalPortBar::removePort(QUuid uid)
{
    auto it = ports_.find(uid);
    if (it == ports_.end())
        return;

    UniversalPort* port = it->second;
    port->deleteLater();
    ports_.erase(it);
}

double UniversalPortBar::width() const
{
    return width_;
}

QJsonObject UniversalPortBar::save() const
{
    QJsonObject retval;
    QJsonArray ports;

    for (const auto &[id, port] : ports_)
    {
        QJsonObject p = port->save();
        ports.push_back(p);
    }

    QJsonObject obj = {
    {  "x", pos().x()}
    , {"y", pos().y()}
    };

    retval["pos"] = obj;

    obj = {
    {  "x"     , rect_.x()}
    , {"y"     , rect_.y()}
    , {"width" , rect_.width()}
    , {"height", rect_.height()}
    };

    retval["rect"] = obj;
    retval["ports"] = ports;

    obj = {
    {  "width" , width_}
    , {"height", width_}
    };
    retval["portSize"] = obj;

    return retval;
}

void UniversalPortBar::restore(const QJsonObject &p)
{
    QJsonObject obj = p["pos"].toObject();
    setPos(obj["x"].toDouble(), obj["y"].toDouble());

    obj = p["rect"].toObject();
    rect_ = {
    obj["x"].toDouble(), obj["y"].toDouble(), obj["width"].toDouble(), obj["height"].toDouble(),
    };

    QJsonArray arr = p["ports"].toArray();

    for (const auto &i : arr)
    {
        UniversalPortProjection* portProjItem = nullptr;
        if (auto par = dynamic_cast<UniversalGraphicsItem*>(parentItem()))
        {
            portProjItem = new UniversalPortProjection(
            QUuid::fromString(i.toObject()["projection"].toObject()["id"].toString()));
            par->innerScene()->addItem(portProjItem);
        }

        // obj = i.toObject()["portSize"].toObject();

        UniversalPort* port = new UniversalPort(i.toObject()["type"].toInt(), -1, portProjItem, {}, width_, {}, this);
        connect(port, &UniversalPort::movingHasStarted, this, &UniversalPortBar::portMovingHasStarted);
        connect(port, &UniversalPort::movingHasFinished, this, &UniversalPortBar::portMovingHasFinished);
        port->restore(i.toObject());
        ports_.insert_or_assign(port->id(), port);
    }
}

QPainterPath UniversalPortBar::shape() const
{
    QPainterPath path;
    path.addRect(rect_);

    return path;
}
} // namespace QtNodes
