#include "ConnectionArrow.h"

#include "ArrowPicture.h"
#include "nodes/Connection.hpp"
#include "models/NodesScene.hpp"
#include "singletones/Facade.hpp"
#include "singletones/MenuBuilder.h"
#include "singletones/UndoController.hpp"

#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>

using namespace QtNodes;
using namespace QtNodes::Arrows;

ConnectionArrow::ConnectionArrow(const QSet<QtNodes::Connection*> &connections, bool isTop, const QPointF &startPoint,
                                 const QPointF &endPoint, QGraphicsItem* parentItem, QObject* parentObject)
: LineArrow {startPoint, endPoint, QUuid::createUuid(), parentItem, parentObject}
, picture_ {nullptr}
, isTop_ {isTop}
{
    setArrowType(ArrowType::Connection);
    setSmartPointsEnabled(false);
    setMovable(false);
    if (!connections.isEmpty())
    {
        setToolTip((*connections.begin())->id().commName_);
        for (auto connection : connections)
        {
            addConnection(connection, true);
        }
    }
}

ConnectionArrow::~ConnectionArrow()
{
    removePicture();
}

ConnectionArrow::ConnectionArrow(const ConnectionArrow &another)
: LineArrow {another}
, picture_ {nullptr}
, connections_ {another.connections_}
, isTop_ {another.isTop_}
{
    if (nullptr != another.picture_)
    {
        picture_ = new ArrowPicture {another.picture_->fileName(), this};
        picture_->setPos(another.picture_->pos());
    }
}

void ConnectionArrow::updatePictureGeometry()
{
    if (nullptr != picture_ && nullptr != picture_->parentItem())
    {
        const QRectF &rect = boundingRect();
        const QRectF pictureRect = picture_->boundingRect();
        const QPointF &newPos {rect.center() - QPointF {pictureRect.width() / 2, pictureRect.height() / 2}};
        picture_->setPos(newPos);
    }
}

void ConnectionArrow::checkOnVisible()
{
    isAllConnectionsInvisible() ? hide() : show();
}

bool ConnectionArrow::isAllConnectionsVisible() const
{
    for (auto isVisible : connections_)
    {
        if (!isVisible)
        {
            return false;
        }
    }
    return true;
}

bool ConnectionArrow::isAllConnectionsInvisible() const
{
    for (auto isVisible : connections_)
    {
        if (isVisible)
        {
            return false;
        }
    }
    return true;
}

QJsonObject ConnectionArrow::save() const
{
    QJsonObject retval = LineArrow::save();
    if (nullptr != picture_)
    {
        retval["Picture"] = picture_->save();
    }
    retval["ToolTip"] = toolTip();
    return retval;
}

void ConnectionArrow::restore(const QJsonObject &restoringObject)
{
    LineArrow::restore(restoringObject);
    if (nullptr != picture_)
    {
        const QJsonObject &pictureObj = restoringObject["Picture"].toObject();
        picture_->restore(pictureObj);
    }
    const QString &toolTip = restoringObject["ToolTip"].toString();
    setToolTip(toolTip);
}

void ConnectionArrow::updateGeometry(const QGraphicsItem* startItem, const QGraphicsItem* endItem,
                                     const ArrowPosition* position, UpdateGeometryMode mode)
{
    LineArrow::updateGeometry(startItem, endItem, position, mode);
    updatePictureGeometry();
}

AbstractArrow* ConnectionArrow::clone() const
{
    return new ConnectionArrow(*this);
}

void ConnectionArrow::removePicture()
{
    if (nullptr != picture_)
    {
        delete picture_;
        picture_ = nullptr;
    }
}

QSet<const QtNodes::Connection*> ConnectionArrow::connections() const
{
    QSet<const QtNodes::Connection*> ret;
    const auto &stdMapConnections = connections_.toStdMap();
    for (auto connection : stdMapConnections)
    {
        ret.insert(connection.first);
    }
    return ret;
}

void ConnectionArrow::setConnectionVisible(const QtNodes::Connection* connection, bool newIsVisible)
{
    if (nullptr == connection)
    {
        return;
    }
    auto foundedConnection = connections_.find(connection);
    if (connections_.end() != foundedConnection)
    {
        if (foundedConnection.value() != newIsVisible)
        {
            foundedConnection.value() = newIsVisible;
            checkOnVisible();
        }
    }
}

void ConnectionArrow::setConnectionsVisible(const QSet<const QtNodes::Connection*> &connections, bool newIsVisible)
{
    for (auto connection : connections)
    {
        setConnectionVisible(connection, newIsVisible);
    }
}

void ConnectionArrow::hideConnection(const QtNodes::Connection* hidingConnection)
{
    setConnectionVisible(hidingConnection, false);
}

void ConnectionArrow::showConnection(const QtNodes::Connection* showingConnection)
{
    setConnectionVisible(showingConnection, true);
}

