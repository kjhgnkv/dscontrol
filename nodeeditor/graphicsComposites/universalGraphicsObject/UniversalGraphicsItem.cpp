#include "graphicsComposites/universalGraphicsObject/UniversalGraphicsItem.h"
#include "nodes/ComponentItemImpl.h"
#include "DangerousProxyWidget.h"
#include "ResizableItem/ResizeBox.h"
#include "TitleItem/TitleItem.h"
#include "factories/CommandFactory.hpp"
#include "graphicsComposites/frame/FrameGraphicsItem.hpp"
#include "imageSelector/ImageSelector.hpp"
#include "models/NodesScene.hpp"
#include "nodes/UniversalGraphicsItemImpl.h"
#include "scenes/FlowScene.hpp"
#include "utils/undo/ChangeItemPropertyCommand.hpp"
#include "views/FlowView.hpp"
#include "ports/UniversalPort.h"
#include "ports/UniversalPortBar.h"
#include "ports/UniversalPortProjection.h"
#include "singletones/Facade.hpp"
#include "singletones/MenuBuilder.h"
#include "singletones/UndoController.hpp"
#include "graphicsComposites/text/NoteGraphicsItem.hpp"
#include "graphicsComposites/text/TextGraphicsItem.hpp"
#include "toolbar/ToolBarBrush.h"
#include "toolbar/ToolBarItem.h"
#include "toolbar/ToolBarWidget.h"
#include "views/FlowView.hpp"
#include "widget/UniversalGraphicsWidget.h"
#include "widgets/TextInputDialog.h"

#include <QApplication>
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QPainter>
#include <QSizePolicy>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <algorithm>

using namespace QtNodes;

UniversalGraphicsItem::UniversalGraphicsItem(QUuid id, UniversalGraphicsItemImpl* impl, QtNodes::FlowScene* innerScene)
: ResizableItem {id}
, nodeWidget_ {nullptr}
, impl_ {nullptr}
, innerScene_ {innerScene}
, portBar_ {nullptr}
, titleItem_ {nullptr}
, isGui_ {false}
{
    setFlags(flags() | ItemSendsGeometryChanges);

    createWidget();

    createPortBar();

    titleItem_ = new TitleItem {impl->deploymentType(), this};

    installNewImplementation(impl);

    titleItem_->setFont(impl->getFont());

    if (resizeBox_)
    {
        resizeBox_->setCornerWidth(impl->getCornerWidth());
    }

    font_ = impl->getFont();
    color_ = impl->getColor();
    backColor_ = impl->getBackColor();
    borderColor_ = impl->getBorderColor();
    borderWidth_ = impl->getBorderWidth();

    // Min UGI size depening on title size
    connect(impl_, &UniversalGraphicsItemImpl::minSizeChanged, [=](const QSizeF &size)
    {
        if (resizeBox_)
        {
            resizeBox_->setMinSize(size);
        }
        updateGeometry();
    });

    setVisibleComponentSideCorners(false);
}

UniversalGraphicsItem::~UniversalGraphicsItem()
{
    if (nodeWidget_)
    {
        nodeWidget_->deleteLater();
    }
    if (impl_)
    {
        impl_->setInstall(false);
    }
}

void UniversalGraphicsItem::installNewImplementation(UniversalGraphicsItemImpl* impl)
{
    if (impl_)
    {
        disconnect(impl_, nullptr, this, nullptr);
    }

    impl_ = impl;
    if (impl_)
    {
        impl->setInstall(true);
        impl->setInnerScene(innerScene_->id());

        if (portBar_)
        {
            portBar_->setPortWidth(impl_->portWidth());
        }
        setPos(impl_->getPos());
        setSize(impl_->getSize());

        impl_->addWidget(nodeWidget_);
        if (resizeBox_)
        {
            resizeBox_->setType(impl_->getType());
            resizeBox_->setMinSize(impl_->getMinSize());
        }

        borderColor_ = impl->getBorderColor();
        borderWidth_ = impl->getBorderWidth();
        setLocked(impl_->isLocked());

        connect(impl_, &UniversalGraphicsItemImpl::deploymentTypeChanged, this, &UniversalGraphicsItem::updateWidget);
        connect(impl_, &UniversalGraphicsItemImpl::nodeUpdated, this, &UniversalGraphicsItem::updateWidget);

        updateWidget();
        updateGeometry();
    }
}

