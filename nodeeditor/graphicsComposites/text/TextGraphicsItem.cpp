#include "TextGraphicsItem.hpp"
#include "graphicsComposites/universalGraphicsObject/ResizableItem/ResizableItem.h"
#include "graphicsComposites/universalGraphicsObject/ResizableItem/ResizeBox.h"
#include "common/nodeeditor_globals.hpp"
#include "scenes/FlowScene.hpp"
#include "singletones/MenuBuilder.h"
#include "GraphicsTextItem.h"
#include "graphicsComposites/universalGraphicsObject/toolbar/ToolBarWidget.h"

#include <QDebug>
#include <QFont>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsTextItem>
#include <QMenu>
#include <QObject>
#include <QPainter>
#include <QRgb>
#include <QTextCursor>

namespace QtNodes
{
TextGraphicsItem::TextGraphicsItem(QUuid uid, QUuid parentId, QObject*)
: ResizableItem(uid)
, defaultFont_ {"Source Sans Pro", 14}
, font_ {defaultFont_}
, defaultColor_ {"#aaffffff"}
, color_ {defaultColor_}
, defaultBackColor_ {"transparent"}
, backColor_ {defaultBackColor_}
, defaultBorderColor_ {"#AAAAC5"}
, borderColor_ {defaultBorderColor_}
, defaultBorderWidth_ {2}
, borderWidth_ {defaultBorderWidth_}
, minSize_ {224, 36}
, placeholderFlag_ {false}
, parentId_ {parentId}
{
    setFlags(flags() | ItemSendsGeometryChanges);
    textItem_ = new GraphicsTextItem(this);
    textItemFilter_ = new TextItemFilter(this);

    type_ = static_cast<int>(Item::NodeType::Text);

    textItem_->installEventFilter(textItemFilter_);
    connect(textItemFilter_, &TextItemFilter::textUpdated, this, &TextGraphicsItem::updateGeometry);

    textItem_->setFocus();

    setSize(minSize_);
    if (resizeBox_)
    {
        resizeBox_->setMinSize(minSize_);
        resizeBox_->setType(Item::NodeType::Text);
    }
    resetFormat();

    if (resizeBox_)
        resizeBox_->setComponentResize(false);
}

TextGraphicsItem::~TextGraphicsItem()
{
    delete textItem_;
    delete textItemFilter_;
}

Item::NodeType TextGraphicsItem::typeN() const
{
    return Item::NodeType::Text;
}

QVariant TextGraphicsItem::property(const Item::GraphicsItemProperties &property) const
{
    switch (property)
    {
        case Item::GraphicsItemProperties::Color:
        {
            return getColor();
        }

        case Item::GraphicsItemProperties::BackColor:
        {
            return getBackColor();
        }
        case Item::GraphicsItemProperties::Font:
        {
            return getFont().family();
        }
        case Item::GraphicsItemProperties::BorderColor:
        {
            return getBorderColor();
        }
        case Item::GraphicsItemProperties::BorderWidth:
        {
            return getBorderWidth();
        }
        default:
        {
            return ResizableItem::property(property);
        }
    }
}

void TextGraphicsItem::setText(const QString &text)
{
    textItem_->setPlainText(text);

    checkPlaceholder();
}

QGraphicsTextItem* TextGraphicsItem::getTextItem()
{
    return textItem_;
}

QRectF TextGraphicsItem::getMaxRect()
{
    if (!textItem_ || !resizeBox_)
        return rect_;

    QRectF textRect = textItem_->boundingRect();
    QSizeF contentSize = resizeBox_->getContentSize();
    double width = std::max(contentSize.width(), textRect.width());
    double height = std::max(contentSize.height(), textRect.height());

    textRect.translate(-borderWidth_, -borderWidth_);
    textRect.setSize({width, height});

    return textRect;
}

void TextGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    QRectF rect = boundingRect();

    if (placeholderFlag_)
    {
        painter->setPen("#aaffffff");
        painter->drawText(rect, Qt::AlignCenter, tr("Enter Text"));
    }

