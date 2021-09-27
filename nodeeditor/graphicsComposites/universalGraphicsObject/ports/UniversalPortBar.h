#pragma once

#include "nodes/Serializable.hpp"
#include <QGraphicsItem>
#include <QUuid>

namespace QtNodes
{
class UniversalPort;
class UniversalPortProjection;

/*!
\brief Class represents port bar of item
*/
class UniversalPortBar : public QObject, public Serializable, public QGraphicsItem
{
Q_OBJECT

public:
    /*!
    \param portWidth width of ports on this port bar
    */
    UniversalPortBar(QGraphicsItem* parent, double portWidth = 8);

    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    /*!
    \brief Sets geometry of this port bar and adjust ports according new
    geometry
    */
    void setGeometry(QRectF rect);

    /*!
    \brief Sets port width
    */
    void setPortWidth(double width);

    void setColor(const QColor &newColor);

    /*!
    \brief Adds port to this port bar
    \param type type of new port (UniversalPort::Type)
    \param commId communication id
    \param portProjItem UniversalPortProjection item, that is the projection of
    new port on items inner scene
    \param side side of new port on port bar (UniversalPort::Side)

    \return Uuid of new port
    */
    QUuid addPort(int type, int commId, UniversalPortProjection* portProjItem, int side = 0);
    /*!
    \return port with target Uuid, nullptr if has no
    */
    UniversalPort* getPort(QUuid uid);

    /*!
    \return all ports on this port bar
    */
    std::map<QUuid, UniversalPort*> ports();

    /*!
    \brief Removes port with target Uuid
    */
    void removePort(QUuid uid);

    /*!
    \return port width
    */
    double width() const;

    /*!
    \brief Paints this port bar. Used by UniversalGraphicsItem in paint method
    because of improper parent-child paint sequence
    */
    void paintBar(QPainter* painter);

signals:
    /*!
    \brief emits when port starts moving
    */
    void portMovingHasStarted();
    /*!
    \brief emits when port ends moving
    */
    void portMovingHasFinished();

private:
    double width_;
    double halfWidth_;
    QRectF rect_;
    std::map<QUuid, UniversalPort*> ports_;

    QColor color_;

    // Serializable interface
public:
    QJsonObject save() const;
    void restore(const QJsonObject &p);

    // QGraphicsItem interface
public:
    QPainterPath shape() const;
};
} // namespace QtNodes
