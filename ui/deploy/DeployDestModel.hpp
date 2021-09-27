#pragma once

#include <set>
#include <QAbstractTableModel>

#include "DestData.hpp"

namespace dscontrol
{
namespace models
{
class DeployDestModel : public QAbstractTableModel
{
public:
    enum Column
    {
        Host,
        Ip,
        ProductName,
        Architecture,
        Size
    };

    DeployDestModel(QObject* parent = nullptr);
    ~DeployDestModel() override;

    int columnCount(const QModelIndex &parent) const override;
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    void setupModelData(const std::vector<DestData> &data);

private:
    QVector<DestData> data_;
};
}
}
