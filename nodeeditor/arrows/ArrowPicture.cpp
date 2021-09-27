#include "ArrowPicture.h"
#include "SpecialTextItem.h"

#include <QApplication>
#include <QSvgRenderer>
#include <QGraphicsSceneMouseEvent>

using namespace QtNodes::Arrows;

const QRectF &defaultBoundingRect {0, 0, 100, 100};

ConnectionArrow::ArrowPicture::ArrowPicture(const QString &fileName, QGraphicsItem* parent, const QUuid &id)
: ArrowPicture(fileName, {}, parent, id)
{
}

ConnectionArrow::ArrowPicture::ArrowPicture(const QString &fileName, const QString &title, QGraphicsItem* parent,
                                            const QUuid &id)
: QGraphicsSvgItem {fileName, parent}
, SerializableIdentityUnit {id}
, filename_ {fileName}
, title_ {new SpecialTextItem {title, this}}
, rect_ {defaultBoundingRect}
{
    setAcceptHoverEvents(true);
}

QString ConnectionArrow::ArrowPicture::fileName() const
{
    return filename_;
}

QString ConnectionArrow::ArrowPicture::caption() const
{
    if (nullptr != title_)
    {
        return title_->toPlainText();
    }
    return {};
}

void ConnectionArrow::ArrowPicture::setCaption(const QString &newCaption)
{
    if (nullptr != title_)
    {
        title_->setPlainText(newCaption);
    }
    else
    {
        title_ = new SpecialTextItem(newCaption, this);
    }
}

void ConnectionArrow::ArrowPicture::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    QApplication::setOverrideCursor(Qt::ForbiddenCursor);
    QGraphicsSvgItem::hoverEnterEvent(event);
}

void ConnectionArrow::ArrowPicture::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    QApplication::restoreOverrideCursor();
    QGraphicsSvgItem::hoverLeaveEvent(event);
}

QRectF ConnectionArrow::ArrowPicture::boundingRect() const
{
    return rect_;
}

void ConnectionArrow::ArrowPicture::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget)
    this->renderer()->render(painter, rect_);
}

QJsonObject ConnectionArrow::ArrowPicture::save() const
{
    QJsonObject ret = SerializableIdentityUnit::save();
    if (nullptr != title_)
    {
        ret["Title"] = title_->save();
    }
    return ret;
}

void ConnectionArrow::ArrowPicture::restore(const QJsonObject &restoringObject)
{
    if (nullptr != title_)
    {
        title_->restore(restoringObject["Title"].toObject());
    }
}

void ConnectionArrow::ArrowPicture::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    event->accept();
}
