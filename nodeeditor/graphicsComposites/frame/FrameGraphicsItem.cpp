#include "FrameGraphicsItem.hpp"

#include "common/ArrowController.h"
#include "arrows/AbstractArrow.h"
#include "arrows/Annulus.h"
//
#include "graphicsComposites/universalGraphicsObject/DangerousProxyWidget.h"
#include "FramePropertiesDialog.h"
#include "graphicsComposites/universalGraphicsObject/ResizableItem/ResizeBox.h"
#include "models/NodesScene.hpp"
#include "common/nodeeditor_globals.hpp"
//
#include "nodes/UniversalGraphicsItemImpl.h"
//
#include "graphicsComposites/universalGraphicsObject/ports/UniversalPort.h"
#include "graphicsComposites/universalGraphicsObject/ports/UniversalPortBar.h"
#include "graphicsComposites/universalGraphicsObject/ports/UniversalPortProjection.h"
//
#include "scenes/FlowScene.hpp"
//
#include "singletones/Facade.hpp"
#include "singletones/MenuBuilder.h"
//
#include "graphicsComposites/text/NoteGraphicsItem.hpp"
#include "graphicsComposites/text/TextGraphicsItem.hpp"
//
#include "graphicsComposites/universalGraphicsObject/toolbar/ToolBarItem.h"
#include "graphicsComposites/universalGraphicsObject/toolbar/ToolBarWidget.h"
//
#include "views/FlowView.hpp"
//
#include "graphicsComposites/universalGraphicsObject/widget/UniversalGraphicsWidget.h"
//
#include "graphicsComposites/universalGraphicsObject/toolbar/ToolBarBrush.h"
#include <QApplication>
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QPainter>
#include <QSizePolicy>
#include <QWidget>

#include "graphicsComposites/universalGraphicsObject/UniversalGraphicsItem.h"

namespace QtNodes
{
FrameGraphicsItem::FrameGraphicsItem(QSizeF size, const QUuid &id, QGraphicsItem* parentItem, QObject* parentObject)
: ResizableItem(id, parentItem, parentObject)
, fontDefault_ {"Source Sans Pro", 13}
, colorDefault_ {"#ffffff"}
, borderColorDefault_ {"#2F80ED"}
, borderWidthDefault_ {4}
, manualMove_ {true}
, horSwimlaneCount {1}
, verSwimlaneCount {1}
, title_ {"Boundary"}
{
    type_ = static_cast<int>(QtNodes::Item::NodeType::Frame);

    font_ = fontDefault_;
    color_ = colorDefault_;
    borderColor_ = borderColorDefault_;
    borderWidth_ = borderWidthDefault_;

    setChangesFromBackToForeground(true);
    setFlag(ItemSendsGeometryChanges, true);

    if (resizeBox_)
        resizeBox_->setContentSize(size);

    ResizableItem::updateGeometry();
}

FrameGraphicsItem::~FrameGraphicsItem()
{
}

void FrameGraphicsItem::addToolBar()
{
    ResizableItem::addToolBar();

    if (!toolBarItem_)
        return;

    auto toolBarWidget = toolBarItem_->getToolBarWidget();
    if (!toolBarWidget)
        return;

    toolBarWidget->setMode(ToolBarWidget::Frame);

    toolBarWidget->setProperties(font_, color_, {}, borderColor_, borderWidth_);

    connect(toolBarWidget, &ToolBarWidget::reseted, [=]
    {
        toolBarWidget->setProperties(fontDefault_, colorDefault_, {}, borderColorDefault_, borderWidthDefault_);

        font_ = fontDefault_;
        color_ = colorDefault_;
        borderColor_ = borderColorDefault_;
        borderWidth_ = borderWidthDefault_;

        update();
    });
}

void FrameGraphicsItem::openPropertiesDialog()
{
    FramePropertiesDialog dialog(title_, verSwimlaneCount, horSwimlaneCount);
    dialog.exec();

    if (dialog.result() == QDialog::Accepted)
    {
        title_ = dialog.getTitle();
        verSwimlaneCount = dialog.getVerticalSwimlaneCount();
        horSwimlaneCount = dialog.getHorizontalSwimlaneCount();
    }
}

void FrameGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    // Draw grid
    auto width = rect_.width();
    auto height = rect_.height();
    auto cellWidth = width / verSwimlaneCount;
    auto cellHeight = height / horSwimlaneCount;
    auto verLineCount = verSwimlaneCount - 1;
    auto horLineCount = horSwimlaneCount - 1;

    painter->setPen({{"#cccccc"}, 1, Qt::SolidLine});
    for (auto i = 1; i <= verLineCount; i++)
    {
        auto x = int(cellWidth * i);
        painter->drawLine(x, borderWidth_, x, int(height - 1));
    }
    for (auto j = 1; j <= horLineCount; j++)
    {
        auto y = int(cellHeight * j);
        painter->drawLine(borderWidth_, y, int(width - 1), y);
    }

