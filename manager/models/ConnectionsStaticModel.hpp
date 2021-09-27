#pragma once

#include <QAbstractTableModel>
#include <memory>

namespace QtNodes
{
class DataFlowModel;
class MessageList;
class Connection;
} // namespace QtNodes

namespace dscontrol
{
/*!
\brief Model containing connections
 */
class ConnectionsStaticModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Columns
    {
        Num,
        SenderName,
        SenderType,
        RecvName,
        RecvType,
        CommName,
        CommID,
        MsgName,
        MsgID,
        Size
    };
    explicit ConnectionsStaticModel(QObject *parent = nullptr);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    virtual ~ConnectionsStaticModel() override;

private:
    QtNodes::DataFlowModel *model_;
    QtNodes::MessageList *messageList_;
    std::vector<std::shared_ptr<QtNodes::Connection>> conns_;
};
} // namespace dscontrol
