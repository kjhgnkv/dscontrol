#include "SpecialTextItem.h"

#include <QApplication>
#include <QGraphicsSceneMouseEvent>
#include <QPen>

constexpr qreal defaultLineWidth {3};
const QColor &defaultLineColor {"#AAAAC5"};
const QColor &defaultTextColor {"#FFFFFF"};

using namespace QtNodes;

SpecialTextItem::SpecialTextItem(const QString &text, QGraphicsItem* parent, const QUuid &id)
: QGraphicsTextItem {text, parent}
, SerializableIdentityUnit {id}
, line_ {new QGraphicsLineItem {this}}
{
    setFlags(flags() | QGraphicsItem::ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);
    setDefaultTextColor(::defaultTextColor);
    const QPen &linePen {{defaultLineColor}, defaultLineWidth, Qt::DotLine};
    line_->setPen(linePen);
    line_->setVisible(false);
}

QJsonObject SpecialTextItem::save() const
{
    QJsonObject retval = SerializableIdentityUnit::save();
    const QJsonObject &posObj {{  "x", pos().x()}
                               , {"y", pos().y()}
    };
    retval["pos"] = posObj;
    return retval;
}

void SpecialTextItem::restore(const QJsonObject &restoringObject)
{
    SerializableIdentityUnit::restore(restoringObject);
    const QJsonObject &posObj = restoringObject["pos"].toObject();
    const QPointF &restoringPos {posObj["x"].toDouble(), posObj["y"].toDouble()};
    setPos(restoringPos);
}

void SpecialTextItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (nullptr != line_ && nullptr != event && Qt::LeftButton == event->button())
    {
        line_->setLine(QLineF(mapFromParent(parentItem()->boundingRect().center()), boundingRect().center()));
        QApplication::setOverrideCursor(Qt::ClosedHandCursor);
    }
    QGraphicsTextItem::mousePressEvent(event);
}

void SpecialTextItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (nullptr == event || nullptr == line_)
    {
        return;
    }
    if (Qt::LeftButton != event->button())
    {
        return;
    }
    const auto* const parent = parentItem();
    if (parent)
    {
        line_->setVisible(true);
        const QLineF &newLine {mapFromParent(parent->boundingRect().center()), boundingRect().center()};
        line_->setLine(newLine);
        QApplication::restoreOverrideCursor();
    }
    QGraphicsTextItem::mouseReleaseEvent(event);
}

void SpecialTextItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    QApplication::setOverrideCursor(Qt::OpenHandCursor);
    QGraphicsTextItem::hoverEnterEvent(event);
}

void SpecialTextItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    QApplication::restoreOverrideCursor();
    QGraphicsTextItem::hoverEnterEvent(event);
}

QVariant SpecialTextItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (nullptr == line_)
    {
        return QGraphicsTextItem::itemChange(change, value);
    }
    switch (change)
    {
        case QGraphicsItem::ItemPositionChange:
        {
            line_->setVisible(false);
            break;
        }
        case QGraphicsItem::ItemSelectedHasChanged:
        {
            line_->setVisible(value.toBool());
            break;
        }
        default:
        {
            break;
        }
    }
    return QGraphicsTextItem::itemChange(change, value);
}
