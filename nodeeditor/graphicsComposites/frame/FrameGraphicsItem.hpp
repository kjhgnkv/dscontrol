#pragma once

#include <QResizeEvent>
#include <QUuid>
#include "graphicsComposites/universalGraphicsObject/ResizableItem/ResizableItem.h"
#include <memory>

class QGraphicsProxyWidget;

namespace QtNodes
{
namespace Arrows
{
class AbstractArrow;
}
class FlowScene;
class UniversalGraphicsWidget;
class UniversalGraphicsItemImpl;
class ArrowController;
class ResizeBox;
class Corner;
class UniversalPortBar;

/*!
\brief Class represents boundary frame
*/
class NODEEDITOR_EXPORT FrameGraphicsItem : public ResizableItem
{
Q_OBJECT

public:
    enum
    {
        Default,
        Selected
    };

    /*!
    \param size initial size
    */
    FrameGraphicsItem(QSizeF size, const QUuid &id = QUuid::createUuid(), QGraphicsItem* parentItem = nullptr,
                      QObject* parentObject = nullptr);

    virtual ~FrameGraphicsItem() override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    /*!
    \return QtNodes::Item::NodeType::Frame
    */
    int type() const override;

    // Serializable interface
    QJsonObject save() const override;
    void restore(const QJsonObject &p) override;

    /*!
    \brief Sets if this frame is about to move manually or not (i.e. when moves
    boundary frame that contains this frame)
    */
    void setManualMove(bool b);

    /*!
    \brief fix item properties depending on scale
    */
    void setCurrentScale(double scale) override;

    void moveItemInRect();

    /*!
    \brief Increases size of item tool bar brush when view zooming out and
    decreases when zooming in. Uses by FlowView
    \param scale Current view scale
    */
    void correctToolBarBrushSize(double scale);
    QVariant property(const Item::GraphicsItemProperties &property) const override;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
    void setPropertyCommand(const Item::GraphicsItemProperties &property, const QVariant &value) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

private:
    void updateGeometry() override;
    void addToolBar() override;
    void openPropertiesDialog();

private:
    QFont fontDefault_;
    QColor colorDefault_;
    QColor borderColorDefault_;
    int borderWidthDefault_;
    bool manualMove_;
    int horSwimlaneCount;
    int verSwimlaneCount;
    QString title_;
};
} // namespace QtNodes
