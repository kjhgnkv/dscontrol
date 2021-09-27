#include "ConnectionsStaticModel.hpp"

#include "lists/Message.hpp"
#include "lists/MessageList.hpp"
#include "models/DataFlowModel.hpp"
#include "nodes/Connection.hpp"
#include "nodes/Node.hpp"
#include "singletones/Facade.hpp"

namespace dscontrol
{
ConnectionsStaticModel::ConnectionsStaticModel(QObject* parent)
: QAbstractTableModel(parent)
, model_ {nullptr}
, messageList_ {nullptr}
{
    model_ = QtNodes::Facade::Instance().model();
    messageList_ = QtNodes::Facade::Instance().messageList();
    if (model_)
    {
        for (auto conn : model_->connections())
        {
            conns_.push_back(conn.second);
        }
    }
}

ConnectionsStaticModel::~ConnectionsStaticModel()
{
}

QVariant ConnectionsStaticModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
    {
        return QVariant();
    }

    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
            case Columns::Num:
            {
                return tr("#");
            }
            case Columns::SenderName:
            {
                return tr("Entity #1");
            }
            case Columns::SenderType:
            {
                return tr("Entity\nType");
            }
            case Columns::RecvName:
            {
                return tr("Entity #2");
            }
            case Columns::RecvType:
            {
                return tr("Entity\nType");
            }
            case Columns::CommName:
            {
                return tr("Communication\nType");
            }
            case Columns::CommID:
            {
                return tr("ID");
            }
            case Columns::MsgName:
            {
                return tr("Message");
            }
            case Columns::MsgID:
            {
                return tr("ID");
            }
            default:
            {
                return QVariant();
            }
        }
    }
    return QVariant();
}

int ConnectionsStaticModel::rowCount(const QModelIndex &) const
{
    return (int)conns_.size();
}

int ConnectionsStaticModel::columnCount(const QModelIndex &) const
{
    return Size;
}

QVariant ConnectionsStaticModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if (!model_ || !messageList_ || !index.isValid() || row < 0 || row >= rowCount())
    {
        return QVariant();
    }
    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
            case Columns::Num:
            {
                return row + 1;
            }
            case Columns::SenderName:
            {
                return conns_.at(row)->getNode(QtNodes::Item::PortType::Out)->caption();
            }
            case Columns::SenderType:
            {
                return tr("Sender");
            }
            case Columns::RecvName:
            {
                return conns_.at(row)->getNode(QtNodes::Item::PortType::In)->caption();
            }
            case Columns::RecvType:
            {
                return tr("Receiver");
            }
            case Columns::CommName:
            {
                return conns_.at(row)->id().commName_;
            }
            case Columns::CommID:
            {
                return conns_.at(row)->id().commId_;
            }
            case Columns::MsgName:
            {
                return messageList_->messageName(conns_.at(row)->id().msgId_);
            }
            case Columns::MsgID:
            {
                return conns_.at(row)->id().msgId_;
            }
            default:
            {
                return QVariant();
            }
        }
    }
    if (role == Qt::TextAlignmentRole)
    {
        return Qt::AlignCenter;
    }
    return QVariant();
}

Qt::ItemFlags ConnectionsStaticModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

} // namespace dscontrol
