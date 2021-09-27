#pragma once

#include "graphicsComposites/universalGraphicsObject/UniversalSelectableItem.h"
#include <QEvent>
#include <QFont>
#include "graphicsComposites/universalGraphicsObject/ResizableItem/ResizableItem.h"

class QPainter;
class QGraphicsTextItem;
class GraphicsTextItem;

namespace QtNodes
{
class TextGraphicsItem;

class TextItemFilter : public QObject
{
Q_OBJECT
public:
    TextItemFilter(TextGraphicsItem* item)
    : item_(item)
    {
    }

signals:
    void textUpdated();

protected:
    bool eventFilter(QObject* obj, QEvent* event);

private:
    TextGraphicsItem* item_;
};

class TextGraphicsItem : public ResizableItem
{
Q_OBJECT
public:
    TextGraphicsItem(QUuid uid, QUuid parentId = {}, QObject* parent = nullptr);
    ~TextGraphicsItem() override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    Item::NodeType typeN() const;

    QVariant property(const Item::GraphicsItemProperties &property) const override;

    void setParentItem(const QUuid &id);

    QUuid getParentItem();

    QJsonObject save() const override;
    void restore(const QJsonObject &obj) override;

    // QRectF boundingRect() const override;

    QGraphicsTextItem* getTextItem();

    void setText(const QString &text);

    void focusIn();
    void focusOut();
    void setLocked(bool flag);

    /*!
    \brief hides text if scale 30% or lower
    */
    void setCurrentScale(double scale) override;
    void checkPlaceholder();

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

private:
    TextItemFilter* textItemFilter_;

    GraphicsTextItem* textItem_;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
    void setPropertyCommand(const Item::GraphicsItemProperties &property, const QVariant &value) override;

private:
    void updateGeometry() override;
    QRectF getMaxRect();

private:
    void addToolBar(QMenu* menu);
    void setFont(const QFont &font);
    void setColor(const QColor &color);
    void setBackColor(const QColor &color);
    void setBorderColor(const QColor &color);
    void setBorderWidth(const int &width);
    void resetFormat();

    QFont getFont() const;
    QColor getColor() const;
    QColor getBackColor() const;
    QColor getBorderColor() const;
    int getBorderWidth() const;

private:
    QFont defaultFont_, font_;
    QColor defaultColor_, color_;
    QColor defaultBackColor_, backColor_;
    QColor defaultBorderColor_, borderColor_;
    int defaultBorderWidth_, borderWidth_;
    QSizeF minSize_;
    bool placeholderFlag_;
    QUuid parentId_;
};
} // namespace QtNodes