void UniversalGraphicsItem::setCurrentScale(double scale)
{
    correctToolBarBrushGeometry(scale);
    updateViewWithScale(scale);

    ResizableItem::setCurrentScale(scale);
    updateGeometry();

    auto innerItems = innerScene_->getResizableItems();
    for (auto innerItem : innerItems)
        if (auto ugi = dynamic_cast<UniversalGraphicsItem*>(innerItem))
            ugi->setCurrentScale(scale);
}

void UniversalGraphicsItem::setHiddenInnerLevels(const bool &hide)
{
    if (hide)
        setViewMode(UniversalGraphicsItemImpl::ViewMode::Hidden);
    else
        setViewMode(UniversalGraphicsItemImpl::ViewMode::Scene);

    if (resizeBox_)
        resizeBox_->setViewMode(impl_->viewMode());

    // FIXME why? for what?
    nodeWidget_->getFlowView()->showTopLevelConnections();
}

void UniversalGraphicsItem::resize(const QSizeF &size)
{
    ResizableItem::resize(size);

    emit itemPropertiesChanged();
}

void UniversalGraphicsItem::updateViewWithScale(double scale)
{
    if (nodeWidget_)
    {
        nodeWidget_->nestedView()->changeScaleForResizableItems(scale);
    }
}

QRectF UniversalGraphicsItem::boundingRect() const
{
    return rect_;
}

void UniversalGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    if (portBar_ && portBar_->isVisible())
        portBar_->paintBar(painter);
}

QVariant UniversalGraphicsItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change)
    {
        case ItemSelectedHasChanged:
        {
            if (value.toBool())
            {
                emit geometryAndTitleChanged({scenePos(), rect_.size()}, impl_->caption());
            }
            break;
        }
        case ItemSceneHasChanged:
        {
            QVariant retValue = ResizableItem::itemChange(change, value);
            if (nullptr != impl_)
            {
                auto scene = flowScene();
                if (nullptr != scene)
                {
                    impl_->setParentScene(scene->id());
                }
            }
            emit sceneHasChanged();
            return retValue;
        }
        case ItemPositionHasChanged:
        {
            if (nullptr != impl_)
            {
                impl_->setPos(pos());
            }

            emit itemPropertiesChanged();
            break;
        }
        default:
        {
            break;
        }
    }
    return ResizableItem::itemChange(change, value);
}

void UniversalGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->buttons() == Qt::LeftButton)
    {
        emit doubleClicked();
        ResizableItem::mouseDoubleClickEvent(event);
    }
}

void UniversalGraphicsItem::updateGeometry()
{
    ResizableItem::updateGeometry();

    if (!impl_ || !proxyWidget_ || !nodeWidget_ || !portBar_ || !titleItem_)
    {
        return;
    }

    qreal dw = portBar_->width();
    if (impl_->viewMode() == UniversalGraphicsItemImpl::ViewMode::Picture)
    {
        dw = 0;
    }

    QRectF widgetRect = rect_.translated(dw, dw);
    widgetRect.setSize({rect_.width() - dw * 2, rect_.height() - dw * 2});

    proxyWidget_->setGeometry(widgetRect);
    nodeWidget_->setGeometry(widgetRect.toRect());

    if (impl_->viewMode() == UniversalGraphicsItemImpl::ViewMode::Picture)
    {
        qreal padding = 8; // Item name padding under item
        titleItem_->setWidth(rect_.width());
        titleItem_->setPos(rect_.bottomLeft() + QPointF {0, padding});
    }
    else
    {
        titleItem_->setWidth(widgetRect.width());
        titleItem_->setPos(widgetRect.topLeft());
    }

    impl_->setPos(pos());
    impl_->setSize(rect_.size());

    if (portBar_)
    {
        portBar_->setGeometry(rect_);
    }

    emit geometryAndTitleChanged({{scenePos().rx(), scenePos().ry()}, rect_.size()}, impl_->caption());
    emit itemPropertiesChanged();
}

void UniversalGraphicsItem::updateWidget()
{
    if (!impl_ || !nodeWidget_)
        return;

    if (titleItem_)
        titleItem_->setText(impl_->caption());

    setFont(impl_->getFont());
    nodeWidget_->setViewMode(impl_->viewMode());

    if (impl_->viewMode() == UniversalGraphicsItemImpl::ViewMode::Scene || impl_->viewMode() == UniversalGraphicsItemImpl::ViewMode::Component)
    {
        nodeWidget_->setBackColor(impl_->getBackColor());
    }
    else if (impl_->viewMode() == UniversalGraphicsItemImpl::ViewMode::Hidden)
    {
        nodeWidget_->setPicture(impl_->hiddenPicture());
        nodeWidget_->setBackColor(impl_->getBackColor());
    }
    else if (impl_->viewMode() == UniversalGraphicsItemImpl::ViewMode::Picture)
    {
        nodeWidget_->setPicture(impl_->picture());
        nodeWidget_->setBackColor(Qt::transparent);
    }

    if (portBar_)
        portBar_->setColor(impl_->getBorderColor());

    update();
}

