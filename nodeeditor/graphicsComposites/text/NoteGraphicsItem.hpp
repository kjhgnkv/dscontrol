#pragma once

#include "graphicsComposites/universalGraphicsObject/UniversalConnectionableItem.h"
#include "nodeeditor/nodeeditor_export.h"
#include <QUuid>

class QLabel;

namespace QtNodes
{
// class FlowScene;

class DangerousProxyWidget;

class NODEEDITOR_EXPORT NoteGraphicsItem : public UniversalConnectionableItem
{
Q_OBJECT

public:
    enum
    {
        Default,
        Selected
    };

    NoteGraphicsItem(const QUuid &id, const QUuid &parentId, QSizeF size, const QString &text);
    virtual ~NoteGraphicsItem() override;
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* m_widget) override;

    QUuid getParentId();
    void setText(const QString &text);

    QJsonObject save() const override;
    void restore(const QJsonObject &p) override;

signals:
    void pressed();
    void released();
    void doubleClicked();
    void geometryAndTitleChanged(QRectF, QString);

protected:
    //     QVariant itemChange(GraphicsItemChange change,
    //                         const QVariant &   value) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

private:
    void setRect(const QRectF &newRect);

private:
    DangerousProxyWidget* proxyWidget_;
    QLabel* widget_;
    QUuid parentId_;
    QString text_;
    QRectF rect_;
    int state_;
    bool moveFlag_;
    bool locked_;
};
} // namespace QtNodes
