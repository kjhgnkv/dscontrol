#pragma once

#include "LineArrow.h"

namespace QtNodes
{
class Connection;
namespace Arrows
{
class ConnectionArrow final : public LineArrow
{
public:
    ConnectionArrow(const QSet<QtNodes::Connection*> &connections, bool isTop, const QPointF &startPoint = {},
                    const QPointF &endPoint = {}, QGraphicsItem* parentItem = nullptr, QObject* parentObject = nullptr);

    ConnectionArrow(const ConnectionArrow &another);
    ~ConnectionArrow() override;

private:
    ConnectionArrow(ConnectionArrow &&) = delete;
    ConnectionArrow &operator=(const ConnectionArrow &) = delete;
    ConnectionArrow &operator=(ConnectionArrow &&) = delete;

public:
    QJsonObject save() const override;
    void restore(const QJsonObject &restoringObject) override;
    AbstractArrow* clone() const override;
    void updateGeometry(const QGraphicsItem* startItem, const QGraphicsItem* endItem, const ArrowPosition* position,
                        UpdateGeometryMode mode = UpdateGeometryMode::UpdatedBothItems) override;

    QSet<const QtNodes::Connection*> connections() const;

    void removePicture();
    void setPicture(const QString &filename);
    void setDeviceName(const QString &name);

    void setConnectionVisible(const QtNodes::Connection* connection, bool newIsVisible);
    void hideConnection(const QtNodes::Connection* hidingConnection);
    void showConnection(const QtNodes::Connection* showingConnection);

    void setConnectionsVisible(const QSet<const QtNodes::Connection*> &connections, bool newIsVisible);
    void hideConnections(const QSet<const QtNodes::Connection*> &hidingConnections);
    void showConnections(const QSet<const QtNodes::Connection*> &showingConnections);

    void addConnection(const QtNodes::Connection* connection, bool visible);
    void removeConnection(const QtNodes::Connection* removingConnection);

    void setProperty(const Item::GraphicsItemProperties &property, const QVariant &value) override;

    void setDeviceType(Device::Type deviceType);

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    void updatePictureGeometry();

private:
    bool isAllConnectionsVisible() const;
    bool isAllConnectionsInvisible() const;

    void checkOnVisible();

    QString createDevicePixmap(Device::Type type) const;

protected slots:
    void toolBarWidgetPropertyChangedSlot(const Item::GraphicsItemProperties &property, const QVariant &value) override;

private slots:
    void connectionUpdatedSlot(QtNodes::Connection &updatedConnection);
    void connectionHasDestroyedSlot();

private:
    class ArrowPicture;

private:
    ArrowPicture* picture_;
    QMap<const QtNodes::Connection*, bool> connections_;
    bool isTop_;
};
} // namespace Arrows
} // namespace QtNodes
