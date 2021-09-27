#include "SingleTinyMapModel.hpp"

#include "ui/platform/DSMessageBox/DSMessageBox.hpp"
#include <QDebug>

namespace dscontrol
{
SingleTinyMapModel::SingleTinyMapModel(const QString &lastCommName, const QStringList &allCommNames, QObject* parent)
: QAbstractListModel(parent)
, lastChecked_ {false}
, last_ {allCommNames_.end()}
{
    qDebug() << "SingleTinyMapModel: Constructor";
    for (const auto &commName : allCommNames)
    {
        auto state = Qt::Unchecked;
        if (commName == lastCommName)
        {
            lastChecked_ = true;
            state = Qt::Checked;
        }
        auto[it, ret] = allCommNames_.insert(std::pair<QString, Qt::CheckState>(commName, state));

        if (lastChecked_)
            last_ = it;
    }
}

int SingleTinyMapModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return static_cast<int>(allCommNames_.size());
}

QVariant SingleTinyMapModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal && section == 0)
    {
        return tr("Please, select the communication names from the list.");
    }
    return QVariant();
}

Qt::ItemFlags SingleTinyMapModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    if (createdCommNames_.contains(index.data(Qt::DisplayRole).toString()))
    {
        return QAbstractListModel::flags(index) & (~Qt::ItemIsEnabled) & (~Qt::ItemIsUserCheckable);
    }

    return QAbstractListModel::flags(index) | Qt::ItemIsUserCheckable;
}

QVariant SingleTinyMapModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.column() >= 1 || static_cast<uint64_t>(index.row()) > allCommNames_.size())
        return QVariant();

    auto it = allCommNames_.begin();
    std::advance(it, index.row());
    switch (role)
    {
        case Qt::CheckStateRole:
            return it->second;
        case Qt::DisplayRole:
            return it->first;
        case Qt::TextAlignmentRole:
        {
            return Qt::AlignRight;
        }
        default:
            return QVariant();
    }
}

bool SingleTinyMapModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || !index.flags().testFlag(
    Qt::ItemIsUserCheckable) || role != Qt::EditRole || index.column() >= 1 || static_cast<uint64_t>(index.row()) > allCommNames_.size())
        return false;

    auto val = static_cast<Qt::CheckState>(value.toUInt());

    if (val == Qt::Checked && lastChecked_)
    {
        DSMessageBox::critical(nullptr, tr("Error"), tr("You can select only one component!"));
        return false;
    }

    auto it = allCommNames_.begin();
    std::advance(it, index.row());
    it->second = val;

    if (val == Qt::Unchecked)
        lastChecked_ = false;
    else if (!lastChecked_ && val == Qt::Checked)
        lastChecked_ = true;

    emit dataChanged(index, index, QVector<int>() << role);
    return true;
}

void SingleTinyMapModel::setLastCommNames(const QString &lastCommName)
{
    qDebug() << "SingleTinyMapModel: set last comm names";
    for (auto &it : allCommNames_)
    {
        if (it.first == lastCommName)
        {
            lastChecked_ = true;
            it.second = Qt::Checked;
        }
        else
        {
            it.second = Qt::Unchecked;
        }
    }

    emit dataChanged(index(0, 0), index(rowCount() - 1, 0), QVector<int>() << Qt::CheckStateRole);
}

void SingleTinyMapModel::setCreatedCommNames(const QStringList &createdCommNames)
{
    qDebug() << "SingleTinyMapModel: set created comm names";
    createdCommNames_ = createdCommNames;
    for (auto &it : allCommNames_)
    {
        if (createdCommNames.contains(it.first))
        {
            it.second = Qt::Checked;
        }
    }

    emit dataChanged(index(0, 0), index(rowCount() - 1, 0), QVector<int>() << Qt::CheckStateRole);
}

QStringList SingleTinyMapModel::getLastCommNames() const
{
    QStringList ret;
    for (const auto &it : allCommNames_)
        if (it.second)
            ret << it.first;
    return ret;
}

void SingleTinyMapModel::setAllCommNames(const QStringList &allCommNames)
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
