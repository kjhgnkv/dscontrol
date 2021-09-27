#pragma once

#include "nodeeditor/nodeeditor_export.h"
#include "common/nodeeditor_globals.hpp"
#include "nodes/ConnectionID.hpp"
#include "ports/UniversalPort.h"
#include "ResizableItem/ResizableItem.h"
#include "TitleItem/TitleItem.h"

#include <QGraphicsItem>
#include <QResizeEvent>
#include <QUuid>
#include <memory>

namespace QtNodes
{
namespace Arrows
{
class AbstractArrow;
}
class FlowScene;
class DangerousProxyWidget;
class UniversalGraphicsWidget;
class UniversalGraphicsItemImpl;
class ArrowController;
class ResizeBox;
class Corner;
class UniversalPortBar;
class ToolBarBrush;
class ToolBarItem;
class ToolBarItem;
class TitleItem;

/*!
\brief Class represents graphics item of all nodes on scene (i.e. System,
Subsystem, Computer, Application, Component)
*/
class NODEEDITOR_EXPORT UniversalGraphicsItem : public ResizableItem
{
Q_OBJECT

public:
    /*!
    \param id Uuid of node from model
    \param impl Implementation of item (storage of item properties)
    \param scene Scene of the inner view
    \param scaleCoef Coefficient of scaling inner view (not used now)
    */
    UniversalGraphicsItem(QUuid id, UniversalGraphicsItemImpl* impl, QtNodes::FlowScene* scene);

    ~UniversalGraphicsItem() override;
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* nodeWidget) override;

    /*!
    \brief Updates widget of item
    */
    void updateWidget();

    QVariant property(const Item::GraphicsItemProperties &property) const override;

    int type() const override;

    /*!
    \brief Adds a new port to an item
    \param type Type of the port (In, Out, InOut)
    \param commId Id of the communication for new port
    \param side Side of item where new port will be allocated
    \return Uuid of created port
    */
    QUuid addPort(int type, int commId, int side = UniversalPort::Side::Top);
    /*!
    \return true if item has port with target type and communication id
    */
    bool hasPort(int type, int commId);
    /*!
    \brief Returns port with target type and communication id, if has no, return
    nullptr
    */
    UniversalPort* getPort(int type, int commId);
    /*!
    \brief Returns port with target Uuid, if has no, returns nullptr
    */
    UniversalPort* getPort(QUuid uid);
    /*!
    \brief Removes port with target Uuid from item
    */
    void removePort(QUuid uid);
    /*!
    \return Scene of inner view
    */
    FlowScene* innerScene();

    /*!
    \brief Sets new implementation and update item according implementation
    properties
    */
    void installNewImplementation(UniversalGraphicsItemImpl* impl);

    // Serializable interface
    QJsonObject save() const override;
    void restore(const QJsonObject &p) override;

    /*!
    \brief Sets item title visibility
    \param flag If true - title is visible, false - hidden
    */
    void setTitleViewMode(TitleItem::ViewMode viewMode);
    /*!
    \brief Increases size of item tool bar brush when view zooming out and
    decreases when zooming in. Uses by FlowView
    \param scale Current view scale
    */
    void correctToolBarBrushGeometry(qreal scale);
    void setLocked(bool isLocked);
    /*!
    \return true if item is locked, false - if unlocked
    */

    void setPortBarColor(const QColor &newColor);

    void setPortBarColorToDefault();
    /*!
    \brief Sets item border color
    */
    void setBorderColor(const QColor &color);
    /*!
    \brief Sets item background color
    */
    void setBackColor(const QColor &color);
    /*!
    \brief Sets font color of item title
    */
    void setColor(const QColor &color);
    /*!
    \brief Sets font of item title
    */
    void setFont(const QFont &font);
    /*!
    \brief Sets font family of item title
    */
    void setFontFamily(const QString &family);
    /*!
    \brief Sets font size of item title
    */
    void setFontSize(int size);
    /*!
    \brief Sets item title font italic property
    \param italic if true - italic, false - normal
    */
    void setFontItalic(bool italic);
    /*!
    \brief Sets item title font underline property
    \param underline if true - underline, false - normal
    */
    void setFontUnderline(bool underline);
    /*!
    \brief Sets items picture and switch item to picture mode
    */
    void setPicture(const QString &picture);
    /*!
    \brief Increases and decreases sizes of resize box and border of item depend
    on view scale
    \param scale Current view scale
    */
    void updateViewWithScale(double scale);

    /*!
    \brief fix item properties depending on scale
    */
    void setCurrentScale(double scale) override;

    void setHiddenInnerLevels(const bool &hide);

    void resize(const QSizeF &size) override;

    void setViewMode(int mode);

    void setAsGui(bool isGui);
    bool isGui() const;
    void setGuiXml(const QString &guiXml);
    QString guiXml() const;

signals:
    /*!
    \brief emits when on item mouse pressed
    */
    void pressed();
    /*!
    \brief emits when on item mouse released
    */
    void released();
    /*!
    \brief emits when on  item mouse double clicked
    */
    void doubleClicked();
    /*!
    \brief emits when items title or geometry changes. Uses to update status bar
    */
    void geometryAndTitleChanged(QRectF, QString);
    /*!
    \brief emits when some property of item changes. Uses to update item
    properties widget
    */
    void itemPropertiesChanged();

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
    void setPropertyCommand(const Item::GraphicsItemProperties &property, const QVariant &value) override;

private:
    void updateGeometry() override;
    void addToolBar() override;
    void configureToolBarWidget(ToolBarWidget* toolBarWidget);
    void createPortBar();
    void createWidget();
    void setVisibleComponentSideCorners(bool flag = true);
    void addMenuToolBar(QMenu* menu);

private:
    DangerousProxyWidget* proxyWidget_;
    UniversalGraphicsWidget* nodeWidget_;
    UniversalGraphicsItemImpl* impl_;
    QtNodes::FlowScene* innerScene_;
    UniversalPortBar* portBar_;
    // PictureList *pictureList_;
    TitleItem* titleItem_;
    bool isGui_;
    QString guiXml_;
};
} // namespace QtNodes
