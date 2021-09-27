#include "graphicsComposites/universalGraphicsObject/UniversalSelectableItem.h"
//
#include "models/NodesScene.hpp"
#include "views/FlowView.hpp"
//
#include "utils/undo/AbstractSceneCommand.hpp"
//
#include "factories/AbstractCommandFactory.hpp"
#include "factories/CommandFactory.hpp"
//
#include "singletones/Facade.hpp"
#include "singletones/UndoController.hpp"
//
#include "scenes/FlowScene.hpp"
//
#include <QGraphicsProxyWidget>
//
#include "toolbar/ToolBarItem.h"
#include <QDebug>
#include <QGraphicsSceneContextMenuEvent>

namespace QtNodes
{
UniversalSelectableItem::UniversalSelectableItem(const QUuid &id, QGraphicsItem* parentItem, QObject* parent)
: QObject {parent}
, QGraphicsItem {parentItem}
, SerializableIdentityUnit {id}
, type_ {static_cast<int>(QtNodes::Item::NodeType::OtherType)}
, font_ {}
, color_ {}
, backColor_ {}
, borderColor_ {}
, borderWidth_ {}
, flowScene_ {dynamic_cast<FlowScene*>(scene())}
, changesFromBackToForeground_ {true}
{
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

UniversalSelectableItem::~UniversalSelectableItem()
{
    if (nullptr != flowScene_)
    {
        flowScene_->removeGraphicsItem(id());
        removeChildrenFromFlowScene();
    }
}

UniversalSelectableItem::UniversalSelectableItem(const UniversalSelectableItem &another)
: QObject {another.parent()}
, QGraphicsItem {another.parentItem()}
, SerializableIdentityUnit {another}
, type_ {another.type_}
, font_ {another.font_}
, color_ {another.color_}
, backColor_ {another.backColor_}
, borderColor_ {another.borderColor_}
, borderWidth_ {another.borderWidth_}
, flowScene_ {another.flowScene_}
, changesFromBackToForeground_ {another.changesFromBackToForeground_}
{
    setAcceptDrops(another.acceptDrops());
    setAcceptHoverEvents(another.acceptHoverEvents());
    setAcceptTouchEvents(another.acceptTouchEvents());
    setAcceptedMouseButtons(another.acceptedMouseButtons());
    setActive(another.isActive());
    setBoundingRegionGranularity(another.boundingRegionGranularity());
    setCacheMode(another.cacheMode());
    setCursor(another.cursor());
    setEnabled(another.isEnabled());
    setFiltersChildEvents(another.filtersChildEvents());
    setFlags(another.flags());
    setFocusProxy(another.focusProxy());
    setGraphicsEffect(another.graphicsEffect());
    setGroup(another.group());
    setInputMethodHints(another.inputMethodHints());
    setOpacity(another.opacity());
    setPanelModality(another.panelModality());
    setRotation(another.rotation());
    setScale(another.scale());
    setSelected(another.isSelected());
    setToolTip(another.toolTip());
    setTransform(another.transform());
    setTransformOriginPoint(another.transformOriginPoint());
    setTransformations(another.transformations());
    setVisible(another.isVisible());
    setZValue(another.zValue());
}

void UniversalSelectableItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    if (nullptr != event)
    {
        auto sceneModel = Facade::Instance().sceneModel();
        if (sceneModel)
        {
            sceneModel->clearSelection();
        }
        setSelected(true);

        if (!event->isAccepted())
        {
            QGraphicsItem::contextMenuEvent(event);
        }
    }
}

void UniversalSelectableItem::setPropertyCommand(const Item::GraphicsItemProperties &, const QVariant &)
{
}

QJsonObject UniversalSelectableItem::save() const
{
    auto retval = SerializableIdentityUnit::save();
    retval["isEnabled"] = isEnabled();
    retval["isVisible"] = isVisible();
    retval["flags"] = QVariant(static_cast<quint32>(flags())).toJsonValue();
    retval["type"] = type_;
    return retval;
}

void UniversalSelectableItem::restore(const QJsonObject &p)
{
    SerializableIdentityUnit::restore(p);
    type_ = p["type"].toInt();

    setFlags(static_cast<QGraphicsItem::GraphicsItemFlags>(p["flags"].toVariant().toInt()));
    setEnabled(p["isEnabled"].toBool());
    setVisible(p["isVisible"].toBool());
    setSelected(false);
}

QList<UniversalSelectableItem*> UniversalSelectableItem::siblings(Qt::SortOrder order) const
{
    QList<UniversalSelectableItem*> ret;
    auto scene = this->scene();
    if (nullptr != scene)
    {
        auto items = scene->items(order);
        UniversalSelectableItem* universalSelectableItem = nullptr;
        for (const auto &item : items)
        {
            if (universalSelectableItem = dynamic_cast<UniversalSelectableItem*>(item); nullptr != universalSelectableItem && universalSelectableItem != this)
            {
                ret.append(universalSelectableItem);
            }
        }
        universalSelectableItem = nullptr;
        items.clear();
    }
    else
    {
        qWarning() << "UniversalSelectableItem::siblings(): scene is nullptr.";
    }
    return ret;
}

void UniversalSelectableItem::setToTopMostVisible()
{
    auto siblings = this->siblings(Qt::AscendingOrder);
    if (!siblings.empty())
    {
        for (auto sibling : siblings)
        {
            if (sibling->changesFromBackToForeground())
            {
                sibling->stackBefore(this);
                sibling->update();
            }
        }
        update();
    }
}

void UniversalSelectableItem::setToBottomMostVisible()
{
    auto siblings = this->siblings();
    if (!siblings.empty())
    {
        for (auto sibling : siblings)
        {
            if (sibling->changesFromBackToForeground())
            {
                stackBefore(sibling);
                sibling->update();
            }
        }
        update();
    }
}

bool UniversalSelectableItem::changesFromBackToForeground() const
{
    return changesFromBackToForeground_;
}

QList<UniversalSelectableItem*> UniversalSelectableItem::selectableChildren() const
{
    const QList<QGraphicsItem*> &children = childItems();
    QList<UniversalSelectableItem*> ret;
    for (auto child : children)
    {
        if (auto selectableChild = dynamic_cast<UniversalSelectableItem*>(child))
        {
            ret.append(selectableChild);
        }
    }
    return ret;
}

int UniversalSelectableItem::type() const
{
    return type_;
}

void UniversalSelectableItem::setProperty(const Item::GraphicsItemProperties &property, const QVariant &value)
{
    if (nullptr != flowScene_)
    {
        auto command = UndoController::Instance().factory()->createChangeItemPropertyCommand(flowScene_->id(), id(),
                                                                                             property, value, nullptr);
        UndoController::Instance().push(command);
    }

}

QVariant UniversalSelectableItem::property(const Item::GraphicsItemProperties &) const
{
    return {};
}

FlowScene* UniversalSelectableItem::flowScene() const
{
    return flowScene_;
}

void UniversalSelectableItem::setChangesFromBackToForeground(bool newChangesFromBackToForeground)
{
    changesFromBackToForeground_ = newChangesFromBackToForeground;
}

QVariant UniversalSelectableItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change)
    {
        case ItemSceneChange:
        {
            removeChildrenFromFlowScene();
            break;
        }
        case ItemSceneHasChanged:
        {
            flowScene_ = dynamic_cast<FlowScene*>(scene());
            if (hasContainedOnFlowSceneMap())
            {
                addChildrenToFlowScene();
            }
            break;
        }
        case ItemSelectedHasChanged:
        {
            emit itemSelectionChanged(value.toBool());
            break;
        }
        default:
        {
            break;
        }
    }
    return QGraphicsItem::itemChange(change, value);
}