QVariant UniversalGraphicsItem::property(const Item::GraphicsItemProperties &property) const
{
    switch (property)
    {
        case Item::GraphicsItemProperties::Color:
        {
            return impl_->getColor();
        }
        case Item::GraphicsItemProperties::Picture:
        {
            return impl_->picture();
        }
        case Item::GraphicsItemProperties::BackColor:
        {
            return impl_->getBackColor();
        }
        case Item::GraphicsItemProperties::Font:
        {
            return impl_->getFontFamily();
        }
        case Item::GraphicsItemProperties::BorderColor:
        {
            return impl_->getBorderColor();
        }
        case Item::GraphicsItemProperties::BorderWidth:
        {
            return impl_->getBorderWidth();
        }
        case Item::GraphicsItemProperties::HideInnerLevels:
        {
            return impl_->viewMode() == UniversalGraphicsItemImpl::ViewMode::Hidden;
        }
        default:
        {
            return ResizableItem::property(property);
        }
    }
}

void UniversalGraphicsItem::createPortBar()
{
    if (!portBar_)
        portBar_ = new UniversalPortBar(this);
}

void UniversalGraphicsItem::createWidget()
{
    proxyWidget_ = new DangerousProxyWidget(this);
    nodeWidget_ = new UniversalGraphicsWidget(innerScene_);
    proxyWidget_->setWidget(nodeWidget_);

    // updateWidget();
    // Set inner scene for item widget
    // TODO we do it 2 times! fix
    nodeWidget_->setScene(innerScene_);
}

void UniversalGraphicsItem::setVisibleComponentSideCorners(bool flag)
{
    if (impl_ && impl_->getType() == Item::NodeType::ComponentNode && resizeBox_)
        resizeBox_->setComponentResize(flag);
}

void UniversalGraphicsItem::setViewMode(int mode)
{
    if (impl_)
        impl_->setViewMode(mode);

    if (mode == UniversalGraphicsItemImpl::ViewMode::Picture)
        portBar_->setVisible(false);
    else
        portBar_->setVisible(true);

    updateWidget();
    updateGeometry();

    emit itemPropertiesChanged();
}

void UniversalGraphicsItem::setAsGui(bool isGui)
{
    isGui_ = isGui;
    if (isGui)
    {
        QString path = {"apps/" + impl_->caption() + ".ui"};
        if (!QFile::exists(path))
        {
            QFile::copy(":/manager/UiExample.ui", path);
        }

        QFile file {path};
        file.open(QFile::ReadOnly);
        guiXml_ = file.readAll();
        file.close();

        emit Facade::Instance().runDesigner(file.fileName());
    }
    else
    {
        guiXml_.clear();
    }
}

bool UniversalGraphicsItem::isGui() const
{
    return isGui_;
}

void UniversalGraphicsItem::setGuiXml(const QString &guiXml)
{
    guiXml_ = guiXml;
}

QString UniversalGraphicsItem::guiXml() const
{
    return guiXml_;
}

void UniversalGraphicsItem::addMenuToolBar(QMenu* menu)
{
    if (!menu)
        return;

    auto toolBarWidget = new ToolBarWidget(menu);

    configureToolBarWidget(toolBarWidget);

    toolBarWidget->setGeometry({{10, 0}, toolBarWidget->size()});
    menu->setFixedWidth(toolBarWidget->width() + 20);
    menu->setContentsMargins(0, toolBarWidget->height() - 4, 0, 0);

    MenuBuilder::setMenuEffect(menu, true);

    toolBarWidget->show();
}

void UniversalGraphicsItem::addToolBar()
{
    ResizableItem::addToolBar();

    if (!toolBarItem_)
    {
        return;
    }

    auto toolBarWidget = toolBarItem_->getToolBarWidget();
    if (!toolBarWidget)
    {
        return;
    }

    configureToolBarWidget(toolBarWidget);
}

