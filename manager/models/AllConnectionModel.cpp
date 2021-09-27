// AllConnectionModel.cpp

#include "AllConnectionModel.hpp"

#include "models/DataFlowModel.hpp"
#include "nodes/Connection.hpp"
#include "nodes/Node.hpp"
#include "singletones/Facade.hpp"

#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QRegExpValidator>
#include <QTableView>
#include <memory>

namespace dscontrol
{
AllConnectionModel::AllConnectionModel(const QUuid &sourceId, QObject* parent)
: QAbstractTableModel(parent)
, sourceId_(sourceId)
{
    qDebug() << "AllConnectionModel: constructor";

    nodesModel_ = QtNodes::Facade::Instance().model();

    connect(nodesModel_, &QtNodes::DataFlowModel::connectionRemoved, this,
            qOverload<QtNodes::ConnectionID>(&AllConnectionModel::removeRow));

    connect(nodesModel_, &QtNodes::DataFlowModel::connectionAdded, this,
            qOverload<QtNodes::ConnectionID>(&AllConnectionModel::insertRow));

    index_ = nodesModel_->nodeIndex(sourceId_);
}

AllConnectionModel::AllConnectionModel(QObject* parent)
: QAbstractTableModel(parent)
{
    qDebug() << "AllConnectionModel: konstructor";
    nodesModel_ = QtNodes::Facade::Instance().model();
}

AllConnectionModel::~AllConnectionModel()
{
    qDebug() << "AllConnectionModel: destructor";
}

QVariant AllConnectionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
            case Columns::FromComponent:
                return tr("Source component");
            case Columns::ToComponent:
                return tr("Dest component");
            case Columns::CommunicationId:
                return tr("Communication Id");
            case Columns::MessageId:
                return tr("Message Id");
            case Columns::Type:
                return tr("Type");
            case Columns::Uuid:
                return tr("Uuid");
            default:
                return QVariant();
        }
    }
    return QVariant();
}

int AllConnectionModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (index_.isValid())
        return static_cast<int>(nodesModel_->nodeConnections(index_).size());
    return 0;
}

int AllConnectionModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return Columns::Size;
}

QVariant AllConnectionModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() > rowCount() || index.column() > columnCount())
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        if (!index_.isValid())
            return {};

        auto connections_ = nodesModel_->nodeConnections(index_);

        auto it = connections_.begin();
        std::advance(it, index.row());

        if (it == connections_.end())
            return {};

        auto conn = *it;
        QtNodes::Item::PortType type {QtNodes::Item::PortType::None};
        QtNodes::NodeIndex idx {};
        if (conn->getNode(QtNodes::Item::PortType::In) == nodesModel_->nodePointer(index_))
        {
            type = QtNodes::Item::PortType::In;
            idx = nodesModel_->nodeIndex(conn->getNode(QtNodes::Item::PortType::Out)->id());
        }

        else if (conn->getNode(QtNodes::Item::PortType::Out) == nodesModel_->nodePointer(index_))
        {
            type = QtNodes::Item::PortType::Out;
            idx = nodesModel_->nodeIndex(conn->getNode(QtNodes::Item::PortType::In)->id());
        }

        switch (index.column())
        {
            case Columns::FromComponent:
            {
                return QtNodes::Facade::Instance().generateComponentOwnership(index_);
            }
            case Columns::ToComponent:
            {
                if (idx.isValid())
                    return QtNodes::Facade::Instance().generateComponentOwnership(idx);
                return {};
            }
            case Columns::CommunicationId:
            {
                return conn->id().commId_;
            }
            case Columns::MessageId:
            {
                return conn->id().msgId_;
            }
            case Columns::Type:
            {
                switch (type)
                {
                    case QtNodes::Item::PortType::Out:
                    {
                        return "Send";
                    }

                    case QtNodes::Item::PortType::In:
                    {
                        return "Recv";
                    }
                    case QtNodes::Item::PortType::None:
                    {
                        return "None";
                    }
                }
                break;
            }
            case Columns::Uuid:
            {
                return conn->uuid();
            }
            default:
                return QVariant();
        }
    }
    if (role == Qt::TextAlignmentRole)
    {
        return Qt::AlignCenter;
    }

    return QVariant();
} // namespace dscontrol

Qt::ItemFlags AllConnectionModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    switch (index.column())
    {
        case Columns::FromComponent:
        case Columns::ToComponent:
        case Columns::CommunicationId:
        case Columns::MessageId:
        case Columns::Type:
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        case Columns::Uuid:
            return QAbstractTableModel::flags(index) & (~Qt::ItemIsEnabled) & (~Qt::ItemIsUserCheckable);

        default:
            return Qt::NoItemFlags;
    }
}

bool AllConnectionModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (count <= 0 || row < 0 || row > rowCount())
        return false;

    beginInsertRows(parent, row, row + count - 1);
    endInsertRows();

    return true;
}

bool AllConnectionModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (count <= 0 || row < 0 || row + count > rowCount())
        return false;

    beginRemoveRows(parent, row, row + count - 1);
    endRemoveRows();

    return true;
}

QStringList AllConnectionModel::getConnectionAssociate(const QtNodes::Item::PortType &type) const
{
    QStringList retval;

    for (const auto &conn : nodesModel_->nodeConnections(index_))
    {
        if (conn->getNode(type) == nodesModel_->nodePointer(index_))
        {
            retval.push_back(conn->id().commName_);
        }
    }

    return retval;
}

void AllConnectionModel::insertRow(const int &id)
{
    insertRows(id, 1, {});
}

void AllConnectionModel::removeRow(const int &id)
{
    removeRows(id, 1, {});
}

void AllConnectionModel::removeRow(QtNodes::ConnectionID id)
{
    auto lst = nodesModel_->nodeConnections(index_);
    auto found = std::find_if(lst.begin(), lst.end(), [id](const auto &conn)
    {
        if (conn->id() == id)
        {
            return true;
        }
        return false;
    });

    if (found != lst.end())
    {
        int dst = static_cast<int>(std::distance(lst.begin(), found));
        removeRow(dst);
    }
}

bool AllConnectionModel::hasIndex()
{
    return index_.isValid();
}

void AllConnectionModel::reset(const QUuid &newUuid)
{
    if (newUuid.isNull() && !hasIndex())
    {
        return;
    }

    qDebug() << "reset AllConnectionModel";
    beginResetModel();
    sourceId_ = newUuid;
    index_ = nodesModel_->nodeIndex(sourceId_);
    endResetModel();
    insertRows(0, rowCount());
    emit dataChanged(createIndex(0, 0), createIndex(rowCount(), Columns::Size));
}

void AllConnectionModel::clear()
{
    qDebug() << "clear AllConnectionModel";
    beginResetModel();
    sourceId_ = QUuid {};
    index_ = {};
    endResetModel();
}

void AllConnectionModel::insertRow(QtNodes::ConnectionID id)
{
    auto lst = nodesModel_->nodeConnections(index_);
    auto found = std::find_if(lst.begin(), lst.end(), [id](const auto &conn)
    {
        if (conn->id() == id)
        {
            return true;
        }
        return false;
    });

    if (found != lst.end())
    {
        int dst = static_cast<int>(std::distance(lst.begin(), found));
        insertRow(dst);
    }
}
} // namespace dscontrol