QPointF UniversalSelectableItem::mapToTopScene(const QPointF innerScenePos)
{
    auto sceneModel = Facade::Instance().sceneModel();
    if (!sceneModel)
        return {};

    auto topScene = sceneModel->getTopScene();
    if (!topScene)
        return {};

    auto topView = topScene->flowView();
    if (!topView)
        return {};

    auto itemScene = dynamic_cast<FlowScene*>(scene());
    if (!itemScene)
        return {};

    auto itemView = itemScene->flowView();
    if (!itemView)
        return {};

    QPointF toolBarPos = innerScenePos;

    double sc = 100.0 / dynamic_cast<QtNodes::FlowView*>(topView)->getCurrentScale();

    if (itemView != topView)
    {
        QPointF itemScenePos = innerScenePos;
        QPointF itemViewPos = itemView->mapFromScene(itemScenePos);
        QPointF globalPos = itemView->mapToGlobal(itemViewPos.toPoint());
        QPointF topViewPos = topView->mapFromGlobal(globalPos.toPoint());

        QPointF viewPos = {294, 99}; // Main view position in platform coords
        viewPos = viewPos - viewPos / sc;
        topViewPos += viewPos;

        QMarginsF margins = topView->contentsMargins();
        topViewPos -= QPointF {margins.left() / sc, margins.top() / sc};

        toolBarPos = topView->mapToScene((topViewPos).toPoint());
    }

    return toolBarPos - QPointF {1 * sc, 1 * sc};
}

void UniversalSelectableItem::removeChildrenFromFlowScene()
{
    if (nullptr == flowScene_)
    {
        return;
    }
    const auto &curSelectableChildren = selectableChildren();
    for (auto selectableChild : curSelectableChildren)
    {
        flowScene_->removeGraphicsItem(selectableChild);
    }
}

void UniversalSelectableItem::addChildrenToFlowScene()
{
    if (nullptr == flowScene_)
    {
        return;
    }
    const auto &curSelectableChildren = selectableChildren();
    for (auto selectableChild : curSelectableChildren)
    {
        flowScene_->addGraphicsItem(selectableChild, selectableChild->pos());
    }
}

bool UniversalSelectableItem::hasContainedOnFlowSceneMap() const
{
    if (nullptr != flowScene_)
    {
        const auto &flowSceneGraphicsItems = flowScene_->graphicsItems();
        return !(flowSceneGraphicsItems.find(id()) == flowSceneGraphicsItems.end());
    }
    return false;
}
} // namespace QtNodes
