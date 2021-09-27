#include "DeployTargetModel.hpp"
#include "DeployTargetItem.hpp"

#include "common/nodeeditor_globals.hpp"

using namespace dscontrol::models;

DeployTargetModel::DeployTargetModel(QObject* parent)
{
    root_ = new DeployTargetItem {"asd", "", false};
}

DeployTargetModel::~DeployTargetModel()
{
    delete root_;
}

void DeployTargetModel::setupModelData(const std::vector<std::tuple<int, QString, QString>> &data)
{
    root_->clear();
    QVector<QPair<QtNodes::DSLevels, DeployTargetItem*>> parents;

    for (const auto&[level, name, ip] :  data)
    {
        auto type = static_cast<QtNodes::DSLevels>(level);

        while (!parents.isEmpty() && parents.last().first >= type)
        {
            parents.pop_back();
        }

        switch (type)
        {
            case QtNodes::DSLevels::System:
            {
                auto item = new DeployTargetItem {name, ip, false, root_};
                parents.clear();
                parents.push_back({type, item});
                break;
            }
            case QtNodes::DSLevels::SubSystem:
            {
                auto parent = parents.last().second;
                auto item = new DeployTargetItem {name, ip, false, parent};
                parents.push_back({type, item});
                break;
            }
            case QtNodes::DSLevels::Computer:
            {
                auto parent = parents.last().second;
                auto item = new DeployTargetItem {name, ip, false, parent};
                parents.push_back({type, item});
                break;
            }
            case QtNodes::DSLevels::Application:
            {
                auto parent = parents.last().second;
                auto item = new DeployTargetItem {name, ip, false, parent};
                parents.push_back({type, item});
                break;
            }
            case QtNodes::DSLevels::Component:
            {
                auto parent = parents.last().second;
                auto item = new DeployTargetItem {name, ip, true, parent};
                parents.push_back({type, item});
                break;
            }
            default:
            {
                break;
            }
        }
    }
}

QModelIndex DeployTargetModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
    {
        return {};
    }

    DeployTargetItem* item = parentItem(parent);
    if (!item)
    {
        return {};
    }

    DeployTargetItem* child = item->child(row);
    if (child)
    {
        return createIndex(row, column, child);
    }

    return {};
}

QModelIndex DeployTargetModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
    {
        return {};
    }

    auto childItem = static_cast<DeployTargetItem*>(child.internalPointer());
    if (!childItem)
    {
        return {};
    }

    auto parentItem = childItem->parent();
    if (!parentItem || parentItem == root_)
    {
        return {};
    }

    auto upperParentItem = parentItem->parent();
    if (!upperParentItem)
    {
        return {};
    }

    return createIndex(upperParentItem->childIndex(parentItem), child.column(), parentItem);
}

int DeployTargetModel::rowCount(const QModelIndex &parent) const
{

    DeployTargetItem* item = parentItem(parent);

    if (!item)
    {
        return 0;
    }

    return item->children().count();
}

int DeployTargetModel::columnCount(const QModelIndex &parent) const
{
    return Column::Size;
}

QVariant DeployTargetModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return {};
    }

    auto column = index.column();
    auto item = static_cast<DeployTargetItem*>(index.internalPointer());

    if (!item)
    {
        return {};
    }

    switch (role)
    {
        case Qt::DisplayRole:
        {
            switch (column)
            {
                case Column::Name:
                {
                    return item->name();
                }
                case Column::Ip:
                {
                    return item->ip();
                }
                default:
                {
                    return {};
                }
            }
        }
        case Qt::CheckStateRole:
        {
            if (column == Column::Name && item->isIpVisible())
            {
                return item->isChecked();
            }
            break;
        }
        default:
        {
            return {};
        }
    }

    return {};
}

bool dscontrol::models::DeployTargetModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
    {
        return false;
    }

    auto item = static_cast<DeployTargetItem*>(index.internalPointer());
    if (!item)
    {
        return false;
    }

    switch (role)
    {
        case Qt::EditRole:
        {
            if (index.column() == Column::Ip && item->isIpVisible())
            {
                item->setIp(value.toString());
            }
            break;
        }
        case Qt::CheckStateRole:
        {
            if (item->isIpVisible())
            {
                item->setChecked(static_cast<Qt::CheckState>(value.toInt()));
            }
            break;
        }
        default:
        {
            break;
        }
    }

    emit dataChanged(index, index, {role});
    return true;
}

DeployTargetItem* DeployTargetModel::parentItem(const QModelIndex &index) const
{
    DeployTargetItem* parentItem = nullptr;

    if (!index.isValid())
    {
        parentItem = root_;
    }
    else
    {
        parentItem = static_cast<DeployTargetItem*>(index.internalPointer());
    }

    return parentItem;
}

QVariant DeployTargetModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
    {
        return {};
    }

    switch (section)
    {
        case Column::Name:
        {
            return "Name";
        }
        case Column::Ip:
        {
            return "Ip";
        }
        default:
        {
            return {};
        }
    }
}

Qt::ItemFlags dscontrol::models::DeployTargetModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags retval = QAbstractItemModel::flags(index);

    if (!index.isValid())
    {
        return retval;
    }

    auto item = static_cast<DeployTargetItem*>(index.internalPointer());

    if (!item)
    {
        return retval;
    }

    if (item->isIpVisible())
    {
        retval |= (Qt::ItemFlag::ItemIsUserCheckable | Qt::ItemFlag::ItemIsEditable);
    }

    return retval;
}

// TODO Deploy
// формирует xml для отправки на сервер
std::map<QString, QString> DeployTargetModel::selectedApplications() const
{
    std::map<QString, QString> retval {};
    auto indexes = match(index(0, 0), Qt::CheckStateRole, Qt::CheckState::Checked, -1, Qt::MatchRecursive);

    for (const auto &index : indexes)
    {
        auto parentIdx = parent(index);
        retval.insert({
                      data(parentIdx, Qt::DisplayRole).toString(), data(this->index(index.row(), Column::Ip, parentIdx),
                                                                        Qt::DisplayRole).toString()
                      });
    }

    return retval;
}

