#pragma once

#include <QSortFilterProxyModel>

class proxy_interface_model : public ::QSortFilterProxyModel
{
public:
    using QSortFilterProxyModel::QSortFilterProxyModel;
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
};
