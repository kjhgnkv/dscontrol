#include "PropertiesTreeItem.hpp"

namespace dscontrol
{
PropertiesTreeItem::PropertiesTreeItem(const QVector<QVariant> &data, int type, PropertiesTreeItem* parent)
: type_ {type}
, itemData_ {data}
, parentItem_ {parent}
{
}

PropertiesTreeItem::~PropertiesTreeItem()
{
    qDeleteAll(childItems_);
}

int PropertiesTreeItem::type() const
{
    return type_;
}

void PropertiesTreeItem::setCaption(const QVector<QVariant> &data)
{
    itemData_ = data;
}

void PropertiesTreeItem::appendChild(PropertiesTreeItem* item)
{
    childItems_.append(item);
}

PropertiesTreeItem* PropertiesTreeItem::child(int row)
{
    return (row < 0 || row >= childItems_.size()) ? nullptr : childItems_.at(row);
}

int PropertiesTreeItem::childCount() const
{
    return childItems_.count();
}

int PropertiesTreeItem::columnCount() const
{
    return itemData_.count();
}

QVariant PropertiesTreeItem::data(int column) const
{
    return (column < 0 || column >= itemData_.size()) ? QVariant {} : itemData_.at(column);
}

PropertiesTreeItem* PropertiesTreeItem::parentItem()
{
    return parentItem_;
}

int PropertiesTreeItem::row() const
{
    return parentItem_ ? parentItem_->childItems_.indexOf(const_cast<PropertiesTreeItem*>(this)) : 0;
}
} // namespace dscontrol
