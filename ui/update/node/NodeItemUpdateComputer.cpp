#include "NodeItemUpdateComputer.hpp"

#include "models/AbstractItemModel.hpp"
#include "NodeItemUpdateComponent.hpp"
#include <QDebug>

NodeItemUpdateComputer::NodeItemUpdateComputer()
    : itemModelComponent_ {}
{
}

NodeItemUpdateComputer::NodeItemUpdateComputer(const QString &value, int level)
{
    itemModelComponent_ = new AbstractItemModel(new NodeItemUpdateComponent());
    itemModelComponent_->appendQtFlag(Qt::ItemIsUserCheckable);

    value_.clear();
    value_ << value;
    level_ = level;
}

NodeItemUpdateComputer::~NodeItemUpdateComputer()
{
    if(itemModelComponent_)
    {
        delete itemModelComponent_;
    }
}

bool NodeItemUpdateComputer::setData(const QModelIndex &index, const QVariant &value, int role)
{
    switch (role)
    {
        case Qt::EditRole:
        {
            value_ = value.toStringList();
            level_ = value.toStringList().value(1).toInt();
            break;
        }

        case Qt::CheckStateRole:
        {
            checkState_ = value.toInt();
            break;
        }

        default:
        {
            return false;
        }
    }

    return true;
}

QVariant NodeItemUpdateComputer::data(const QModelIndex &index, int role)
{
    if (itemRoles_.find(index.column()) != itemRoles_.end() && role != Qt::DisplayRole)
    {
        if (role == Qt::CheckStateRole)
        {
            return checkState_;
        }
    }
    else if (role == Qt::DisplayRole)
    {
        return QVariant(value_.value(index.column()));
    }

    return QVariant();
}

int NodeItemUpdateComputer::getLevel() const
{
    return level_;
}

AbstractItemModel* NodeItemUpdateComputer::ItemModelComponent() const
{
    return itemModelComponent_;
}

const QModelIndex &NodeItemUpdateComputer::getSelectModelIndexXml() const
{
    return selectModelIndexXML_;
}

void NodeItemUpdateComputer::setSelectModelIndexXml(const QModelIndex &selectModelIndexXml)
{
    selectModelIndexXML_ = selectModelIndexXml;
}