void UniversalGraphicsItem::configureToolBarWidget(ToolBarWidget* toolBarWidget)
{
    if (!toolBarWidget || !impl_)
        return;

    toolBarWidget->setMode(ToolBarWidget::Item);
    toolBarWidget->setProperties(impl_->getFont(), impl_->getColor(), impl_->getBackColor(), impl_->getBorderColor(),
                                 impl_->getBorderWidth());

    connect(toolBarWidget, &ToolBarWidget::itemPropertyChanged, this, &UniversalGraphicsItem::setProperty);

    connect(toolBarWidget, &ToolBarWidget::reseted, [=]
    {
        impl_->resetFormat();

        toolBarWidget->setProperties(impl_->getFont(), impl_->getColor(), impl_->getBackColor(),
                                     impl_->getBorderColor(), impl_->getBorderWidth());

        borderColor_ = impl_->getBorderColor();
        borderWidth_ = impl_->getBorderWidth();

        updateWidget();
        emit itemPropertiesChanged();
    });
}

void UniversalGraphicsItem::correctToolBarBrushGeometry(qreal scaleFactor)
{
    if (toolBarBrush_)
    {
        toolBarBrush_->setScale(100 / scaleFactor);
    }

    if (scaleFactor <= 100)
    {
        currentBrushPadding_ = 100 / scaleFactor * brushPadding_;
    }

    auto items = innerScene_->graphicsItems();
    for (const auto &item : items)
    {
        auto ugi = dynamic_cast<UniversalGraphicsItem*>(item.second);
        if (ugi)
        {
            ugi->correctToolBarBrushGeometry(scaleFactor);
        }
        else
        {
            auto fgi = dynamic_cast<FrameGraphicsItem*>(item.second);
            if (fgi)
            {
                fgi->correctToolBarBrushSize(scaleFactor);
            }
        }
    }
}

int UniversalGraphicsItem::type() const
{
    if (!impl_)
        return -1;
    return int(impl_->getType());
}

QUuid UniversalGraphicsItem::addPort(int type, int commId, int side)
{
    // Create port projection item and add it on inner scene
    UniversalPortProjection* portProjItem = new UniversalPortProjection(QUuid::createUuid());
    innerScene_->addItem(portProjItem);
    return portBar_->addPort(type, commId, portProjItem, side);
}

bool UniversalGraphicsItem::hasPort(int type, int commId)
{
    for (const auto &port : portBar_->ports())
    {
        if (port.second->compare(type, commId))
        {
            return true;
        }
    }
    return false;
}

UniversalPort* UniversalGraphicsItem::getPort(int type, int commId)
{
    for (const auto &port : portBar_->ports())
    {
        if (port.second->compare(type, commId))
        {
            return port.second;
        }
    }
    return nullptr;
}

UniversalPort* UniversalGraphicsItem::getPort(QUuid uid)
{
    return portBar_->getPort(uid);
}

void UniversalGraphicsItem::removePort(QUuid uid)
{
    portBar_->removePort(uid);
}

QtNodes::FlowScene* UniversalGraphicsItem::innerScene()
{
    return innerScene_;
}

QJsonObject UniversalGraphicsItem::save() const
{
    QJsonObject retval = ResizableItem::save();
    retval["sceneId"] = innerScene_->id().toString();
    retval["portBar"] = portBar_->save();
    return retval;
}

void UniversalGraphicsItem::restore(const QJsonObject &p)
{
    ResizableItem::restore(p);

    portBar_->restore(p["portBar"].toObject());
}

void UniversalGraphicsItem::setLocked(bool isLocked)
{
    ResizableItem::setLocked(isLocked);

    impl_->setLocked(isLocked);

    titleItem_->setLocked(isLocked);

    emit itemPropertiesChanged();
}

void UniversalGraphicsItem::setPortBarColor(const QColor &newColor)
{
    if (portBar_)
        portBar_->setColor(newColor);
}

void UniversalGraphicsItem::setPortBarColorToDefault()
{
    if (portBar_)
    {
        portBar_->setColor(impl_->borderColorDefault());
    }
}

void UniversalGraphicsItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    if (nullptr != event)
    {
        event->accept();

        auto itemMenu = MenuBuilder::itemMenu(this);
        auto resultMenu = itemMenu;
        if (nullptr != innerScene_)
        {
            auto sceneMenu = innerScene_->createContextMenu({0, 0});

            resultMenu = MenuBuilder::combineMenus(sceneMenu, itemMenu);
            itemMenu->deleteLater();
            sceneMenu->deleteLater();
        }

        resultMenu->setFixedWidth(300);

        MenuBuilder::setMenuEffect(resultMenu);

        addMenuToolBar(resultMenu);

        resultMenu->exec(QCursor::pos());
        resultMenu->deleteLater();
    }
}

