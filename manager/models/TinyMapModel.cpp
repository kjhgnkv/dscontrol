#include "TinyMapModel.hpp"

#include <QDebug>
#include <stdexcept>

namespace dscontrol
{
TinyMapModel::TinyMapModel(const QStringList &lastCommNames, const QStringList &allCommNames, QObject* parent)
: QAbstractListModel(parent)
{
    qDebug() << "TinyMapModel: constructor";
    for (const auto &commName : allCommNames)
        allCommNames_.insert(std::pair<QString, Qt::CheckState>(commName, Qt::Unchecked));

    for (const auto &lastCommName : lastCommNames)
    {
        try
        {
            allCommNames_.at(lastCommName) = Qt::Checked;
        }
        catch (const std::out_of_range &)
        {
        }
    }
}

int TinyMapModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return static_cast<int>(allCommNames_.size());
}

QVariant TinyMapModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal && section == 0)
    {
        return tr("Please, select the communication names from the list.");
    }
    return QVariant();
}

Qt::ItemFlags TinyMapModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    if (createdCommNames_.contains(index.data(Qt::DisplayRole).toString()))
    {
        return QAbstractListModel::flags(index) & (~Qt::ItemIsEnabled) & (~Qt::ItemIsUserCheckable);
    }

    return QAbstractListModel::flags(index) | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
}

QVariant TinyMapModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.column() >= 1 || static_cast<uint64_t>(index.row()) > allCommNames_.size())
        return QVariant();

    auto it = allCommNames_.begin();
    std::advance(it, index.row());
    switch (role)
    {
        case Qt::DisplayRole:
            return it->first;
        case Qt::CheckStateRole:
            return it->second;
        case Qt::TextAlignmentRole:
        {
            return Qt::AlignRight;
        }
        default:
            return QVariant();
    }
}

bool TinyMapModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.column() >= 1 || static_cast<uint64_t>(index.row()) > allCommNames_.size() || !index.flags().testFlag(
    Qt::ItemIsUserCheckable))
        return false;

        //    if (role == Qt::CheckStateRole)
        //    {
        //        auto it = allCommNames_.begin();
        //        std::advance(it, index.row());
        //        it->second = static_cast<Qt::CheckState>(value.toUInt());
        //    }
    else if (role == Qt::EditRole)
    {
        auto it = allCommNames_.begin();
        std::advance(it, index.row());
        it->second = static_cast<Qt::CheckState>(value.toUInt());
    }

    emit dataChanged(index, index, QVector<int>() << role);
    return true;
}

void TinyMapModel::setLastCommNames(const QStringList &lastCommNames)
{
    qDebug() << "TinyMapModel: set last comm names";
    for (auto &it : allCommNames_)
        it.second = Qt::Unchecked;

    for (const auto &lastCommName : lastCommNames)
    {
        try
        {
            allCommNames_.at(lastCommName) = Qt::Checked;
        }
        catch (const std::out_of_range &)
        {
        }
    }

    emit dataChanged(index(0, 0), index(rowCount() - 1, 0), QVector<int>() << Qt::CheckStateRole);
}

QStringList TinyMapModel::getLastCommNames() const
{
    QStringList ret;
    for (const auto &it : allCommNames_)
        if (it.second)
            ret << it.first;
    return ret;
}

void TinyMapModel::setCreatedCommNames(const QStringList &createdCommNames)
{
    qDebug() << "TinyMapModel: set created comm names";
    createdCommNames_ = createdCommNames;
    createdCommNames_.removeDuplicates();
    for (auto &it : allCommNames_)
    {
        if (createdCommNames_.contains(it.first))
        {
            it.second = Qt::Checked;
        }
    }

    emit dataChanged(index(0, 0), index(rowCount() - 1, 0), QVector<int>() << Qt::CheckStateRole);
}

QStringList TinyMapModel::getCreatedCommNames() const
{
    return createdCommNames_;
}

void TinyMapModel::setAllCommNames(const QStringList &allCommNames)
{
    beginResetModel();
    allCommNames_.clear();
    for (const auto &comm : allCommNames)
    {
        allCommNames_.insert({comm, Qt::Unchecked});
    }
    endResetModel();
}
} // namespace dscontrol
