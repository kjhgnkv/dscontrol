#include "ResizableItem.h"
#include "ResizeBox.h"
#include "factories/CommandFactory.hpp"
#include "models/NodesScene.hpp"
#include "scenes/FlowScene.hpp"
#include "singletones/Facade.hpp"
#include "singletones/UndoController.hpp"
#include "utils/undo/ChangeItemPositionCommand.hpp"
#include "views/FlowView.hpp"
#include <QDebug>
#include <QGraphicsProxyWidget>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include "graphicsComposites/universalGraphicsObject/toolbar/ToolBarBrush.h"
#include "graphicsComposites/universalGraphicsObject/toolbar/ToolBarItem.h"
#include "graphicsComposites/universalGraphicsObject/toolbar/ToolBarWidget.h"

using namespace QtNodes;

bool ResizableItem::isMoving_ = false;

ResizableItem::ResizableItem(const QUuid &id, QGraphicsItem* parentItem, QObject* parentObject)
: UniversalConnectionableItem {id, parentItem, parentObject}
, rect_ {0, 0, 300, 300}
, resizeBox_ {nullptr}
, isLocked_ {false}
, toolBarBrush_ {nullptr}
, toolBarItem_ {nullptr}
, brushPadding_ {8}
, currentBrushPadding_ {brushPadding_}
, isMainForAlignment_ {false}
, alignColor_ {"#f2c94c"}
, selectionColor_ {"#2f80ed"}
{
    prepareResizeBox();
    setMovable(false);
    setFlag(ItemIsMovable, true);
    setUndoRedoPositionEnable(true);

    addToolBarBrush();
}

ResizableItem::~ResizableItem()
{
    if (resizeBox_)
    {
        resizeBox_->deleteLater();
        resizeBox_ = nullptr;
    }
}

QRectF ResizableItem::boundingRect() const
{
    return rect_;
}

void ResizableItem::paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*)
{
}

void ResizableItem::setLocked(bool isLocked)
{
    setFlag(ItemIsMovable, !isLocked);

    isLocked_ = isLocked;
    if (resizeBox_)
    {
        resizeBox_->setLocked(isLocked);
    }
}

bool ResizableItem::isLocked() const
{
    return isLocked_;
}

void ResizableItem::enableResizing(bool flag)
{
    if (resizeBox_)
    {
        resizeBox_->enableResizing(flag);
    }
}

void ResizableItem::setMainForAlignment(bool isMainForAlignment)
{
    isMainForAlignment_ = isMainForAlignment;

    if (resizeBox_ && isSelected())
    {
        const QColor &color = isMainForAlignment ? alignColor_ : selectionColor_;
        resizeBox_->setColor(color);
        update();
    }
}

bool ResizableItem::isMainForAlignment() const
{
    return isMainForAlignment_;
}

void ResizableItem::addToolBar()
{
    if (toolBarItem_)
    {
        return;
    }

    auto sceneModel = Facade::Instance().sceneModel();
    if (!sceneModel)
    {
        return;
    }

    auto topScene = sceneModel->getTopScene();
    if (!topScene)
    {
        return;
    }

    toolBarItem_ = new ToolBarItem;
    topScene->addItem(toolBarItem_);

    QPointF toolBarPos = mapToTopScene(toolBarBrush_->scenePos());
    toolBarItem_->setPos(toolBarPos);

    connect(toolBarItem_, &ToolBarItem::hasHidden, [topScene, &toolBarItem = toolBarItem_]() mutable
    {
        topScene->removeItem(toolBarItem);
        toolBarItem->deleteLater();
        toolBarItem = nullptr;
        topScene->update();
    });

    auto toolBarWidget = toolBarItem_->getToolBarWidget();
    if (toolBarWidget)
    {
        connect(toolBarWidget, &ToolBarWidget::itemPropertyChanged, this, &UniversalSelectableItem::setProperty);
    }
}

qreal ResizableItem::width() const
{
    return rect_.width();
}

qreal ResizableItem::height() const
{
    return rect_.height();
}

void ResizableItem::addToolBarBrush()
{
    if (!toolBarBrush_)
    {
        toolBarBrush_ = new ToolBarBrush(this);
        toolBarBrush_->hide();
        connect(toolBarBrush_, &ToolBarBrush::clicked, this, &ResizableItem::addToolBar);
    }
}

QVariant ResizableItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change)
    {
        case ItemSelectedChange:
        {
            if (resizeBox_ && value.toBool())
            {
                resizeBox_->setColor(selectionColor_);
            }
            break;
        }
        case ItemSelectedHasChanged:
        {
            auto selected = value.toBool();
            if (resizeBox_)
            {
                resizeBox_->setVisible(selected);
            }
            if (toolBarBrush_)
            {
                toolBarBrush_->setVisible(selected);
            }
            break;
        }
        case ItemPositionChange:
        {
            if (!isMoving_)
            {
                break;
            }
            if (resizeBox_)
            {
                raiseResizeBox();
                double dm {resizeBox_->margin()};
                resizeBox_->setPos(value.toPointF() - pos() - QPointF {dm, dm});
            }
            return UniversalConnectionableItem::itemChange(change, pos());
        }
        case ItemSceneHasChanged:
        {
            if (auto scene = flowScene())
            {
                auto view = scene->flowView();
                if (view)
                {
                    setCurrentScale(view->getCurrentScale());
                }
            }
            break;
        }
        default:
        {
            break;
        }
    }
    return UniversalConnectionableItem::itemChange(change, value);
}

void ResizableItem::setPropertyCommand(const Item::GraphicsItemProperties &property, const QVariant &value)
{
    switch (property)
    {
        case Item::GraphicsItemProperties::Lock:
        {
            if (value.canConvert<bool>())
            {
                setLocked(value.toBool());
            }
            break;
        }

        default:
        {
            return UniversalConnectionableItem::setPropertyCommand(property, value);
        }
    }
}

