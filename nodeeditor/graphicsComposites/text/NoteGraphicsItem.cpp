#include "NoteGraphicsItem.hpp"
#include "graphicsComposites/universalGraphicsObject/DangerousProxyWidget.h"
#include "common/nodeeditor_globals.hpp"
#include "widgets/TextInputDialog.h"

#include <QApplication>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QLabel>
#include <QPainter>
#include <QSizePolicy>
#include <algorithm>

#define _NOTE_MARGIN 8

namespace QtNodes
{
NoteGraphicsItem::NoteGraphicsItem(const QUuid &id, const QUuid &parent, QSizeF size, const QString &text)
: UniversalConnectionableItem {id}
, widget_ {nullptr}
, parentId_ {parent}
, text_ {text}
, rect_ {{}, size}
, state_ {Default}
, moveFlag_ {false}
, locked_ {false}
{
    type_ = static_cast<int>(Item::NodeType::Note);
    borderColor_ = QColor {"#AAAAC5"};
    borderWidth_ = 1;
    setFlags(flags() | ItemSendsGeometryChanges | ItemIsMovable);
    setChangesFromBackToForeground(false);

    // Add widget in item
    proxyWidget_ = new DangerousProxyWidget(this);
    widget_ = new QLabel();
    widget_->setStyleSheet("QLabel{background: #605F6E;"
                           "font-size: 12px; color: #FFFFFF;}");
    widget_->setWordWrap(true);
    widget_->setSizePolicy({QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding});
    widget_->setAlignment(Qt::AlignCenter);

    proxyWidget_->setWidget(widget_);
    setText(text);
}

NoteGraphicsItem::~NoteGraphicsItem()
{
    qDebug() << "NoteGraphicsItem: destructor";
    if (widget_)
        delete widget_;
}

QUuid NoteGraphicsItem::getParentId()
{
    return parentId_;
}

void NoteGraphicsItem::setText(const QString &text)
{
    if (text == text_)
    {
        return;
    }

    text_ = text;

    if (nullptr == widget_)
    {
        return;
    }

    widget_->setText(text);
    widget_->adjustSize();
    setRect(widget_->rect());
}

QRectF NoteGraphicsItem::boundingRect() const
{
    QRectF boundingRect = rect_.translated(-_NOTE_MARGIN, -_NOTE_MARGIN);
    boundingRect.setSize({boundingRect.width() + 2 * _NOTE_MARGIN, boundingRect.height() + 2 * _NOTE_MARGIN});
    return boundingRect;
}

void NoteGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->setPen(QPen(QColor(borderColor_), borderWidth_));
    auto borderRect = boundingRect();
    painter->drawRoundedRect(borderRect, _NOTE_MARGIN, _NOTE_MARGIN);

    QPainterPath path;
    path.addRoundedRect(borderRect, _NOTE_MARGIN, _NOTE_MARGIN);
    painter->fillPath(path, QColor("#605F6E"));

    Q_UNUSED(option);
    Q_UNUSED(widget);
}

void NoteGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    qDebug() << "NoteGraphicsItem: mouse double clicked event";
    setText(TextInputDialog::getText(tr("Notes"), widget_->text()));

    Q_UNUSED(event);
}

QJsonObject NoteGraphicsItem::save() const
{
    qDebug() << "NoteGraphicsItem: save";
    QJsonObject retval = UniversalConnectionableItem::save();
    retval["parent"] = parentId_.toString();
    retval["text"] = text_;
    QJsonObject obj = {
    {"x"  , scenePos().x()}
    , {"y", scenePos().y()}
    };
    retval["pos"] = obj;
    return retval;
}

void NoteGraphicsItem::restore(const QJsonObject &p)
{
    qDebug() << "NoteGraphicsItem: restore";
    UniversalConnectionableItem::restore(p);

    QJsonObject tmp = p["pos"].toObject();
    setPos({tmp["x"].toDouble(), tmp["y"].toDouble()});
    setText(p["text"].toString());
    parentId_ = QUuid {p["parent"].toString()};
}

void NoteGraphicsItem::setRect(const QRectF &newRect)
{
    if (rect_ == newRect)
    {
        return;
    }
    prepareGeometryChange();
    rect_ = newRect;
    emit geometryChanged();
}

void NoteGraphicsItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    if (nullptr != event)
    {
        event->accept();
        UniversalConnectionableItem::contextMenuEvent(event);
    }
}
} // namespace QtNodes
