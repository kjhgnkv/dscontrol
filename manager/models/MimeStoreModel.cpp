// MimeStoreModel.cpp

#include "MimeStoreModel.hpp"

#include "nodeeditor/nodes/FlowItemMimeData.hpp"
#include "repository/manager/ConfigLoader.hpp"

#include <QDebug>
#include <QJsonObject>
#include <QUuid>

namespace dscontrol
{
using namespace dsrepository;

class StoredItem
{
public:
    /*!
    \brief if you add first level item - parent have to be set to nullptr,
     otherwise it have to contain pointer to valid item of first level
     */

    StoredItem(StoredItem* parent, const QString &type, const QString caption, const QString &version,
               const QJsonObject &data)
    : parent_ {parent}
    , data_ {data}
    , type_ {type}
    , caption_ {caption}
    , version_ {version}
    , id_ {QUuid::createUuid()}
    {
    }

    ~StoredItem()
    {
        for (auto i : children_)
        {
            delete i;
        }
    }

    StoredItem* addChildren(const QString &type, const QString caption, const QString &version, const QJsonObject &data)
    {
        auto item = new StoredItem {this, type, caption, version, data};

        if (auto found = std::find_if(children_.begin(), children_.end(), [item](auto val)
            {
                if ((*val) == (*item))
                {
                    return true;
                }
                return false;
            });
        found == children_.end())
        {
            children_.push_back(item);
        }
        else
        {
            (*found)->type_ = type;
            (*found)->caption_ = caption;
            (*found)->version_ = version;
            (*found)->data_ = data;

            delete item;
            item = (*found);
        }

        return item;
    }

    /**\brief remove children by row
     */
    void removeChildren(int row)
    {
        if (row >= 0 && children_.size() > static_cast<size_t>(row))
        {
            delete children_[row];
            children_.erase(children_.begin() + row);
        }
    }

    const QString &type() const
    {
        return type_;
    }

    const QString &caption() const
    {
        return caption_;
    }

    const QString &version() const
    {
        return version_;
    }

    const QJsonObject &data() const
    {
        return data_;
    }

    const QVector<StoredItem*> &children() const
    {
        return children_;
    }

    StoredItem* parent() const
    {
        return parent_;
    }

    QUuid id() const
    {
        return id_;
    }

    int row()
    {
        return parent_->children().indexOf(this);
    };

    bool operator==(const StoredItem &lhs)
    {
        return (lhs.type() == this->type_) && (lhs.caption() == this->caption_) && (lhs.version_ == this->version_);
    }

private:
    StoredItem* parent_;
    QVector<StoredItem*> children_;
    QJsonObject data_;
    QString type_;
    QString caption_;
    QString version_;
    QUuid id_;
};

MimeStoreModel::MimeStoreModel(QObject* parent)
: QAbstractItemModel(parent)
{
    root_ = new StoredItem {nullptr, "", "Root", "", {}};
}

MimeStoreModel::~MimeStoreModel()
{
    delete root_;
}

int MimeStoreModel::rowCount(const QModelIndex &parent) const
{
    StoredItem* parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = root_;
    else
        parentItem = static_cast<StoredItem*>(parent.internalPointer());

    return parentItem->children().count();
}

int MimeStoreModel::columnCount(const QModelIndex &parent) const
{
    // have only one column for all elements
    Q_UNUSED(parent);
    return Columns::Size;
}

QVariant MimeStoreModel::data(const QModelIndex &index, int role) const
{
    if (!checkIndex(index))
    {
        return {};
    }

    auto item = static_cast<StoredItem*>(index.internalPointer());
    if (!item)
    {
        return {};
    }

    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
            case Columns::Caption:
            {
                return item->caption();
            }

            default:
                break;
        }
    }
    else if (role == ModelRoles::UniqiueRole)
    {
        return item->id();
    }
    else if (role == ModelRoles::JsonRole)
    {
        return item->data();
    }

    return {};
}

QModelIndex MimeStoreModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
    {
        return {};
    }

    StoredItem* parentItem;

    if (!parent.isValid())
    {
        parentItem = root_;
    }
    else
    {
        parentItem = static_cast<StoredItem*>(parent.internalPointer());
    }

    StoredItem* childItem = parentItem->children()[row];

    if (childItem)
    {
        return createIndex(row, column, childItem);
    }

    return QModelIndex();
}

QModelIndex MimeStoreModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return QModelIndex();
    }

    StoredItem* childItem = static_cast<StoredItem*>(index.internalPointer());
    StoredItem* parentItem = childItem->parent();

    if (parentItem == root_)
    {
        return QModelIndex();
    }

    return createIndex(parentItem->parent()->children().indexOf(parentItem), 0, parentItem);
}

