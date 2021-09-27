#include "ModelAdapter.hpp"

#include "DataFlowModel.hpp"
#include "NodesModel.hpp"
#include "nodes/NodeIndex.hpp"
#include "nodes/Connection.hpp"
#include "nodes/Node.hpp"

#include <QDebug>

namespace QtNodes
{
class PredicatFind
{
public:
    explicit PredicatFind(const NodeIndex &index, int* counter = nullptr)
    : index_ {index}
    , counter_ {counter}
    {
    }

    bool operator()(const NodeIndex &in)
    {
        if (in == index_)
        {
            return true;
        }
        if (counter_)
        {
            ++(*counter_);
        }
        return false;
    };

private:
    const NodeIndex &index_;
    int* counter_;
};

ModelAdapter::ModelAdapter(Item::NodeType type, QObject* parent)
: QAbstractItemModel {parent}
, nodesModel_ {}
, type_ {type}
{
}

void ModelAdapter::setFlowModel(DataFlowModel* model)
{
    // first disconnect from previous model
    beginResetModel();
    if (nodesModel_)
    {
        disconnect(nodesModel_, nullptr, this, nullptr);
        disconnect(this, nullptr, nodesModel_, nullptr);
    }

    nodesModel_ = model;

    if (model)
    {
        connect(model, &QtNodes::NodesModel::destroyed, this, [this]()
        {
            nodesModel_ = nullptr;
        });

        connect(model, &QtNodes::NodesModel::nodeAdded, this, &QtNodes::ModelAdapter::nodeAdded);
        connect(model, &QtNodes::NodesModel::nodeAboutToBeRemoved, this, &QtNodes::ModelAdapter::nodeRemoved);
        connect(model, &QtNodes::NodesModel::nodeUpdated, this, &QtNodes::ModelAdapter::nodeUpdated);
    }
    endResetModel();

    // TODO Maybe bad code

    for (const auto &node : model->nodes())
    {
        nodeAdded(model->nodeIndex(node.first));
    }
}

DataFlowModel* ModelAdapter::nodesModel() const
{
    return nodesModel_;
}

int ModelAdapter::rowCount(const QModelIndex &parent) const
{
    if (nodesModel_)
    {
        if (!parent.isValid())
        {
            return nodesModel_->nodes(type_).size();
        }
    }
    return {};
}

int ModelAdapter::columnCount(const QModelIndex &parent) const
{
    if (nodesModel_)
    {
        if (!parent.isValid())
        {
            return Columns::Size;
        }
    }
    return {};
}

QVariant ModelAdapter::data(const QModelIndex &index, int role) const
{
    if (nodesModel_ && index.isValid() && index.row() < rowCount(index.parent()) && index.column() < columnCount(
    index.parent()))
    {
        switch (role)
        {
            case Qt::DisplayRole:
                if (auto node = getNodeIndex(index); node.isValid())
                {
                    switch (index.column())
                    {
                        case Caption:
                            return node.nodeCaption();
                            break;
                        case Id:
                            return node.id();
                            break;
                        case DeploymentType:
                            return QVariant::fromValue(nodesModel_->nodeDeploymentType(node));
                        default:
                            break;
                    }
                }
                break;
            case Qt::BackgroundRole:
                if (auto node = getNodeIndex(index); node.isValid())
                {
                    if (!nodesModel_->nodeParent(node).isValid() && nodesModel_->nodeType(
                    node) != Item::NodeType::SystemNode)
                    {
                        return QColor(117, 0, 0, 50);
                    }
                    else
                    {
                        return QColor(Qt::transparent);
                    }
                }
                break;
            case Roles::NodiImpPtr:
                if (auto nodeIndex = getNodeIndex(index); nodeIndex.isValid())
                {
                    if (Node* node = nodeIndex.nodePointer())
                    {
                        return QVariant::fromValue(node->imp());
                    }
                }
                break;
            case Roles::NodeIndexRole:
                if (auto nodeIndex = getNodeIndex(index); nodeIndex.isValid())
                {
                    return QVariant::fromValue(nodeIndex);
                }
                break;

            case Roles::NodeType:
                if (auto nodeIndex = getNodeIndex(index); nodeIndex.isValid())
                {
                    return QVariant::fromValue(nodesModel_->nodeType(nodeIndex));
                }
                break;
            default:
                break;
        }
    }
    return {};
}

QModelIndex ModelAdapter::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (nodesModel_ && row >= 0 && column >= 0 && row < rowCount() && column < columnCount())
    {
        return createIndex(row, column);
    }
    return {};
}

