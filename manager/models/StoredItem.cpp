#include "StoredItem.hpp"

StoredItem::StoredItem(StoredItem* parent, const QString &type, const QString &caption, const QJsonObject &data)
: parent_ {parent}
, data_ {data}
, type_ {type}
, caption_ {caption}
{
}

StoredItem::~StoredItem()
{
    for (auto i : children_)
    {
        delete i;
    }
}

void StoredItem::addChildren(const QString &type, const QString &caption, const QJsonObject &data)
{
    children_.push_back(new StoredItem {this, type, caption, data});
}

void StoredItem::removeChildren(int row)
{
    if (row >= 0 && children_.size() > static_cast<size_t>(row))
    {
        delete children_[row];
        children_.erase(children_.begin() + row);
    }
}

const QString &StoredItem::type() const
{
    return type_;
}

const QString &StoredItem::caption() const
{
    return caption_;
}

const QJsonObject &StoredItem::data() const
{
    return data_;
}

const std::vector<StoredItem*> &StoredItem::children() const
{
    return children_;
}

StoredItem* StoredItem::parent() const
{
    return parent_;
}