bool MimeStoreModel::insertRows(int row, int count, const QModelIndex &parent)
{
    // just for send signal for view
    if (!parent.isValid() && row >= 0 && row <= rowCount() && count > 0)
    {
        beginInsertRows(parent, row, row + count);
        endInsertRows();
        return true;
    }
    return false;
}

bool MimeStoreModel::removeRows(int row, int count, const QModelIndex &parent)
{
    // just for send signal for view
    if (row >= 0 && row < rowCount() && count > 0)
    {
        auto item = storedItem(parent);
        beginRemoveRows(parent, row, row + count);
        item->removeChildren(row);
        endRemoveRows();
        return true;
    }
    return false;
}

// first level not generate any mime datas
Qt::ItemFlags MimeStoreModel::flags(const QModelIndex &index) const
{
    if (index.parent().isValid())
    {
        return QAbstractItemModel::flags(index) | Qt::ItemFlag::ItemIsDragEnabled;
    }
    return QAbstractItemModel::flags(index);
}

QStringList MimeStoreModel::mimeTypes() const
{
    return {QtNodes::FlowItemMimeData::mimeType()};
}

QMimeData* MimeStoreModel::mimeData(const QModelIndexList &indexes) const
{
    // because in indexes includes all columns as diverses indexes
    if (indexes.size() == Columns::Size)
    {
        if (const auto &curIndex = *indexes.begin(); checkIndex(curIndex))
        {
            auto item = static_cast<StoredItem*>(curIndex.internalPointer());
            if (item && item->data().isEmpty())
            {
                return nullptr;
            }

            QJsonObject node;
            node["implementation"] = item->data();
            return new QtNodes::FlowItemMimeData {item->type(), node};
        }
    }
    return nullptr;
}

void MimeStoreModel::insertRow(const int &row, const QModelIndex &parent)
{
    insertRows(row, 1, parent);
}

void MimeStoreModel::removeRow(const int &row, const QModelIndex &parent)
{
    removeRows(row, 1, parent);
}

bool MimeStoreModel::addComponentConfig(const Component &component)
{
    // if we not have the category add it
    auto insertedCategory = root_->addChildren(component.type_, component.category_, "", {});

    if (!insertedCategory)
    {
        return false;
    }
    insertRow(insertedCategory->row(), {});
    categories_.push_back(insertedCategory);

    auto child = insertedCategory->addChildren(component.type_, component.caption_, "", {});

    if (!child)
    {
        return false;
    }
    {
        auto indexes = match(index(0, 0), ModelRoles::UniqiueRole, child->id(), 1, Qt::MatchRecursive);
        if (indexes.isEmpty())
        {
            return false;
        }
        const auto &idx = indexes.at(0);
        insertRow(child->row(), idx);
    }

    auto subChild = child->addChildren(component.type_, component.version_, component.version_, component.info_);
    if (!subChild)
    {
        return false;
    }

    {
        auto indexes = match(index(0, 0), ModelRoles::UniqiueRole, subChild->id(), 1, Qt::MatchRecursive);
        if (indexes.isEmpty())
        {
            return false;
        }
        const auto &idx = indexes.at(0);
        insertRow(child->row(), idx);
    }

    return true;
}

void MimeStoreModel::removeComponentConfig(const Component &component)
{
    QModelIndex idx {};
    auto indexes = match(index(0, 0), ModelRoles::JsonRole, component.info_, 3, Qt::MatchRecursive);
    if (indexes.isEmpty())
    {
        return;
    }
    idx = indexes.at(0);

    while (idx.isValid())
    {
        auto parentIdx = idx.parent();
        removeRow(idx.row(), parentIdx);
        idx = parentIdx;

        auto item = storedItem(parentIdx);
        if (!item->children().isEmpty())
        {
            break;
        }
    }
}

int MimeStoreModel::indexType(QModelIndex index)
{
    if (static_cast<StoredItem*>(index.internalPointer()))
        return 1;
    return 0;
}

const QJsonObject MimeStoreModel::storedItemData(QModelIndex index)
{
    if (auto storedItem = static_cast<StoredItem*>(index.internalPointer()))
    {
        return storedItem->data();
    }
    return {};
}

StoredItem* MimeStoreModel::storedItem(const QModelIndex &index)
{
    StoredItem* item;
    if (!index.isValid())
    {
        item = root_;
    }
    else
    {
        item = reinterpret_cast<StoredItem*>(index.internalPointer());
    }

    return item;
}

void MimeStoreModel::clear()
{
    beginResetModel();
    categories_.clear();
    endResetModel();
}
} // namespace dscontrol
