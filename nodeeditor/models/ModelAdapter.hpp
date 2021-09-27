// ModelAdapter.hpp

#pragma once

#include "nodeeditor/nodeeditor_export.h"
#include "common/nodeeditor_globals.hpp"
#include "nodes/NodeImp.hpp"
#include "nodes/NodeIndex.hpp"
#include <QAbstractItemModel>

namespace QtNodes
{
class NodesModel;
class DataFlowModel;
class Node;
class NodeIndex;

/**\brief this class can be used for show all nodes in current NodesModel by
 * standard instruments of Qt (Q*View). Have only one level (no any parentnese)
 */
class NODEEDITOR_EXPORT ModelAdapter : public QAbstractItemModel
{
Q_OBJECT
public:
    enum Columns
    {
        Caption = 0,
        Id,
        DeploymentType,
        InPortCount,
        OutPortCount,
        Size
    };
    enum Roles
    {
        NodiImpPtr = Qt::UserRole + 1,
        NodeIndexRole,
        NodeType
    };

    explicit ModelAdapter(Item::NodeType type, QObject* parent = nullptr);
    ~ModelAdapter() = default;

    void setFlowModel(DataFlowModel* model);
    DataFlowModel* nodesModel() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

    /**\return pair of left and right indexes which represents NodeIndex
     */
    std::pair<QModelIndex, QModelIndex> indexes(const NodeIndex &nodeIndex) const;

    QModelIndex parent(const QModelIndex &index) const;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    QVariant headerData(int section, Qt::Orientation orientaion, int role = Qt::DisplayRole) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;

protected slots:
    /**\brief add index in the model, which represent added node
     */
    void nodeAdded(const NodeIndex &index);
    /**\brief remove node from the model
     */
    void nodeRemoved(const NodeIndex &index);
    /**\brief synchronize information in the model and represented node
     */
    virtual void nodeUpdated(const QtNodes::NodeIndex &nodeIndex);

protected:
    virtual NodeIndex getNodeIndex(const QModelIndex &index) const;

private:
    DataFlowModel* nodesModel_;
    Item::NodeType type_;
};
} // namespace QtNodes

// we must declare it here, because we use this for return from model
Q_DECLARE_METATYPE(QtNodes::NodeImp *)
Q_DECLARE_METATYPE(QtNodes::Item::NodeType)
Q_DECLARE_METATYPE(QtNodes::Item::DeploymentType)
