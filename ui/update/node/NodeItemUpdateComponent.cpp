#include "NodeItemUpdateComponent.hpp"

#include <QDebug>

NodeItemUpdateComponent::NodeItemUpdateComponent()
{
}

NodeItemUpdateComponent::NodeItemUpdateComponent(const QString &caption, const QString &type, int level)
{
    value_.clear();
    value_ << caption << type;
    level_ = level;
    qDebug() << "level_" << level_;
}

QVariant NodeItemUpdateComponent::data(const QModelIndex &index, int role)
{
    if (!index.isValid())
    {
        return QVariant();
    }

    switch (role)
    {
        case Qt::DisplayRole:
        {
            return QVariant(value_.value(index.column()));
        }

        case Qt::CheckStateRole:
        {
            if (index.column() == 0 && level_ == 1)
            {
                return checkState_;
            }
        }
    }

    return QVariant();
}

bool NodeItemUpdateComponent::setData(const QModelIndex &index, const QVariant &value, int role)
{
    switch (role)
    {
        case Qt::EditRole:
        {
            value_ = value.toStringList();
            level_ = value.toStringList().value(2).toInt();
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

int NodeItemUpdateComponent::getLevel() const
{
    return level_;
}