void UniversalGraphicsItem::setTitleViewMode(TitleItem::ViewMode viewMode)
{
    if (titleItem_)
        titleItem_->setViewMode(viewMode);

    // Do it for inner items
    auto items = innerScene_->graphicsItems();
    for (auto it : items)
    {
        auto item = dynamic_cast<UniversalGraphicsItem*>(it.second);
        if (item)
            item->setTitleViewMode(viewMode);
    }
}

void UniversalGraphicsItem::setFont(const QFont &font)
{
    if (!impl_ || !titleItem_)
        return;

    impl_->setFont(font);
    titleItem_->setFont(font);

    updateGeometry();
    emit itemPropertiesChanged();
}

void UniversalGraphicsItem::setFontFamily(const QString &family)
{
    if (!impl_ || !titleItem_)
        return;

    impl_->setFontFamily(family);
    titleItem_->setFont(impl_->getFont());

    updateGeometry();
    emit itemPropertiesChanged();
}

void UniversalGraphicsItem::setFontSize(int size)
{
    if (!impl_ || !titleItem_)
        return;

    impl_->setFontSize(size);
    titleItem_->setFont(impl_->getFont());

    updateGeometry();
    emit itemPropertiesChanged();
}

void UniversalGraphicsItem::setFontItalic(bool italic)
{
    if (!impl_ || !titleItem_)
        return;

    impl_->setItalic(italic);
    titleItem_->setFont(impl_->getFont());

    emit itemPropertiesChanged();
}

void UniversalGraphicsItem::setFontUnderline(bool underline)
{
    if (!impl_ || !titleItem_)
        return;

    impl_->setUnderline(underline);
    titleItem_->setFont(impl_->getFont());

    emit itemPropertiesChanged();
}

void UniversalGraphicsItem::setColor(const QColor &color)
{
    if (!impl_ || !titleItem_)
        return;

    impl_->setColor(color);
    color_ = color;
    titleItem_->setColor(color);

    emit itemPropertiesChanged();
}

void UniversalGraphicsItem::setBackColor(const QColor &color)
{
    if (!impl_ || !nodeWidget_)
        return;

    impl_->setBackColor(color);
    nodeWidget_->setBackColor(color);

    emit itemPropertiesChanged();
}

void UniversalGraphicsItem::setBorderColor(const QColor &color)
{
    if (!impl_ || !portBar_)
        return;

    impl_->setBorderColor(color);
    portBar_->setColor(color);

    emit itemPropertiesChanged();
}

void UniversalGraphicsItem::setPicture(const QString &picture)
{
    if (!picture.isEmpty())
    {
        impl_->setPicture(picture);
        setViewMode(UniversalGraphicsItemImpl::Picture);
        nodeWidget_->setPicture(impl_->picture());
    }
    else
    {
        setViewMode(UniversalGraphicsItemImpl::Scene);
    }

    if (nodeWidget_)
    {
        nodeWidget_->setViewMode(impl_->viewMode());
        nodeWidget_->getFlowView()->showTopLevelConnections();
    }
    updateGeometry();

    if (resizeBox_)
        resizeBox_->setViewMode(impl_->viewMode());

    emit itemPropertiesChanged();
}

void UniversalGraphicsItem::setPropertyCommand(const Item::GraphicsItemProperties &property, const QVariant &value)
{
    switch (property)
    {
        case Item::GraphicsItemProperties::Color:
        {
            if (value.canConvert<QColor>())
            {
                setColor(value.value<QColor>());
            }
            break;
        }
        case Item::GraphicsItemProperties::Picture:
        {
            if (value.canConvert<QString>())
            {
                setPicture(value.toString());
                updateWidget();
            }
            break;
        }
        case Item::GraphicsItemProperties::BackColor:
        {
            if (value.canConvert<QColor>())
            {
                setBackColor(value.value<QColor>());
            }
            break;
        }
        case Item::GraphicsItemProperties::Font:
        {
            if (value.canConvert<QString>())
            {
                QString fontStr = value.toString();
                QFont font;
                if (font.fromString(fontStr))
                {
                    setFont(font);
                }
            }
            break;
        }
        case Item::GraphicsItemProperties::BorderColor:
        {
            if (value.canConvert<QColor>())
            {
                setBorderColor(value.value<QColor>());
            }
            break;
        }
        case Item::GraphicsItemProperties::HideInnerLevels:
        {
            if (value.canConvert<bool>())
            {
                setHiddenInnerLevels(value.toBool());
            }
            break;
        }
        default:
        {
            ResizableItem::setPropertyCommand(property, value);
            if (property == QtNodes::Item::GraphicsItemProperties::Lock && nodeWidget_)
            {
                setLocked(value.toBool());
            }
        }
    }
}
