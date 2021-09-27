#include "AbstractItemModelMonitor.hpp"

#include "AbstractNodeItemMonitor.hpp"
#include <QDataStream>
#include <QPair>
#include <QVariant>

AbstractItemModelMonitor::AbstractItemModelMonitor(AbstractNodeItemMonitor* root)
: rootItem_(root)
, stringTitle_({})
, itemFlags_(Qt::ItemIsEnabled | Qt::ItemIsSelectable)
{
}

AbstractItemModelMonitor::~AbstractItemModelMonitor()
{
    delete rootItem_;
}

int AbstractItemModelMonitor::rowCount(const QModelIndex &parent) const
{
    AbstractNodeItemMonitor* childItem = child(parent);
    return childItem->rowCount();
}

int AbstractItemModelMonitor::columnCount(const QModelIndex &parent) const
{
    return stringTitle_.empty() ? 1 : stringTitle_.size();
}

QVariant AbstractItemModelMonitor::data(const QModelIndex &index, int role) const
{
    if (index.isValid())
    {
        AbstractNodeItemMonitor* childItem = child(index);
        if (childItem != nullptr)
        {
            return childItem->data(index, role);
        }
    }

    return QVariant();
}

bool AbstractItemModelMonitor::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid())
    {
        AbstractNodeItemMonitor* childItem = child(index);
        if (childItem != nullptr)
        {
            childItem->setData(index, value, role);
            emit dataChanged(index, index);
            emit layoutChanged();
        }

        return true;
    }

    return false;
}

void AbstractItemModelMonitor::setQtFlag(Qt::ItemFlag flag)
{
    itemFlags_ = flag;
}

void AbstractItemModelMonitor::setQtFlags(Qt::ItemFlags flags)
{
    itemFlags_ = flags;
}

void AbstractItemModelMonitor::appendQtFlag(Qt::ItemFlag flag)
{
    itemFlags_.setFlag(flag, true);
}

Qt::ItemFlags AbstractItemModelMonitor::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    return itemFlags_;
}

QModelIndex AbstractItemModelMonitor::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        AbstractNodeItemMonitor* parentItem = child(parent);
        if (parentItem != nullptr)
        {
            AbstractNodeItemMonitor* childItem = parentItem->childRow(row);
            if (childItem != nullptr)
            {
                return createIndex(row, column, childItem);
            }

            return createIndex(row, column, parentItem);
        }
    }
    else
    {
        AbstractNodeItemMonitor* childItem = rootItem_->childRow(row);
        if (childItem != nullptr)
        {
            return createIndex(row, column, childItem);
        }
    }

    return QModelIndex();
}

QModelIndex AbstractItemModelMonitor::parent(const QModelIndex &index) const
{
    if (index.isValid())
    {
        AbstractNodeItemMonitor* childItem = child(index);
        if (childItem != nullptr)
        {
            AbstractNodeItemMonitor* parentItem = childItem->parent();
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

QVariant AbstractItemModelMonitor::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        return stringTitle_.value(section);
    }

    return QVariant();
}

bool AbstractItemModelMonitor::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
    {
        return false;
    }

    stringTitle_[section] = value.toString();
    emit headerDataChanged(orientation, section, section);

    return true;
}

QModelIndex AbstractItemModelMonitor::insert(const QVariant &variant, const QModelIndex &parent)
{
    AbstractNodeItemMonitor* parentItem = child(parent);
    int row = parentItem->rowCount();

    if (insertRow(row, parentItem->modelIndex()))
    {
        QModelIndex modelIndex = index(row, 0, parentItem->modelIndex());
        if (setData(modelIndex, variant, Qt::EditRole))
        {
            parentItem->childRow(row)->setModelIndex(modelIndex);
            return modelIndex;
        }
    }

    return QModelIndex();
}

QModelIndex AbstractItemModelMonitor::insert(const std::pair<QString, AbstractNodeItemMonitor*> &nodeItem,
                                             const QModelIndex &parent)
{
    AbstractNodeItemMonitor* parentItem = child(parent);
    int row = parentItem->rowCount();

    if (insertRow(nodeItem, row, parentItem->modelIndex()))
    {
        QModelIndex modelIndex = index(row, 0, parentItem->modelIndex());
        nodeItem.second->setModelIndex(modelIndex);
        return modelIndex;
    }

    return QModelIndex();
}

bool AbstractItemModelMonitor::update(const QModelIndex &index, const QVariant &variant, int role)
{
    if (!index.isValid())
    {
        return false;
    }

    return setData(index, variant, role);
}

bool AbstractItemModelMonitor::remove(const QModelIndex &parent)
{
    if (!parent.isValid())
    {
        return false;
    }

    return removeRow(parent.row(), parent);
}

bool AbstractItemModelMonitor::removeAll(const QModelIndex &modelIndex)
{
    AbstractNodeItemMonitor* childItem = child(modelIndex);

    if (childItem != nullptr)
    {
        if (!childItem->childs().empty())
        {
            // TODO ???
            // for (int i = childItem->childs().size(); i > 0; i--)
            for (int i = childItem->childs().size() - 1; i >= 0; i--)
            {
                // QModelIndex index = childItem->childs().at(i - 1).second->getModelIndex();
                QModelIndex index = childItem->childs().at(i).second->modelIndex();
                removeAll(index);
                remove(index);
            }
        }
    }

    return true;
}

