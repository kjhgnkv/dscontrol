#include "PropertiesProxyModel.hpp"

#include "PropertiesTreeItem.hpp"

namespace dscontrol
{
PropertiesProxyModel::PropertiesProxyModel(QObject* parent)
: QSortFilterProxyModel {parent}
{
}

QVariant PropertiesProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return sourceModel()->headerData(section, orientation, role);
}

bool PropertiesProxyModel::containsStr(const QModelIndex &sourceIndex) const
{
    auto text = sourceModel()->data(sourceIndex).toString();
    return text.contains(filterStr_, Qt::CaseInsensitive);
}

void PropertiesProxyModel::setFilterStr(const QString &filter)
{
    filterStr_ = filter;
    invalidateFilter();
}

bool PropertiesProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    auto index = sourceModel()->index(source_row, 0, source_parent);
    auto item = static_cast<PropertiesTreeItem*>(index.internalPointer());

    if (item->type() == PropertiesTreeItem::GroupItem)
    {
        for (int i = 0; i < item->childCount(); ++i)
        {
            auto child = item->child(i);
            if (child->data(0).toString().contains(filterStr_, Qt::CaseInsensitive))
            {
                return true;
            }
        }
        return false;
    }
    return containsStr(index);
}
} // namespace dscontrol
