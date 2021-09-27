#include "AbstractItemModel.hpp"

#include "AbstractNodeItem.hpp"
#include <QDataStream>
#include <QPair>
#include <QVariant>

AbstractItemModel::AbstractItemModel(AbstractNodeItem* root)
: rootItem_(root)
, stringTitle_({})
, itemFlags_(Qt::ItemIsEnabled | Qt::ItemIsSelectable)
{
}

AbstractItemModel::~AbstractItemModel()
{
    delete rootItem_;
}

int AbstractItemModel::rowCount(const QModelIndex &parent) const
{
    auto childItem = child(parent);
    return childItem->rowCount();
}

int AbstractItemModel::columnCount(const QModelIndex &parent) const
{
    return stringTitle_.empty() ? 1 : stringTitle_.size();
}

QVariant AbstractItemModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid())
    {
        auto childItem = child(index);
        if (childItem != nullptr)
        {
            return childItem->data(index, role);
        }
    }

    return QVariant();
}

bool AbstractItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool result = false;
    auto childItem = child(index);
    if (childItem != nullptr)
    {
        childItem->setData(index, value, role);
        emit dataChanged(index, index);
    }

    return result;
}

void AbstractItemModel::setQtFlag(Qt::ItemFlag flag)
{
    itemFlags_ = flag;
}

void AbstractItemModel::setQtFlags(Qt::ItemFlags flags)
{
    itemFlags_ = flags;
}

void AbstractItemModel::appendQtFlag(Qt::ItemFlag flag)
{
    itemFlags_.setFlag(flag, true);
}

Qt::ItemFlags AbstractItemModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    return itemFlags_;
}

QModelIndex AbstractItemModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        auto parentItem = child(parent);
        if (parentItem != nullptr)
        {
            auto childItem = parentItem->childRow(row);
            if (childItem != nullptr)
            {
                return createIndex(row, column, childItem);
            }

            return createIndex(row, column, parentItem);
        }
    }
    else
    {
        auto childItem = rootItem_->childRow(row);
        if (childItem != nullptr)
        {
            return createIndex(row, column, childItem);
        }
    }

    return QModelIndex();
}

QModelIndex AbstractItemModel::parent(const QModelIndex &index) const
{
    if (index.isValid())
    {
        auto childItem = child(index);
        if (childItem != nullptr)
        {
            auto parentItem = childItem->parent();
            if (parentItem != nullptr)
            {
                if (parentItem != rootItem_)
                {
                    return createIndex(parentItem->childNumber(), 0, parentItem);
                }
            }
        }
    }

    return QModelIndex();
}

QVariant AbstractItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        return stringTitle_.value(section);
    }

    return QVariant();
}

bool AbstractItemModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
    {
        return false;
    }

    stringTitle_.value(section) = value.toString();
    emit headerDataChanged(orientation, section, section);

    return true;
}

QModelIndex AbstractItemModel::insert(const QVariant &variant, const QModelIndex &parent)
{
    auto parentItem = child(parent);
    int row = parentItem->rowCount();

    if (insertRow(row, parentItem->getModelIndex()))
    {
        QModelIndex modelIndex = index(row, 0, parentItem->getModelIndex());
        if (setData(modelIndex, variant, Qt::EditRole))
        {
            parentItem->childRow(row)->setModelIndex(modelIndex);
            return modelIndex;
        }
    }

    return QModelIndex();
}

QModelIndex AbstractItemModel::insert(AbstractNodeItem* nodeItem, const QModelIndex &parent)
{
    auto parentItem = child(parent);
    int row = parentItem->rowCount();

    if (insertRow(nodeItem, row, parentItem->getModelIndex()))
    {
        QModelIndex modelIndex = index(row, 0, parentItem->getModelIndex());
        nodeItem->setModelIndex(modelIndex);
        return modelIndex;
    }

    return QModelIndex();
}

bool AbstractItemModel::update(const QModelIndex &index, const QVariant &variant, int role)
{
    if (!index.isValid())
    {
        return false;
    }

    return setData(index, variant, role);
}

bool AbstractItemModel::remove(const QModelIndex &parent)
{
    if (!parent.isValid())
    {
        return false;
    }

    return removeRow(parent.row(), parent);
}

bool AbstractItemModel::removeAll(const QModelIndex &modelIndex)
{
    auto childItem = child(modelIndex);

    if (childItem != nullptr)
    {
        if (!childItem->childs().empty())
        {
            // TODO ???
            // for (int i = childItem->childs().size(); i > 0; i--)
            for (int i = childItem->childs().size() - 1; i >= 0; i--)
            {
                // QModelIndex index = childItem->childs().at(i - 1)->getModelIndex();
                QModelIndex index = childItem->childs().at(i)->getModelIndex();
                removeAll(index);
                remove(index);
            }
        }
    }

    return true;
}

void AbstractItemModel::allUnchecked()
{
    for (const auto &parent : root()->childs())
    {
        for (const auto &child : parent->childs())
        {
            if (child->getCheckState() == Qt::CheckState::Checked)
            {
                child->setCheckState(Qt::CheckState::Unchecked);
                emit dataChanged(child->getModelIndex(), child->getModelIndex());
            }
        }
    }
}

bool AbstractItemModel::insertRows(AbstractNodeItem* nodeItem, int position, int rows, const QModelIndex &parent)
{
    auto parentItem = child(parent);
    beginInsertRows(parent, position, position + rows - 1);
    bool success = parentItem->insert(nodeItem);
    endInsertRows();
    return success;
}

bool AbstractItemModel::insertRow(AbstractNodeItem* nodeItem, int arow, const QModelIndex &aparent)
{
    return insertRows(nodeItem, arow, 1, aparent);
}