bool AbstractItemModelMonitor::insertRows(const std::pair<QString, AbstractNodeItemMonitor*> &nodeItem, int position,
                                          int rows, const QModelIndex &parent)
{
    AbstractNodeItemMonitor* parentItem = child(parent);
    beginInsertRows(parent, position, position + rows - 1);
    bool success = parentItem->insert(nodeItem);
    endInsertRows();
    return success;
}

bool AbstractItemModelMonitor::insertRow(const std::pair<QString, AbstractNodeItemMonitor*> &nodeItem, int arow,
                                         const QModelIndex &aparent)
{
    return insertRows(nodeItem, arow, 1, aparent);
}

bool AbstractItemModelMonitor::insertRow(int arow, const QModelIndex &aparent)
{
    // return insertRows(arow, 1, aparent);
    // --FIX
    return true;
}

// bool AbstractItemModelMonitor::insertRows(int position, int rows, const
// QModelIndex& parent)
//{
//    auto parentItem = child(parent);
//    bool success;
//    beginInsertRows(parent, position, position + rows - 1);
//    success = parentItem->insert();
//    endInsertRows();
//    return success;
//}

bool AbstractItemModelMonitor::removeRows(int position, int rows, const QModelIndex &index)
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

bool AbstractItemModelMonitor::setCheckState(AbstractNodeItemMonitor* parent, Qt::CheckState state)
{
    switch (state)
    {
        case Qt::Unchecked:
        {
            parent->setCheckState(state);
            updateData(parent->modelIndex());

            for (auto &child : parent->childs())
            {
                setData(child.second->modelIndex(), Qt::CheckState::Unchecked, Qt::CheckStateRole);
            }

            setData(parent->parent()->modelIndex(), Qt::CheckState::PartiallyChecked, Qt::CheckStateRole);
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
                child_state = static_cast<Qt::CheckState>(child.second->getCheckState());
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
                updateData(parent->modelIndex());

                if (parent->parent())
                {
                    setData(parent->parent()->modelIndex(), Qt::PartiallyChecked, Qt::CheckStateRole);
                }
            }
            break;
        }
        case Qt::Checked:
        {
            parent->setCheckState(state);
            updateData(parent->modelIndex());

            for (auto &child : parent->childs())
            {
                setData(child.second->modelIndex(), Qt::Checked, Qt::CheckStateRole);
            }

            if (parent->parent())
            {
                setData(parent->parent()->modelIndex(), Qt::CheckState::PartiallyChecked, Qt::CheckStateRole);
            }
            break;
        }
    }

    return true;
}

AbstractNodeItemMonitor* AbstractItemModelMonitor::child(const QModelIndex &index) const
{
    if (index.isValid())
    {
        auto item = static_cast<AbstractNodeItemMonitor*>(index.internalPointer());
        if (item != nullptr)
        {
            return item;
        }
    }

    return rootItem_;
}

void AbstractItemModelMonitor::appendTitle(const QStringList &listTitle)
{
    stringTitle_.append(listTitle);
}

void AbstractItemModelMonitor::appendTitle(const QString &title)
{
    stringTitle_.append(title);
    emit headerDataChanged(Qt::Orientation::Horizontal, stringTitle_.size(), stringTitle_.size());
}

void AbstractItemModelMonitor::editTitle(const QString &title, int index, const Qt::Orientation &orientation)
{
    setHeaderData(index, orientation, QVariant(title), Qt::EditRole);
}

void AbstractItemModelMonitor::removeTitle()
{
    stringTitle_.clear();
}

void AbstractItemModelMonitor::removeTitle(int position)
{
    stringTitle_.removeAt(position);
}

AbstractNodeItemMonitor* AbstractItemModelMonitor::root()
{
    return rootItem_;
}

bool AbstractItemModelMonitor::updateData(const QModelIndex &index)
{
    if (!index.isValid())
    {
        return false;
    }

    emit dataChanged(index, index);
    emit layoutChanged();
    return true;
}

bool AbstractItemModelMonitor::insertColumn(int acolumn, const QModelIndex &aparent)
{
    return insertColumns(acolumn, 1, aparent);
}

bool AbstractItemModelMonitor::removeRow(int arow, const QModelIndex &aparent)
{
    return removeRows(arow, 1, aparent);
}

bool AbstractItemModelMonitor::removeColumn(int acolumn, const QModelIndex &aparent)
{
    return removeColumns(acolumn, 1, aparent);
}

bool AbstractItemModelMonitor::insertColumns(int column, int count, const QModelIndex &parent)
{
    // beginInsertColumns();
    // ...
    // endInsertColumns();

    return true;
}

bool AbstractItemModelMonitor::removeColumns(int column, int count, const QModelIndex &parent)
{
    // beginRemoveColumns();
    // ...
    // endRemoveColumns();

    return true;
}

bool AbstractItemModelMonitor::moveRows(const QModelIndex &sourceParent, int sourceRow, int count,
                                        const QModelIndex &destinationParent, int destinationChild)
{
    // beginMoveRows
    // ...
    // endMoveRows

    return true;
}

bool AbstractItemModelMonitor::moveColumns(const QModelIndex &sourceParent, int sourceColumn, int count,
                                           const QModelIndex &destinationParent, int destinationChild)
{
    // beginMoveColumns
    // ...
    // endMoveColumns

    return true;
}