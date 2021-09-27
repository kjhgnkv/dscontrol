#include "DeployDestModel.hpp"
#include <QDebug>

using namespace dscontrol::models;

DeployDestModel::DeployDestModel(QObject* parent)
{

}

DeployDestModel::~DeployDestModel()
{

}

int DeployDestModel::columnCount(const QModelIndex &parent) const
{
    return Column::Size;
}

int dscontrol::models::DeployDestModel::rowCount(const QModelIndex &parent) const
{
    return data_.size();
}

QVariant DeployDestModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return {};
    }

    if (index.row() >= data_.size())
    {
        return {};
    }

    auto item = data_[index.row()];
    auto column = index.column();

    switch (role)
    {
        case Qt::DisplayRole:
        {
            switch (column)
            {
                case Column::Host:
                {
                    return item.host_;
                }
                case Column::Ip:
                {
                    return item.ip_;
                }
                case Column::ProductName:
                {
                    return item.productName_;
                }
                case Column::Architecture:
                {
                    return item.architecture_;
                }
                default:
                {
                    return {};
                }
            }
        }
    }

    return {};

}

QVariant DeployDestModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
    {
        return {};
    }

    switch (section)
    {
        case Column::Host:
        {
            return "Host";
        }
        case Column::Ip:
        {
            return "Ip";
        }
        case Column::ProductName:
        {
            return "ProductName";
        }
        case Column::Architecture:
        {
            return "Architecture";
        }
        default:
        {
            return {};
        }
    }
}

void DeployDestModel::setupModelData(const std::vector<DestData> &data)
{
    data_.clear();

    for (const auto &[host, ip, productName, architecture] : data)
    {
        data_.append(DestData {host, ip, productName, architecture});
    }

    emit layoutChanged();
}