    painter->setPen(QPen {borderColor_, static_cast<qreal>(borderWidth_)});
    painter->setBrush(backColor_);

    rect.translate(borderWidth_ / 2, borderWidth_ / 2);
    rect.setSize({rect.width() - borderWidth_, rect.height() - borderWidth_});

    painter->drawRoundedRect(rect, 8, 8);
}

void TextGraphicsItem::updateGeometry()
{
    if (resizeBox_)
    {
        resizeBox_->setContentSize(getMaxRect().size());
    }
    ResizableItem::updateGeometry();
}

void TextGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    if (!isLocked_)
    {
        focusIn();
    }
    UniversalSelectableItem::mouseDoubleClickEvent(event);
}

void TextGraphicsItem::focusIn()
{
    textItem_->setTextInteractionFlags(Qt::TextEditorInteraction);
    textItem_->setFlags(QGraphicsItem::ItemIsFocusable);
    textItem_->setFocus();
}

void TextGraphicsItem::focusOut()
{
    textItem_->setFlags({});
    QTextCursor cursor(textItem_->textCursor());
    cursor.clearSelection();
    textItem_->setTextCursor(cursor);
    textItem_->setTextInteractionFlags({});
    update();
}

void TextGraphicsItem::setLocked(bool flag)
{
    ResizableItem::setLocked(flag);

    if (isLocked_)
    {
        textItem_->setTextInteractionFlags(Qt::NoTextInteraction);
        textItem_->setFlag(QGraphicsItem::ItemIsFocusable, false);
    }
}

void TextGraphicsItem::setCurrentScale(double scale)
{
    if (scale <= 30.)
        textItem_->setVisible(false);
    else
        textItem_->setVisible(true);
    ResizableItem::setCurrentScale(scale);
}

void TextGraphicsItem::setParentItem(const QUuid &id)
{
    parentId_ = id;
}

QUuid TextGraphicsItem::getParentItem()
{
    return parentId_;
}

QJsonObject TextGraphicsItem::save() const
{
    QJsonObject retval = ResizableItem::save();

    retval["text"] = textItem_->toPlainText();

    retval["fontFamily"] = font_.family();
    retval["fontSize"] = font_.pointSize();
    retval["fontWeight"] = font_.weight();
    retval["italic"] = int(font_.italic());

    if (color_ == QColor {Qt::transparent})
    {
        retval["color"] = "transparent";
    }
    else
    {
        retval["color"] = color_.name();
    }

    if (backColor_ == QColor {Qt::transparent})
    {
        retval["backColor"] = "transparent";
    }
    else
    {
        retval["backColor"] = backColor_.name();
    }

    if (borderColor_ == QColor {Qt::transparent})
    {
        retval["borderColor"] = "transparent";
    }
    else
    {
        retval["borderColor"] = borderColor_.name();
    }

    retval["borderWidth"] = borderWidth_;
    retval["locked"] = isLocked_;
    retval["parent"] = parentId_.toString();

    return retval;
}

void TextGraphicsItem::restore(const QJsonObject &obj)
{
    ResizableItem::restore(obj);
    setText(obj["text"].toString());

    QFont font {
    obj["fontFamily"].toString(), obj["fontSize"].toInt(), obj["fontWeight"].toInt(), obj["italic"].toBool()
    };
    setFont(font);

    setColor(obj["color"].toString());
    setBackColor(QColor {obj["backColor"].toString()});
    setBorderColor(obj["borderColor"].toString());
    setBorderWidth(obj["borderWidth"].toInt());
    setLocked(obj["locked"].toBool());
}

bool TextItemFilter::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::FocusOut)
    {
        item_->focusOut();
        emit textUpdated();
        return true;
    }
    if (event->type() == QEvent::KeyRelease)
    {
        emit textUpdated();
        item_->checkPlaceholder();
    }
    return QObject::eventFilter(obj, event);
}

void TextGraphicsItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    setSelected(true);

    QMenu menu;

    addToolBar(&menu);

    auto lockUnlockAction = menu.addAction(QIcon(":/contextMenuIcons/Lock.png"), QAction::tr("Lock"));
    connect(lockUnlockAction, &QAction::triggered, [this]()
    {
        setLocked(!isLocked_);
    });
    if (isLocked())
    {
        lockUnlockAction->setIcon(QIcon(":/contextMenuIcons/Unlock.png"));
        lockUnlockAction->setText(QAction::tr("Unlock"));
    }
    menu.addAction(tr("Rename"));
    menu.addAction(QIcon(":/contextMenuIcons/delete.png"), tr("Delete"), [this]
    {
        if (auto scene = flowScene())
        {
            scene->deleteSelectedNodes();
        }
    });

    menu.exec(QCursor::pos());

    UniversalSelectableItem::contextMenuEvent(event);
}

void TextGraphicsItem::addToolBar(QMenu* menu)
{
    auto toolBarWidget = new ToolBarWidget(menu);

    toolBarWidget->setProperties(font_, color_, backColor_, borderColor_, borderWidth_);

    toolBarWidget->setMode(ToolBarWidget::Mode::Text);

    connect(toolBarWidget, &ToolBarWidget::itemPropertyChanged, this, &TextGraphicsItem::setProperty);

    connect(toolBarWidget, &ToolBarWidget::reseted, [this, toolBarWidget]
    {
        toolBarWidget->setProperties(defaultFont_, defaultColor_, defaultBackColor_, defaultBorderColor_,
                                     defaultBorderWidth_);
        this->resetFormat();
    });

    toolBarWidget->setGeometry({{10, 0}, toolBarWidget->size()});
    menu->setFixedWidth(toolBarWidget->width() + 20);
    menu->setContentsMargins(0, toolBarWidget->height() - 4, 0, 0);

    MenuBuilder::setMenuEffect(menu, true);
}

void TextGraphicsItem::setFont(const QFont &font)
{
    if (textItem_)
        textItem_->setFont(font);
    font_ = font;
}

void TextGraphicsItem::setColor(const QColor &color)
{
    color_ = color;
    textItem_->setDefaultTextColor(color);
}

void TextGraphicsItem::setBackColor(const QColor &color)
{
    backColor_ = color;
}

void TextGraphicsItem::setBorderColor(const QColor &color)
{
    borderColor_ = color;
}

void TextGraphicsItem::setBorderWidth(const int &width)
{
    borderWidth_ = width;
}

QFont TextGraphicsItem::getFont() const
{
    return font_;
}

QColor TextGraphicsItem::getColor() const
{
    return color_;
}

QColor TextGraphicsItem::getBackColor() const
{
    return backColor_;
}

QColor TextGraphicsItem::getBorderColor() const
{
    return borderColor_;
}

int TextGraphicsItem::getBorderWidth() const
{
    return borderWidth_;
}

void TextGraphicsItem::resetFormat()
{
    this->setFont(defaultFont_);
    this->setColor(defaultColor_);
    this->setBackColor(defaultBackColor_);
    this->setBorderColor(defaultBorderColor_);
    this->setBorderWidth(defaultBorderWidth_);
}

void TextGraphicsItem::setPropertyCommand(const Item::GraphicsItemProperties &property, const QVariant &value)
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
        case Item::GraphicsItemProperties::BackColor:
        {
            if (value.canConvert<QColor>())
            {
                setBackColor(value.value<QColor>());
            }
            break;
        }
        case Item::GraphicsItemProperties::BorderWidth:
        {
            if (value.canConvert<int>())
            {
                setBorderWidth(value.toInt());
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
        default:
        {
            ResizableItem::setPropertyCommand(property, value);
        }
    }
    update();
}

void TextGraphicsItem::checkPlaceholder()
{
}

QVariant TextGraphicsItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemSelectedHasChanged && !value.toBool())
    {
        focusOut();
        updateGeometry();
    }

    return ResizableItem::itemChange(change, value);
}

} // namespace QtNodes
