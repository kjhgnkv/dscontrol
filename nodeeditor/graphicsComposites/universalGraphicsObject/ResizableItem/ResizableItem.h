#pragma once

#include "graphicsComposites/universalGraphicsObject/UniversalConnectionableItem.h"

namespace QtNodes
{
class ResizeBox;
// class ChangeItemPositionCommand;
class ToolBarBrush;
class ToolBarItem;

#pragma pack(push, 1)
class ResizableItem : public UniversalConnectionableItem
{
Q_OBJECT

    // friend ChangeItemPositionCommand;

public:
    ResizableItem(const QUuid &id = QUuid::createUuid(), QGraphicsItem* parentItem = nullptr,
                  QObject* parentObject = nullptr);
    ~ResizableItem() override;
    virtual void updateGeometry();
    void setSize(QSizeF size);
    void setMinSize(QSizeF size);
    virtual void setCurrentScale(qreal scale);
    ResizeBox* resizeBox() const;
    void setLocked(bool setLocked);
    bool isLocked() const;
    void enableResizing(bool flag = true);
    void setMainForAlignment(bool isMainForAlignment);
    bool isMainForAlignment() const;
    QJsonObject save() const override;
    void restore(const QJsonObject &jsonObject) override;
    QVariant property(const Item::GraphicsItemProperties &property) const override;
    QRectF boundingRect() const override;
    void updateAfterMoving();
    virtual void resize(const QSizeF &size);
    virtual void addToolBar();
    qreal width() const;
    qreal height() const;

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void setPropertyCommand(const Item::GraphicsItemProperties &property, const QVariant &value) override;
    void raiseResizeBox();
    void lowerResizeBox();

private:
    void prepareResizeBox();
    void addToolBarBrush();

signals:
    void geometryAndTitleChanged(QRectF, QString);

protected:
    QRectF rect_;
    ResizeBox* resizeBox_;
    static bool isMoving_;
    bool isLocked_;
    ToolBarBrush* toolBarBrush_;
    ToolBarItem* toolBarItem_;
    qreal brushPadding_;
    qreal currentBrushPadding_;

private:
    bool isMainForAlignment_;
    QColor alignColor_;
    QColor selectionColor_;
};
#pragma pack(pop)
} // namespace QtNodes
