#include "AllComponentsModel.h"

AllComponentsModel::AllComponentsModel(QObject *parent)
    : QAbstractTableModel{parent}
{

}

int AllComponentsModel::rowCount(const QModelIndex &index) const
{
    return 2;
}

int AllComponentsModel::columnCount(const QModelIndex &index) const
{
    return 3;
}

QVariant AllComponentsModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
        case Qt::DisplayRole:
        {
            return QString("Row%1, Column%2").arg(index.row() + 1).arg(index.column() + 1);
        }
    }
    return QVariant();
}

QVariant AllComponentsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        switch (section)
        {
            case 0:
                return QString("#");
            case 1:
                return QString("Name");
            case 2:
                return QString("Path");
        }
    }
    return {};
}

bool AllComponentsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole)
    {
        if (!checkIndex(index))
        {
            return false;
        }
        //gridData_[index.row()][index.column()] = value.toString();

        return true;
    }
    return false;
}