inline bool AbstractItemModel::insertRow(int arow, const QModelIndex &aparent)
{
    return insertRows(arow, 1, aparent);
}

bool AbstractItemModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    auto parentItem = child(parent);
    beginInsertRows(parent, position, position + rows - 1);
    bool success = parentItem->insert();
    endInsertRows();
    return success;
}

bool AbstractItemModel::removeRows(int position, int rows, const QModelIndex &index)
{
    auto parentItem = child(index)->parent();
    bool success = false;

    if (parentItem != nullptr)
    {
        beginRemoveRows(index.parent(), position, position + rows - 1);
        success = parentItem->remove(index);
        endRemoveRows();
    }

    return success;
}

bool AbstractItemModel::setCheckState(AbstractNodeItem* parent, Qt::CheckState state)
{
    switch (state)
    {
        case Qt::Unchecked:
        {
            parent->setCheckState(state);
            emit dataChanged(parent->getModelIndex(), parent->getModelIndex());

            for (auto &child : parent->childs())
            {
                setData(child->getModelIndex(), Qt::CheckState::Unchecked, Qt::CheckStateRole);
            }

            setData(parent->parent()->getModelIndex(), Qt::CheckState::PartiallyChecked, Qt::CheckStateRole);
            break;
        }
        case Qt::PartiallyChecked:
        {
            int checked_num = 0;
            int unchecked_num = 0;
            bool is_partially = false;
            Qt::CheckState child_state;
            int m_rowCount = parent->rowCount();

            for (auto &child : parent->childs())
            {
                child_state = static_cast<Qt::CheckState>(child->getCheckState());
                switch (child_state)
                {
                    case Qt::PartiallyChecked:
                    {
                        is_partially = true;
                        break;
                    }
                    case Qt::Unchecked:
                    {
                        unchecked_num++;
                        break;
                    }
                    case Qt::Checked:
                    {
                        checked_num++;
                        break;
                    }
                    default:
                    {
                        checked_num++;
                        break;
                    }
                }
            }

            Qt::CheckState now_state;
            if (is_partially)
            {
                now_state = Qt::PartiallyChecked;
            }
            else if (checked_num == m_rowCount)
            {
                now_state = Qt::Checked;
            }
            else if (unchecked_num == m_rowCount)
            {
                now_state = Qt::Unchecked;
            }
            else
            {
                now_state = Qt::PartiallyChecked;
            }
            if (static_cast<Qt::CheckState>(parent->getCheckState()) != now_state)
            {
                parent->setCheckState(now_state);
                emit dataChanged(parent->getModelIndex(), parent->getModelIndex());

                if (parent->parent())
                {
                    setData(parent->parent()->getModelIndex(), Qt::PartiallyChecked, Qt::CheckStateRole);
                }
            }
            break;
        }
        case Qt::Checked:
        {
            parent->setCheckState(state);
            emit dataChanged(parent->getModelIndex(), parent->getModelIndex());

            for (auto &child : parent->childs())
            {
                setData(child->getModelIndex(), Qt::Checked, Qt::CheckStateRole);
            }

            if (parent->parent())
            {
                setData(parent->parent()->getModelIndex(), Qt::CheckState::PartiallyChecked, Qt::CheckStateRole);
            }
            break;
        }
    }

    return true;
}

AbstractNodeItem* AbstractItemModel::child(const QModelIndex &index) const
{
    if (index.isValid())
    {
        auto item = static_cast<AbstractNodeItem*>(index.internalPointer());
        if (item != nullptr)
        {
            return item;
        }
    }

    return rootItem_;
}

void AbstractItemModel::appendTitle(const QStringList &listTitle)
{
    stringTitle_.append(listTitle);
}

void AbstractItemModel::appendTitle(const QString &title)
{
    stringTitle_.append(title);
    emit headerDataChanged(Qt::Orientation::Horizontal, stringTitle_.size(), stringTitle_.size());
}

void AbstractItemModel::editTitle(const QString &title, int index, const Qt::Orientation &orientation)
{
    setHeaderData(index, orientation, QVariant(title), Qt::EditRole);
}

void AbstractItemModel::removeTitle()
{
    stringTitle_.clear();
}

void AbstractItemModel::removeTitle(int position)
{
    stringTitle_.removeAt(position);
}

AbstractNodeItem* AbstractItemModel::root()
{
    return rootItem_;
}

bool AbstractItemModel::updateData(const QModelIndex &index)
{
    if (!index.isValid())
    {
        return false;
    }

    emit dataChanged(index, index);
    return true;
}

bool AbstractItemModel::insertColumn(int acolumn, const QModelIndex &aparent)
{
    return insertColumns(acolumn, 1, aparent);
}

bool AbstractItemModel::removeRow(int arow, const QModelIndex &aparent)
{
    return removeRows(arow, 1, aparent);
}

bool AbstractItemModel::removeColumn(int acolumn, const QModelIndex &aparent)
{
    return removeColumns(acolumn, 1, aparent);
}

bool AbstractItemModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    // beginInsertColumns();
    // ...
    // endInsertColumns();

    return true;
}

bool AbstractItemModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    // beginRemoveColumns();
    // ...
    // endRemoveColumns();

    return true;
}

bool AbstractItemModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count,
                                 const QModelIndex &destinationParent, int destinationChild)
{
    // beginMoveRows
    // ...
    // endMoveRows

    return true;
}

bool AbstractItemModel::moveColumns(const QModelIndex &sourceParent, int sourceColumn, int count,
                                    const QModelIndex &destinationParent, int destinationChild)
{
    // beginMoveColumns
    // ...
    // endMoveColumns

    return true;
}