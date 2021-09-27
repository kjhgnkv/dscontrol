#include "AbstractNodeItem.hpp"

#include <algorithm>

AbstractNodeItem::AbstractNodeItem(AbstractNodeItem* parent)
: parent_(parent)
, childs_({})
, checkState_ {0}
, value_({})
, modelIndex_(QModelIndex())
{
}

const QModelIndex &AbstractNodeItem::getModelIndex() const
{
    return modelIndex_;
}

AbstractNodeItem* AbstractNodeItem::childRow(int row)
{
    if (!childs_.empty() && row < childs_.size())
    {
        return childs_.at(row);
    }

    return nullptr;
}

int AbstractNodeItem::rowCount() const
{
    return (int)childs_.size();
}

int AbstractNodeItem::childNumber() const
{
    if (parent_ != nullptr)
    {
        auto it = std::find_if(parent_->childs_.begin(), parent_->childs_.end(), [&](const AbstractNodeItem* root)
        {
            return root->getModelIndex().internalPointer() == getModelIndex().internalPointer();
        });

        if (it != parent_->childs_.end())
        {
            return (int)std::distance(parent_->childs_.begin(), it);
        }
    }

    return 0;
}

AbstractNodeItem* AbstractNodeItem::parent() const
{
    return parent_ != nullptr ? parent_ : nullptr;
}

bool AbstractNodeItem::insert()
{
    AbstractNodeItem* node = new AbstractNodeItem(this);
    childs_.emplace_back(node);

    return true;
}

bool AbstractNodeItem::insert(AbstractNodeItem* item)
{
    item->parent(this);
    childs_.emplace_back(item);

    return true;
}

void AbstractNodeItem::setModelIndex(const QModelIndex &index)
{
    modelIndex_ = index;
}

bool AbstractNodeItem::removeAll(std::vector<AbstractNodeItem*> &parent)
{
    for (auto &child : parent)
    {
        removeAll(child->childs_);
    }

    parent.clear();

    return true;
}

bool AbstractNodeItem::remove(const QModelIndex &index)
{
    if (childs_.empty())
    {
        return false;
    }

    auto it = std::find_if(childs_.begin(), childs_.end(), [&index](const AbstractNodeItem* item)
    {
        return item->getModelIndex().internalPointer() == index.internalPointer();
    });

    if (it == childs_.end())
    {
        return false;
    }

    removeAll((*it)->childs_);
    childs_.erase(it);
    return true;
}

const std::vector<AbstractNodeItem*> &AbstractNodeItem::childs() const
{
    return childs_;
}

int AbstractNodeItem::getCheckState() const
{
    return checkState_;
}

QVariant AbstractNodeItem::data(const QModelIndex &index, int role)
{
    if (itemRoles_.find(index.column()) != itemRoles_.end() && role != Qt::DisplayRole)
    {
        return role;
    }
    else if (role == Qt::DisplayRole)
    {
        return QVariant(value_.value(index.column()));
    }

    return QVariant();
}

bool AbstractNodeItem::setData(const QModelIndex &index, const QVariant &value, int role)
{
    switch (role)
    {
        case Qt::EditRole:
        {
            value_ = value.toStringList();
            break;
        }

        default:
        {
            return false;
        }
    }

    return true;
}

const QStringList &AbstractNodeItem::getValue() const
{
    return value_;
}

void AbstractNodeItem::setCheckState(int checkState)
{
    checkState_ = checkState;
}

bool AbstractNodeItem::appendItemDataRole(int index, const Qt::ItemDataRole &itemRole)
{
    auto it = itemRoles_.find(index);
    if (it != itemRoles_.end())
    {
        return false;
    }
    else
    {
        itemRoles_.insert(std::make_pair(index, itemRole));
    }

    return true;
}

bool AbstractNodeItem::findItemDataRole(int index, const Qt::ItemDataRole &findItemRole)
{
    auto it = std::find_if(itemRoles_.begin(), itemRoles_.end(), [&](const std::pair<int, Qt::ItemDataRole> &itemRole)
    {
        return findItemRole == itemRole.second;
    });

    return it != itemRoles_.end();
}

bool AbstractNodeItem::readItemDataRole(int index, const Qt::ItemDataRole &readItemRole)
{
    auto it = itemRoles_.find(index);
    if (it != itemRoles_.end())
    {
        it->second = readItemRole;
        return true;
    }

    return false;
}

bool AbstractNodeItem::removeItemDataRole(int index)
{
    auto it = itemRoles_.find(index);
    if (it != itemRoles_.end())
    {
        itemRoles_.erase(it);
        return true;
    }

    return false;
}

bool AbstractNodeItem::isItemDataRole(int index, int isItemRole) const
{
    auto it = itemRoles_.find(index);
    if (it != itemRoles_.end())
    {
        return it->second != isItemRole;
    }

    return false;
}

void AbstractNodeItem::parent(AbstractNodeItem* parent)
{
    parent_ = parent;
}
