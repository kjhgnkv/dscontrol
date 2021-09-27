#include "AbstractNodeItemMonitor.hpp"

#include <algorithm>

AbstractNodeItemMonitor::AbstractNodeItemMonitor(AbstractNodeItemMonitor* parent)
: parent_(parent)
, childs_({})
, checkState_ {0}
, value_({})
, modelIndex_(QModelIndex())
{
}

const QModelIndex &AbstractNodeItemMonitor::modelIndex() const
{
    return modelIndex_;
}

AbstractNodeItemMonitor* AbstractNodeItemMonitor::childRow(int row)
{
    if (!childs_.empty() && row < int(childs_.size()))
    {
        return childs_.at(row).second;
    }

    return nullptr;
}

int AbstractNodeItemMonitor::rowCount() const
{
    return (int)childs_.size();
}

int AbstractNodeItemMonitor::childNumber() const
{
    if (parent() != nullptr)
    {
        auto it = std::find_if(parent()->childs_.begin(), parent()->childs_.end(),
                               [&](const std::pair<QString, AbstractNodeItemMonitor*> &root)
                               {
                                   return root.second->modelIndex().internalPointer() == modelIndex().internalPointer();
                               });

        if (it != parent()->childs_.end())
        {
            return (int)std::distance(parent()->childs_.begin(), it);
        }
    }

    return 0;
}

AbstractNodeItemMonitor* AbstractNodeItemMonitor::parent() const
{
    return parent_ != nullptr ? parent_ : nullptr;
}

void AbstractNodeItemMonitor::parent(AbstractNodeItemMonitor* parent)
{
    parent_ = parent;
}

bool AbstractNodeItemMonitor::insert(const std::pair<QString, AbstractNodeItemMonitor*> &item)
{
    item.second->parent(this);
    childs_.emplace_back(item);

    return true;
}

void AbstractNodeItemMonitor::setModelIndex(const QModelIndex &index)
{
    modelIndex_ = index;
}

bool AbstractNodeItemMonitor::removeAll(std::vector<std::pair<QString, AbstractNodeItemMonitor*>> &parent)
{
    for (auto &child : parent)
    {
        removeAll(child.second->childs_);
    }

    parent.clear();

    return true;
}

bool AbstractNodeItemMonitor::remove(const QModelIndex &index)
{
    if (childs_.empty())
    {
        return false;
    }

    auto it = std::find_if(childs_.begin(), childs_.end(),
                           [&index](const std::pair<QString, AbstractNodeItemMonitor*> &item)
                           {
                               return item.second->modelIndex().internalPointer() == index.internalPointer();
                           });

    if (it == childs_.end())
    {
        return false;
    }

    removeAll(it->second->childs_);
    childs_.erase(it);
    return true;
}

int AbstractNodeItemMonitor::getCheckState() const
{
    return checkState_;
}

QVariant AbstractNodeItemMonitor::data(const QModelIndex &index, int role)
{
    if (isItemDataRole(index.column(), role))
    {
        if (role == Qt::DisplayRole)
        {
            return QVariant(value_.value(index.column()));
        }
    }
    else if (role == Qt::DisplayRole)
    {
        return QVariant(value_.value(index.column()));
    }

    return QVariant();
}

bool AbstractNodeItemMonitor::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole)
    {
        value_ = value.toStringList();
        return true;
    }

    return false;
}

const QStringList &AbstractNodeItemMonitor::getValue() const
{
    return value_;
}

void AbstractNodeItemMonitor::setCheckState(int checkState)
{
    checkState_ = checkState;
}

bool AbstractNodeItemMonitor::appendItemDataRole(int index, const Qt::ItemDataRole &itemRole)
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

bool AbstractNodeItemMonitor::findItemDataRole(int index, const Qt::ItemDataRole &findItemRole)
{
    auto it = std::find_if(itemRoles_.begin(), itemRoles_.end(), [&](const std::pair<int, Qt::ItemDataRole> &itemRole)
    {
        return findItemRole == itemRole.second;
    });

    return it != itemRoles_.end();
}

bool AbstractNodeItemMonitor::readItemDataRole(int index, const Qt::ItemDataRole &readItemRole)
{
    auto it = itemRoles_.find(index);
    if (it != itemRoles_.end())
    {
        it->second = readItemRole;
        return true;
    }

    return false;
}

bool AbstractNodeItemMonitor::removeItemDataRole(int index)
{
    auto it = itemRoles_.find(index);
    if (it != itemRoles_.end())
    {
        itemRoles_.erase(it);
        return true;
    }

    return false;
}

bool AbstractNodeItemMonitor::isItemDataRole(int index, int isItemRole) const
{
    auto it = itemRoles_.find(index);
    if (it != itemRoles_.end())
    {
        return it->second != isItemRole;
    }

    return false;
}

const std::vector<std::pair<QString, AbstractNodeItemMonitor*>> &AbstractNodeItemMonitor::childs() const
{
    return childs_;
}
