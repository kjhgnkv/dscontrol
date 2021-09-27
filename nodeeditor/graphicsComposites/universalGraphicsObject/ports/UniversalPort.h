#pragma once

#include "../UniversalConnectionableItem.h"

namespace QtNodes
{
class UniversalPortProjection;

/*!
\brief Class represents port item on UniversalGraphicsItem`s port bar
*/
class UniversalPort : public UniversalConnectionableItem
{
Q_OBJECT

public:
    /*!
    \param type Type of the port (enum Type)
    \param commId communication Id
    \param portProjItem UniversalPortProjection item for this port
    \param side Side of the port on port bar rectangle (enum Side)
    \param width Width of the port
    \param id Uuid of the port
    \param parentItem parent item of the port (i.e. UniversalPortBar)
    */
    UniversalPort(int type, int commId, UniversalPortProjection* portProjItem, int side, double width,
                  const QUuid &id = QUuid::createUuid(), QGraphicsItem* parentItem = nullptr,
                  QObject* parentObject = nullptr);

    ~UniversalPort() override = default;

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    QJsonObject save() const override;
    void restore(const QJsonObject &p) override;

    /*!
    \brief Corrects position of the port according to the port bar geometry
    */
    void correctPosition();
    /*!
    \brief Moves port to the port bar side according to the side property
    */
    void moveToSide();

    /*!
    \return UniversalPortPorojection item of this port
    */
    UniversalConnectionableItem* getPortProjectionItem();

    /*!
    \return true if type and commId are equals to this port type and
    communication Id, else - false
    */
    bool compare(int type, int commId);

    /*!
    \return Uuid of this item
    */
    QUuid index() const;

    /*!
    \return communication Id of this port
    */
    int communicationId() const;

    virtual QPainterPath shape() const override;

public:
    /*!
    \brief type of port
    */
    enum InOutType
    {
        In,
        Out,
        InOut
    };

    enum DeviceType
    {
        General
    };

    /*!
    \brief side of port on port tool bar rectangle
    */
    enum Side
    {
        Top,
        Bottom,
        Left,
        Right
    };

signals:
    /*!
    \brief emits when port starts moving
    */
    void movingHasStarted();
    /*!
    \brief emits when port ends moving
    */
    void movingHasFinished();

private:
    void correctProjection();

private:
    int inOutType_;
    int commId_;
    UniversalPortProjection* portProjItem_;
    double width_;
    QRectF rect_;
    bool leftPressedFlag_;
    QPointF leftPressedPos_;
    int side_;
    QString brashColor_;

    // QGraphicsItem interface
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
};
} // namespace QtNodes
