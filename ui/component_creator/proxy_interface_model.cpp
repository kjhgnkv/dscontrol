#include "proxy_interface_model.hpp"

bool proxy_interface_model::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (!source_parent.isValid())
    {
        return ::QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
    }
    return true;
}
