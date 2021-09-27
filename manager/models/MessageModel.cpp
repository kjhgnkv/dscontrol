#include "MessageModel.hpp"

#include "nodeeditor/lists/MessageList.hpp"
#include <QDebug>
#include <QJsonArray>

namespace dscontrol
{
MessageModel::MessageModel(QtNodes::MessageList* msgList, QObject* parent)
: QAbstractTableModel(parent)
, msgList_ {msgList}
{
    qDebug() << "MessageModel: konstructor";

    auto conn = connect(msgList_, &QtNodes::MessageList::messageAdded, this, &MessageModel::insertRow);
    metaConn_.push_back(conn);

    conn = connect(msgList_, &QtNodes::MessageList::messageUpdated, this, &MessageModel::update);
    metaConn_.push_back(conn);

    conn = connect(msgList_, &QtNodes::MessageList::messageToBeRemoved, this, &MessageModel::removeRow);

    metaConn_.push_back(conn);
}

MessageModel::~MessageModel()
{
    for (const auto &obj : metaConn_)
    {
        QObject::disconnect(obj);
    }
    qDebug() << "MessageModel: destructor";
}

QVariant MessageModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
            case Columns::Name:
                return tr("Name");
            case Columns::Id:
                return tr("ID");
            case Columns::Protocol:
                return tr("Protocol");
            case Columns::CommNames:
                return tr("Communication names");
            default:
                return QVariant();
        }
    }
    return QVariant();
}

int MessageModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return static_cast<int>(msgList_->size());
}

int MessageModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return Size;
}

QVariant MessageModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= rowCount())
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
            case Columns::Name:
                return msgList_->messageName(index.row());

            case Columns::Id:
                return msgList_->messageId(index.row());

            case Columns::Protocol:
                return msgList_->messageProtocol(index.row());

            case Columns::CommNames:
            {
                auto comms = msgList_->messageComms(index.row());
                QString retval;
                for (const auto &comm : comms)
                {
                    if (!retval.isEmpty())
                    {
                        retval.push_back("\n");
                    }
                    retval.push_back(comm.name_);
                }
                return retval;
            }
        }
    }
    if (role == Qt::TextAlignmentRole)
    {
        return Qt::AlignCenter;
    }
    return QVariant();
}

bool MessageModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (checkIndex(index) && index.row() < rowCount() && index.column() < columnCount() && role == Qt::EditRole)
    {
        QVariantList updates;

        switch (index.column())
        {
            case Name:
                updates.push_back(value.toString());
                break;
            case Id:
                updates.push_back(value.toString());
                break;
            case Protocol:
                updates.push_back(value.toString());
                break;
            case CommNames:
                updates.push_back(value.toString());
                break;
            default:
                return false;
        }
        msgList_->updateMessage(index.row(), updates, index.column());
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

Qt::ItemFlags MessageModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool MessageModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (count <= 0 || row < 0 || row > rowCount())
        return false;

    beginInsertRows(parent, row, row + count - 1);
    endInsertRows();
    return true;
}

bool MessageModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (count <= 0 || row < 0 || row + count > rowCount())
        return false;

    beginRemoveRows(parent, row, row + count - 1);
    endRemoveRows();
    return true;
}

void MessageModel::clear()
{
    qDebug() << "MEssageModel: clear";
    beginResetModel();
    msgList_->clear();
    endResetModel();
}

void MessageModel::insertRow(const int &row)
{
    insertRows(row, 1, {});
}

void MessageModel::removeRow(const int &row)
{
    removeRows(row, 1, {});
}

void MessageModel::update(const int &commId, int role)
{
    auto idx = this->index(commId, role, {});
    if (idx.isValid())
    {
        this->dataChanged(idx, idx, {role});
    }
    // TODO now update all model, code above doesn`t works? Fix it
    emit dataChanged(index(0, 0), index(rowCount(), columnCount()));
}
} // namespace dscontrol
