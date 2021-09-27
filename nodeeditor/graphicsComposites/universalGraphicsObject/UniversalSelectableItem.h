#pragma once

#include "common/SerializableIdentityUnit.h"
#include "scenes/FlowScene.hpp"
#include "nodeeditor/nodeeditor_export.h"

#include <QFont>
#include <QGraphicsItem>

class QGraphicsProxyWidget;

namespace QtNodes
{
class ChangeItemPropertyCommand;
class ToolBarWidget;

/*!
\brief Base class for items that can be selected
*/
class NODEEDITOR_EXPORT UniversalSelectableItem : public QObject, public QGraphicsItem, public SerializableIdentityUnit
{
Q_OBJECT

    friend ChangeItemPropertyCommand;

public:
    UniversalSelectableItem(const QUuid &id = QUuid::createUuid(), QGraphicsItem* parentItem = nullptr,
                            QObject* parent = nullptr);

    ~UniversalSelectableItem() override;

    UniversalSelectableItem(const UniversalSelectableItem &another);

    QJsonObject save() const override;
    void restore(const QJsonObject &restoringObject) override;

    /*!
    \return List of all UniversalGraphicsItems on the scene of this item
    */
    QList<UniversalSelectableItem*> siblings(Qt::SortOrder order = Qt::DescendingOrder) const;

    /*!
    \brief Sets this item on top of all other items on the scene
    */
    void setToTopMostVisible();

    /*!
    \brief Sets this item to the bottom of all other items on the scene
    */
    void setToBottomMostVisible();
    bool changesFromBackToForeground() const;

    QList<UniversalSelectableItem*> selectableChildren() const;

    int type() const override;

    virtual void setProperty(const Item::GraphicsItemProperties &property, const QVariant &value);
    virtual QVariant property(const Item::GraphicsItemProperties &property) const;

    FlowScene* flowScene() const;

protected:
    void setChangesFromBackToForeground(bool newChangesFromBackToForeground);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
    virtual void setPropertyCommand(const Item::GraphicsItemProperties &property, const QVariant &value);

    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    QPointF mapToTopScene(const QPointF innerScenePos);

    void removeChildrenFromFlowScene();
    void addChildrenToFlowScene();
    bool hasContainedOnFlowSceneMap() const;

signals:
    /*!
    \brief emits when item selected\unselected
    */
    void itemSelectionChanged(bool selected);

protected:
    qint32 type_;
    QFont font_;
    QColor color_;
    QColor backColor_;
    QColor borderColor_;
    int borderWidth_;

private:
    FlowScene* flowScene_;
    bool changesFromBackToForeground_;
};
} // namespace QtNodes