void ResizableItem::prepareResizeBox()
{
    if (!resizeBox_)
    {
        resizeBox_ = new ResizeBox(this);

        connect(resizeBox_, &ResizeBox::geometryChangeFinished, [this]
        {
            if (!isLocked_)
            {
                lowerResizeBox();

                QSizeF newSize = resizeBox_->getContentSize();

                if (rect_.size() != newSize)
                    resize(newSize);

                setFlag(ItemIsMovable, true);
            }
        });

        connect(resizeBox_, &ResizeBox::geometryChangeStarted, [this]
        {
            if (!isLocked_)
            {
                raiseResizeBox();
                setFlag(ItemIsMovable, false);
            }
        });
    }

    resizeBox_->hide();

    double margin = resizeBox_->margin();
    resizeBox_->setPos(-margin, -margin);
}

void ResizableItem::updateGeometry()
{
    if (!resizeBox_)
    {
        return;
    }

    double margin = resizeBox_->margin();

    // Move resizable item to resize box position
    QPointF deltaPos = resizeBox_->pos() + QPointF {margin, margin};

    blockSignals(true);
    moveBy(deltaPos.x(), deltaPos.y());

    // Resize resizable item
    QSizeF size = resizeBox_->getContentSize();
    prepareGeometryChange();
    rect_.setSize(size);

    // Reset resize box
    resizeBox_->setPos({-margin, -margin});
    blockSignals(false);

    if (toolBarBrush_)
    {
        toolBarBrush_->setPos(
        rect_.topRight() + QPointF {resizeBox_->currentMargin(), 0} + QPointF {currentBrushPadding_, 0});
    }

    emit geometryChanged();
}

void ResizableItem::setSize(QSizeF size)
{
    resizeBox_->setContentSize(size);
    updateGeometry();
}

void ResizableItem::setMinSize(QSizeF size)
{
    if (resizeBox_)
        resizeBox_->setMinSize(size);
}

void ResizableItem::setCurrentScale(qreal scale)
{
    if (resizeBox_)
    {
        resizeBox_->setCurrentScale(scale);
    }
    currentBrushPadding_ = (100 / scale) * brushPadding_;
    updateGeometry();
}

ResizeBox* ResizableItem::resizeBox() const
{
    return resizeBox_;
}

void ResizableItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (nullptr == event)
    {
        UniversalConnectionableItem::mouseReleaseEvent(event);
    }

    if (Qt::LeftButton != event->button())
    {
        return UniversalConnectionableItem::mouseReleaseEvent(event);
    }

    if (nullptr == resizeBox_)
    {
        return UniversalConnectionableItem::mouseReleaseEvent(event);
    }

    auto currentScene = flowScene();
    if (nullptr == currentScene)
    {
        return UniversalConnectionableItem::mouseReleaseEvent(event);
    }

    isMoving_ = false;

    for (auto it : currentScene->selectedItems())
    {
        if (auto uniItem = dynamic_cast<ResizableItem*>(it))
        {
            uniItem->updateAfterMoving();
        }
    }
    return UniversalConnectionableItem::mouseReleaseEvent(event);
}

void ResizableItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->buttons() == Qt::LeftButton)
        isMoving_ = true;

    UniversalConnectionableItem::mouseMoveEvent(event);
}

void ResizableItem::updateAfterMoving()
{
    lowerResizeBox();
    if (nullptr == resizeBox_)
    {
        qWarning() << "QtNodes::ResizableItem::updateAfterMoving: resizeBox_ is nullptr.";
        return;
    }
    double margin = resizeBox_->margin();
    setPos(pos() + resizeBox()->pos() + QPointF {margin, margin});
    resizeBox_->setPos(-margin, -margin);
}

void ResizableItem::resize(const QSizeF &size)
{
    auto command = UndoController::Instance().factory()->createChangeItemSizeCommand(
    reinterpret_cast<FlowScene*>(scene())->id(), id(), resizeBox_->getContentSize(), size, nullptr);
    if (command)
        UndoController::Instance().push(command);
}

void ResizableItem::raiseResizeBox()
{
    if (resizeBox_)
        resizeBox_->setZValue(childItems().count());
}

void ResizableItem::lowerResizeBox()
{
    if (resizeBox_)
        resizeBox_->setZValue(0);
}

QJsonObject ResizableItem::save() const
{
    QJsonObject retval = UniversalConnectionableItem::save();
    QJsonObject obj = {
    {  "x", pos().x()}
    , {"y", pos().y()}
    };
    retval["pos"] = obj;
    obj = {
    {  "height", rect_.height()}
    , {"width" , rect_.width()}
    };
    retval["size"] = obj;
    return retval;
}

void ResizableItem::restore(const QJsonObject &jsonObject)
{
    UniversalConnectionableItem::restore(jsonObject);
    bool isUndoRedoPositionEnabled = undoRedoPositionEnable();
    setUndoRedoPositionEnable(false);
    QJsonObject tmp = jsonObject["pos"].toObject();
    QPointF pos {tmp["x"].toDouble(), tmp["y"].toDouble()};
    setPos(pos);
    tmp = jsonObject["size"].toObject();
    setSize(QSizeF {tmp["width"].toDouble(), tmp["height"].toDouble()});

    setUndoRedoPositionEnable(isUndoRedoPositionEnabled);
}

QVariant ResizableItem::property(const Item::GraphicsItemProperties &property) const
{
    switch (property)
    {
        case Item::GraphicsItemProperties::Lock:
        {
            return isLocked();
        }
        default:
        {
            return {};
        }
    }
}
