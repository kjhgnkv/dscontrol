#pragma once

#include "nodeeditor/common/nodeeditor_globals.hpp"
#include "nodeeditor/nodes/ConnectionID.hpp"
#include "nodeeditor/nodes/NodeIndex.hpp"
#include <QAbstractTableModel>
#include <QUuid>
#include <memory>

namespace QtNodes
{
class DataFlowModel;
class Connection;
} // namespace QtNodes

namespace dscontrol
{
/*!
\brief model class that represents all connections
 */
class AllConnectionModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Columns
    {
        FromComponent,
        ToComponent,
        CommunicationId,
        MessageId,
        Type,
        Uuid,
        Size
    };
    /*!
    \param sourceId Uuid of target component
    */
    explicit AllConnectionModel(const QUuid &sourceId, QObject *parent = nullptr);
    explicit AllConnectionModel(QObject *parent = nullptr);

    virtual ~AllConnectionModel() override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    /*!
    \return All connections associated with current component by port type
    */
    QStringList getConnectionAssociate(const QtNodes::Item::PortType &type) const;

    /*!
    \return true if current component index is valid
    */
    bool hasIndex();

    /*!
    \brief resets model by target component Uuid
    */
    void reset(const QUuid &newUuid);

    /*!
    \brief set current component index to invalid and reset model
    */
    virtual void clear();

public slots:
    void insertRow(QtNodes::ConnectionID id);
    void insertRow(const int &id);
    void removeRow(const int &id);
    void removeRow(QtNodes::ConnectionID id);

protected:
    QUuid sourceId_;
    QtNodes::DataFlowModel *nodesModel_;
    QtNodes::NodeIndex index_;
};
} // namespace dscontrol
Q_DECLARE_METATYPE(QtNodes::Item::PortType)