std::pair<QModelIndex, QModelIndex> ModelAdapter::indexes(const NodeIndex &nodeIndex) const
{
    if (nodesModel_)
    {
        int row {};
        auto found = std::find_if(nodesModel_->begin(), nodesModel_->end(),
                                  [&row, &nodeIndex, &type_ = type_, &nodesModel_ = nodesModel_](const auto &val)
                                  {
                                      if (nodeIndex == val)
                                      {
                                          return true;
                                      }
                                      if (nodesModel_->nodeType(val) == type_)
                                      {
                                          ++row;
                                      }
                                      return false;
                                  });
        if (found != nodesModel_->end())
        {
            return std::pair {createIndex(row, 0), createIndex(row, Size - 1)};
        }
    }

    return {};
}

QModelIndex ModelAdapter::parent(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return {};
}

bool ModelAdapter::insertRows(int row, int count, const QModelIndex &parent)
{
    if (nodesModel_ && row >= 0)
    {
        beginInsertRows(parent, row, row + count - 1);
        endInsertRows();
        return true;
    }
    return false;
}

bool ModelAdapter::removeRows(int row, int count, const QModelIndex &parent)
{
    if (nodesModel_ && row >= 0)
    {
        beginRemoveRows(parent, row, row + count - 1);
        endRemoveRows();
        return true;
    }
    return false;
}

QVariant ModelAdapter::headerData(int section, Qt::Orientation orientaion, int role) const
{
    if (orientaion == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch (section)
        {
            case Columns::Caption:
                return {tr("caption")};
            case Columns::Id:
                return {tr("id")};
            case Columns::InPortCount:
                return {tr("input port count")};
            case Columns::OutPortCount:
                return {tr("output port count")};
            default:
                break;
        }
    }
    return {};
}

bool ModelAdapter::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (nodesModel_ && role == Qt::EditRole && index.isValid() && index.row() < rowCount() && index.column() < columnCount() && index.flags() & Qt::ItemIsEditable)
    {
        if (auto node = getNodeIndex(index); node.isValid())
        {
            nodesModel_->nodeChangeCaption(node, value.toString());
            emit dataChanged(index, index);
            return true;
        }
    }
    return false;
}

Qt::ItemFlags ModelAdapter::flags(const QModelIndex &index) const
{
    if (index.isValid() && index.row() < rowCount())
    {
        switch (index.column())
        {
            case Columns::Caption:
                return Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable | Qt::ItemIsEditable;
            default:
                break;
        }
    }
    return Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable;
}

NodeIndex ModelAdapter::getNodeIndex(const QModelIndex &index) const
{
    // TODO this works only if all NodeIndexes is in one model and have one
    // level
    if (index.isValid() && index.row() < rowCount() && index.column() < columnCount())
    {
        if (index.row() < static_cast<int>(nodesModel_->nodes(type_).size()))
        {
            return nodesModel_->nodes(type_).at(index.row());
        }
    }
    return {};
}

void ModelAdapter::nodeAdded(const NodeIndex &index)
{
    if (nodesModel_->nodeType(index) != type_)
        return;
    int row {};
    if (auto found = std::find_if(nodesModel_->begin(), nodesModel_->end(),
                                  [&row, &index, &type_ = type_, &nodesModel_ = nodesModel_](const auto &val)
                                  {
                                      if (index == val)
                                      {
                                          return true;
                                      }
                                      if (nodesModel_->nodeType(val) == type_)
                                      {
                                          ++row;
                                      }
                                      return false;
                                  });
    found != nodesModel_->end())
    {
        insertRow(row, {});
        return;
    }
    qWarning() << "problem in model adapter while insert";
}

void ModelAdapter::nodeRemoved(const NodeIndex &index)
{
    if (nodesModel_ && nodesModel_->nodeType(index) == type_)
    {
        int row {};
        auto found = std::find_if(nodesModel_->begin(), nodesModel_->end(),
                                  [&row, &index, &type_ = type_, &nodesModel_ = nodesModel_](const auto &val)
                                  {
                                      if (index == val)
                                      {
                                          return true;
                                      }
                                      if (nodesModel_->nodeType(val) == type_)
                                      {
                                          ++row;
                                      }
                                      return false;
                                  });
        if (found != nodesModel_->end())
        {
            removeRow(row, QModelIndex());
            return;
        }
        qDebug() << "problem in model adapter while remove";
    }
}

void ModelAdapter::nodeUpdated(const QtNodes::NodeIndex &nodeIndex)
{
    if (nodesModel_ && nodesModel_->nodeType(nodeIndex) == type_)
    {
        int row {};
        auto found = std::find_if(nodesModel_->begin(), nodesModel_->end(),
                                  [&row, &nodeIndex, &type_ = type_, &nodesModel_ = nodesModel_](const auto &val)
                                  {
                                      if (nodeIndex == val)
                                      {
                                          return true;
                                      }
                                      if (nodesModel_->nodeType(val) == type_)
                                      {
                                          ++row;
                                      }
                                      return false;
                                  });
        if (found != nodesModel_->end())
        {
            auto cur = index(row, 0);
            emit dataChanged(cur, cur.siblingAtColumn(Columns::Size - 1));
        }
    }
}
} // namespace QtNodes