void ConnectionArrow::hideConnections(const QSet<const QtNodes::Connection*> &hidingConnections)
{
    setConnectionsVisible(hidingConnections, false);
}

void ConnectionArrow::showConnections(const QSet<const QtNodes::Connection*> &showingConnections)
{
    setConnectionsVisible(showingConnections, true);
}

void ConnectionArrow::addConnection(const QtNodes::Connection* connection, bool visible)
{
    if (nullptr == connection)
    {
        return;
    }

    connections_.insert(connection, visible);
    connect(connection, &QtNodes::Connection::updated, this, &ConnectionArrow::connectionUpdatedSlot);
    connect(connection, &QtNodes::Connection::connectionDestroyed, this, &ConnectionArrow::connectionHasDestroyedSlot);

    if (isTop_)
    {
        connect(connection, &Connection::deviceTypeHasUpdated, this, &ConnectionArrow::setDeviceType);
        connect(connection, &Connection::deviceNameHasUpdated, this, &ConnectionArrow::setDeviceName);
    }
}

void ConnectionArrow::removeConnection(const QtNodes::Connection* removingConnection)
{
    if (nullptr != removingConnection)
    {
        connections_.remove(removingConnection);
    }
}

void ConnectionArrow::setProperty(const Item::GraphicsItemProperties &property, const QVariant &value)
{
    QVariant newValue = value;
    switch (property)
    {
        case Item::GraphicsItemProperties::ArrowDirection:
        {
            newValue = static_cast<qint32>(Direction::NoDirection);
            break;
        }
        default:
        {
            break;
        }
    }
    return LineArrow::setProperty(property, newValue);
}

void ConnectionArrow::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    if (nullptr == event)
    {
        return;
    }
    event->accept();
    UniversalSelectableItem::contextMenuEvent(event);
    auto connectionMenu = MenuBuilder::connectionArrowMenu(this);
    if (nullptr != connectionMenu)
    {
        addToolBar(connectionMenu);
        connectionMenu->exec(QCursor::pos());
        connectionMenu->deleteLater();
    }
}

QVariant ConnectionArrow::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    switch (change)
    {
        case ItemVisibleHasChanged:
        {
            setConnectionsVisible(connections(), value.toBool());
            break;
        }
        default:
        {
            break;
        }
    }
    return LineArrow::itemChange(change, value);
}

void ConnectionArrow::setDeviceName(const QString &name)
{
    if (picture_)
    {
        picture_->setCaption(name);
    }
}

void ConnectionArrow::setPicture(const QString &filename)
{
    QString name;
    if (nullptr != picture_)
    {
        name = picture_->caption();
        delete picture_;
        picture_ = nullptr;
    }

    if (!filename.isEmpty())
    {
        picture_ = new ArrowPicture {filename, this};
        updatePictureGeometry();
        picture_->setCaption(name);
    }
}

void ConnectionArrow::connectionUpdatedSlot(QtNodes::Connection &connection)
{
    setToolTip(connection.id().commName_);
}

void ConnectionArrow::connectionHasDestroyedSlot()
{
    const auto* const senderConnection = qobject_cast<QtNodes::Connection*>(sender());
    removeConnection(senderConnection);
}

void ConnectionArrow::toolBarWidgetPropertyChangedSlot(const Item::GraphicsItemProperties &property,
                                                       const QVariant &value)
{
    if (Item::GraphicsItemProperties::BackColor == property)
    {
        const auto &allScenes = Facade::Instance().sceneModel()->allScenes();
        UndoController::Instance().begin("Connections properties");
        for (auto scene : allScenes)
        {
            if (nullptr != scene)
            {
                scene->setConnectionsProperty(connections(), property, value);
            }
        }
        UndoController::Instance().end();
    }
    else
    {
        LineArrow::toolBarWidgetPropertyChangedSlot(property, value);
    }
}

void ConnectionArrow::setDeviceType(Device::Type deviceType)
{
    const QString &newSvg = createDevicePixmap(deviceType);
    setPicture(newSvg);
}

QString ConnectionArrow::createDevicePixmap(Device::Type type) const
{
    switch (type)
    {
        case Device::Type::General:
        {
            return ":/deviceIcons/General.svg";
        }
        case Device::Type::Switch:
        {
            return ":/deviceIcons/Switch.svg";
        }
        case Device::Type::Router:
        {
            return ":/deviceIcons/Router.svg";
        }
        case Device::Type::SerialPort:
        {
            return ":/deviceIcons/Serial port.svg";
        }
        case Device::Type::Cloud:
        {
            return ":/deviceIcons/Cloud.svg";
        }
        case Device::Type::ShareMemory:
        {
            return ":/deviceIcons/Shared memory.svg";
        }
        case Device::Type::RadioDevice:
        {
            return ":/deviceIcons/Radio.svg";
        }
        default:
        {
            return {};
        }
    }
}
