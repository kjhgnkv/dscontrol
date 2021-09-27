// RecentProjectsModel.cpp

#include "RecentProjectsModel.hpp"

#include <QFileInfo>

namespace dscontrol
{
QVariant RecentProjectsModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
        case Qt::DisplayRole:
        {
            auto fullFileName = QStringListModel::data(index, Qt::DisplayRole).toString();
            // return just file name, which is the project name
            return fullFileName;
        }
        case Role::FullFileName:
            return QStringListModel::data(index, Qt::DisplayRole);
        default:
            return {};
    }
}

Qt::ItemFlags RecentProjectsModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemNeverHasChildren | Qt::ItemFlag::ItemIsEnabled;
}
} // namespace dscontrol