    // Border
    painter->setPen({borderColor_, double(borderWidth_)});
    QRectF borderRect = rect_.translated(double(borderWidth_) / 2, double(borderWidth_) / 2);
    borderRect.setSize({borderRect.width() - borderWidth_, borderRect.height() - borderWidth_});
    painter->drawRoundedRect(borderRect, 8, 8);

    // Draw title
    double padding = 10;
    QRectF textRect = rect_.translated(padding, padding);
    textRect.setSize(rect_.size() - QSizeF {padding * 2, padding * 2});
    painter->setPen(color_);
    painter->setFont(font_);
    QTextOption textOption(Qt::AlignHCenter);
    textOption.setWrapMode(QTextOption::WrapMode::WrapAtWordBoundaryOrAnywhere);
    painter->drawText(textRect, title_, textOption);
}

QVariant FrameGraphicsItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change)
    {
        case ItemSelectedHasChanged:
        {
            auto isSelected = value.toBool();

            if (isSelected)
            {
                emit geometryAndTitleChanged({{scenePos().rx(), scenePos().ry()}, rect_.size()}, tr("Boundary"));
            }
            break;
        }
        case ItemPositionChange:
        {
            if (!isMoving_)
            {
                moveItemInRect();
            }
            break;
        }
        default:
            break;
    }
    return ResizableItem::itemChange(change, value);
}

void FrameGraphicsItem::setPropertyCommand(const Item::GraphicsItemProperties &property, const QVariant &value)
{
    switch (property)
    {
        case Item::GraphicsItemProperties::Font:
        {
            if (value.canConvert<QString>())
            {
                QString fontStr = value.toString();
                QFont font;
                if (font.fromString(fontStr))
                {
                    font_ = font;
                }
            }
            break;
        }
        case Item::GraphicsItemProperties::Color:
        {
            if (value.canConvert<QColor>())
            {
                color_ = value.value<QColor>();
            }
            break;
        }
        case Item::GraphicsItemProperties::BorderColor:
        {
            if (value.canConvert<QColor>())
            {
                auto colorName = value.value<QColor>().name();
                borderColor_ = colorName;
            }
            break;
        }
        case Item::GraphicsItemProperties::BorderWidth:
        {
            if (value.canConvert<int>())
            {
                borderWidth_ = value.toInt();
            }
            break;
        }
        default:
        {
            ResizableItem::setPropertyCommand(property, value);
            break;
        }
    }

    update();
}

void FrameGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    openPropertiesDialog();
    ResizableItem::mouseDoubleClickEvent(event);
}

void FrameGraphicsItem::setManualMove(bool b)
{
    manualMove_ = b;
}

void FrameGraphicsItem::updateGeometry()
{
    ResizableItem::updateGeometry();

    if (toolBarBrush_)
    {
        toolBarBrush_->setPos(rect_.topRight() + QPointF {12, 0});
    }

    auto currentScene = scene();
    if (nullptr == currentScene)
    {
        return;
    }

    auto items = currentScene->items(mapToScene(rect_).boundingRect(), Qt::ItemSelectionMode::ContainsItemShape);
    for (auto item : items)
    {
        stackBefore(item);
    }

    auto currentScenePos = scenePos();
    emit geometryAndTitleChanged({{currentScenePos.rx(), currentScenePos.ry()}, rect_.size()}, tr("Boundary"));

    currentScene->update(); ///< Is this really necessary?
}

int FrameGraphicsItem::type() const
{
    return static_cast<int>(Item::NodeType::Frame);
}

QJsonObject FrameGraphicsItem::save() const
{
    qDebug() << "FrameGraphicsItem: save";
    QJsonObject retval = ResizableItem::save();
    retval["borderColor"] = borderColor_.name();
    retval["borderWidth"] = borderWidth_;
    return retval;
}

void FrameGraphicsItem::restore(const QJsonObject &p)
{
    ResizableItem::restore(p);

    borderColor_ = QColor {p["borderColor"].toString()};
    borderWidth_ = p["borderWidth"].toInt();
}

void FrameGraphicsItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    if (nullptr != event)
    {
        event->accept();
        UniversalConnectionableItem::contextMenuEvent(event);

        if (auto curScene = dynamic_cast<QtNodes::FlowScene*>(scene()); curScene)
        {
            auto resultMenu = curScene->createContextMenu(event->scenePos());

            resultMenu->addAction(QAction::tr("Note"));

            resultMenu->addSeparator();

            resultMenu->addAction(QIcon(":/contextMenuIcons/SetImage.png"), QAction::tr("Set image"));
            auto lockUnlockAction = resultMenu->addAction(QIcon(":/contextMenuIcons/Lock.png"), QAction::tr("Lock"));
            connect(lockUnlockAction, &QAction::triggered, [this]()
            {
                setLocked(!isLocked_);
            });
            if (isLocked())
            {
                lockUnlockAction->setIcon(QIcon(":/contextMenuIcons/Unlock.png"));
                lockUnlockAction->setText(QAction::tr("Unlock"));
            }

            auto deleteObjectAction = resultMenu->addAction(QIcon(":/delete.png"), tr("Delete group frame"));
            connect(deleteObjectAction, &QAction::triggered, this, [curScene, this]()
            {
                curScene->deleteGraphicsItem(id());
            });

            // Properties action
            auto propAction = resultMenu->addAction(QAction::tr("Properties"));
            connect(propAction, &QAction::triggered, this, &FrameGraphicsItem::openPropertiesDialog);

            resultMenu->setFixedWidth(300);

            // Set shadow effect
            MenuBuilder::setMenuEffect(resultMenu);

            resultMenu->exec(QCursor::pos());
            resultMenu->deleteLater();
        }
    }
}

// TODO SCALE REFACTORING
void FrameGraphicsItem::setCurrentScale(double scale)
{
    correctToolBarBrushSize(scale);

    ResizableItem::setCurrentScale(scale);
}

void FrameGraphicsItem::moveItemInRect()
{
    // When ResizableItem resizes it sets move flag to false, here we use it
    if (manualMove_ && (flags() & ItemIsMovable))
    {
        // setToBottomMostVisible();
        auto deltaX = resizeBox_->scenePos().rx() - scenePos().rx() + 8;
        auto deltaY = resizeBox_->scenePos().ry() - scenePos().ry() + 8;
        auto curScene = flowScene();
        if (nullptr == curScene)
        {
            return;
        }

        QPainterPath shapeInSceneCoords = mapToScene(shape());
        auto itemsInside = curScene->itemsInPath(shapeInSceneCoords);
        auto connectionableItems = curScene->connectionableItemsInPath(shapeInSceneCoords);

        QSet<Arrows::AbstractArrow*> arrows;

        for (auto item : itemsInside)
        {
            if (item->isSelected())
            {
                continue;
            }
            auto selectableItem = dynamic_cast<UniversalSelectableItem*>(item);
            if (nullptr == selectableItem)
            {
                continue;
            }

            auto type = static_cast<Item::NodeType>(selectableItem->type());
            if (type != Item::NodeType::OtherType)
            {
                if (type == Item::NodeType::Frame)
                {
                    dynamic_cast<FrameGraphicsItem*>(selectableItem)->setManualMove(false);
                }
                else
                {
                    selectableItem->blockSignals(true);
                }

                if (this != selectableItem)
                {
                    selectableItem->moveBy(deltaX, deltaY);
                    stackBefore(selectableItem);
                }

                if (type == Item::NodeType::Frame)
                {
                    dynamic_cast<FrameGraphicsItem*>(selectableItem)->setManualMove(true);
                }
                else
                {
                    selectableItem->blockSignals(false);
                }
            }
            else
            {
                // TODO think about native annulus moving and
                // synchronizing other with something delta
                auto annulus = dynamic_cast<GeometricShapes::Annulus*>(item);
                if (nullptr == annulus)
                {
                    continue;
                }
                auto arrow = dynamic_cast<Arrows::AbstractArrow*>(annulus->parentItem());
                if (nullptr == arrow)
                {
                    continue;
                }
                auto controller = curScene->arrowController(arrow->id());
                if (nullptr == controller)
                {
                    continue;
                }
                bool isSourceInRect = connectionableItems.find(controller->source()) != connectionableItems.end();
                bool isDestInRect = connectionableItems.find(controller->destination()) != connectionableItems.end();
                if (isSourceInRect || isDestInRect)
                {
                    annulus->moveBy(deltaX, deltaY);
                    stackBefore(selectableItem);
                    arrows.insert(arrow);
                }
            }
        }
        for (auto arrow : arrows)
        {
            emit arrow->arrowNeedToUpdateGeometry();
            arrow->synchronize();
        }
    }
}

void FrameGraphicsItem::correctToolBarBrushSize(double scale)
{
    if (toolBarBrush_)
        toolBarBrush_->setScale(100 / scale);
}

QVariant FrameGraphicsItem::property(const Item::GraphicsItemProperties &property) const
{
    switch (property)
    {
        case Item::GraphicsItemProperties::BorderColor:
        {
            return borderColor_;
        }
        case Item::GraphicsItemProperties::BorderWidth:
        {
            return borderWidth_;
        }
        default:
        {
            return ResizableItem::property(property);
        }
    }
}

} // namespace QtNodes
