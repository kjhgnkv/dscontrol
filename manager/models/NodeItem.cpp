#include "NodeItem.hpp"

#include "nodeeditor/models/ModelAdapter.hpp"
#include "NestedItem.hpp"
#include "ProjectModel.hpp"
#include "ProjectModelItem.hpp"
#include "nodeeditor/singletones/Facade.hpp"
#include <QDebug>

// TODO here too many new-delete in diverse methods - maybe better use smart
// pointers?

namespace dscontrol
{
NodeItem::NodeItem(ProjectModelItem* parent, QtNodes::ModelAdapter* adapter, const QString &caption, const Type &type)
: caption_ {caption}
, parent_ {parent}
, type_ {type}
, nestedModel_ {adapter}
{
    if (parent_)
    {
        parent_->addChild(this);
    }

    // FIXME need fix pre reserve
    children_.reserve(100);
}

NodeItem::~NodeItem()
{
    for (auto &item : children_)
    {
        delete item;
    }

    delete nestedModel_;
}

ProjectModelItem::Type NodeItem::type() const
{
    return type_;
}

void NodeItem::setFlowModel(QtNodes::DataFlowModel* model)
{
    if (nestedModel_)
    {
        connect(nestedModel_, &QAbstractItemModel::rowsInserted, this, &NodeItem::itemAdded);
        connect(nestedModel_, &QAbstractItemModel::rowsAboutToBeRemoved, this, &NodeItem::itemRemoved);
        connect(nestedModel_, &QAbstractItemModel::dataChanged, this, &NodeItem::itemChanged);
    }
    nestedModel_->setFlowModel(model);
}

ProjectModelItem* NodeItem::parentItem() const
{
    return parent_;
}

int NodeItem::rowOfItem(ProjectModelItem* childItem) const
{
    int row {};
    if (auto found = std::find_if(children_.begin(), children_.end(), [&row, childItem](const auto &val)
        {
            if (childItem == val)
            {
                return true;
            }
            ++row;
            return false;
        });
    found != children_.end())
    {
        return row;
    }
    qWarning() << "in ComputerItem asked unknows row";
    return -1;
}

ProjectModelItem* NodeItem::itemAtRow(int row) const
{
    if (row >= 0 && row < size())
    {
        return children_.at(row);
    }
    return nullptr;
}

const QString &NodeItem::caption() const
{
    return caption_;
}

int NodeItem::size() const
{
    return children_.size();
}

void NodeItem::itemAdded(const QModelIndex &, int row, int)
{
    if (row >= 0)
    {
        auto curIndex = nestedModel_->index(row, 0, {});

        std::shared_ptr<QtNodes::FlowScene> scene {};
        if (curIndex.data(QtNodes::ModelAdapter::Roles::NodiImpPtr).value<QtNodes::NodeImp*>())
        {
            scene = QtNodes::Facade::Instance().scenePart(
            curIndex.data(QtNodes::ModelAdapter::Roles::NodeIndexRole).value<QtNodes::NodeIndex>().id(), 1.0);
        }

        auto color = curIndex.data(Qt::BackgroundRole).value<QColor>();

        auto item = new NestedItem(this, color, nestedModel_->index(row, QtNodes::ModelAdapter::Columns::Caption).data(
        Qt::DisplayRole).toString(), scene.get() ? scene.get() : nullptr /*scene*/);

        item->setId(nestedModel_->index(row, QtNodes::ModelAdapter::Columns::Id).data(Qt::DisplayRole).toUuid());
        item->setDeploymentType(nestedModel_->index(row, QtNodes::ModelAdapter::Columns::DeploymentType).data(
        Qt::DisplayRole).value<QtNodes::Item::DeploymentType>());

        children_.insert(children_.begin() + row, item);

        rowInserted(row, this);
        return;
    }
    qWarning() << "in ComputerItem try add not valid row: " << row;
}

void NodeItem::itemRemoved(const QModelIndex &index, int row, int)
{
    // here we just ignore parent, because it always invalid, and give only
    // one inserted index (because ModelAdapter one level and remove only one
    // element at time);
    if (row >= 0)
    {
        rowRemoved(row, this);
        return;
    }
    qWarning() << "in ComputerItem try remove not valid row " << row;

    Q_UNUSED(index)
}

void NodeItem::itemChanged(const QModelIndex &index)
{
    if (index.isValid() && index.column() == QtNodes::ModelAdapter::Columns::Caption && index.row() <= size())
    {
        int row = index.row();

        if (children_.empty() || children_.size() <= static_cast<size_t>(row))
        {
            return;
        }

        auto item = children_.at(row);
        auto txt = index.data(Qt::DisplayRole).toString();
        item->setCaption(txt);
        if (auto itm = dynamic_cast<NestedItem*>(item); itm)
        {
            itm->setBackColor(index.data(Qt::BackgroundRole).value<QColor>());
            itm->setDeploymentType(
            index.model()->index(index.row(), QtNodes::ModelAdapter::Columns::DeploymentType).data(
            Qt::DisplayRole).value<QtNodes::Item::DeploymentType>());
        }

        dataChanged(row, item);
    }
}

void NodeItem::setCaption(const QString &caption)
{
    caption_ = caption;
}

void NodeItem::dataChanged(int row, ProjectModelItem* item)
{
    if (parent_)
    {
        parent_->dataChanged(row, item);
    }
}

void NodeItem::rowInserted(int row, ProjectModelItem* parent)
{
    if (parent_)
    {
        parent_->rowInserted(row, parent);
    }
}

void NodeItem::rowRemoved(int row, ProjectModelItem* parent)
{
    if (parent_)
    {
        parent_->rowRemoved(row, parent);
    }
}

void NodeItem::removeItemAtRow(int row)
{
    auto found = children_.begin() + row;
    delete *found;
    children_.erase(found);
}

} // namespace dscontrol
