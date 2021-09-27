#pragma once

#include "nodeeditor/common/nodeeditor_globals.hpp"
#include "nodeeditor/nodes/NodeIndex.hpp"
#include <QAbstractTableModel>
#include <QUuid>

namespace QtNodes
{
class DataFlowModel;
class Connection;
} // namespace QtNodes

namespace dscontrol
{
/*!
\brief Model for object attributes dialog tables
 */
class ObjectConnectionModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Columns
    {
        Name,
        Id,
        Protocol,
        Communications,
        Size
    };
    explicit ObjectConnectionModel(const QUuid &sourceId, const QtNodes::Item::PortType &type, QObject *parent = nullptr);
    explicit ObjectConnectionModel(QObject *parent = nullptr);

    virtual ~ObjectConnectionModel() override = default;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    virtual void clear();

protected:
    QUuid sourceId_;
    QtNodes::DataFlowModel *nodesModel_;
    QtNodes::NodeIndex index_;
    QtNodes::Item::PortType type_;
    std::list<QtNodes::Connection *> connections_;
    int rows;
};
} // namespace dscontrol
