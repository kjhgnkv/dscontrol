// DSProxyModel.cpp

#include "DSProxyModel.hpp"

#include <QDebug>

namespace dscontrol
{
DSProxyModel::DSProxyModel(QObject* parent)
: QSortFilterProxyModel {parent}
{
}

void DSProxyModel::setFilter(const Filter &filter, const int &column)
{
    beginResetModel();
    filter_ = filter;
    column_ = column;
    endResetModel();
}

QVariant DSProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return sourceModel()->headerData(section, orientation, role);
}

bool DSProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (filter_(sourceModel()->index(source_row, column_, source_parent)))
        return true;
    return false;
}
} // namespace dscontrol
