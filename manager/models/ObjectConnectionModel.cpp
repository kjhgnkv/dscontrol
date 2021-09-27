#include "ObjectConnectionModel.hpp"

#include "nodes/Connection.hpp"
#include "nodes/Node.hpp"
#include "singletones/Facade.hpp"
#include "models/DataFlowModel.hpp"

#include <QDebug>

dscontrol::ObjectConnectionModel::ObjectConnectionModel(const QUuid &sourceId, const QtNodes::Item::PortType &type,
                                                        QObject* parent)
: QAbstractTableModel {parent}
, sourceId_ {sourceId}
, type_ {type}
, rows {0}
{
    qDebug() << "ObjectConnectionModel: konstructor";

    nodesModel_ = QtNodes::Facade::Instance().model();
    index_ = nodesModel_->nodeIndex(sourceId_);

    auto conns = nodesModel_->nodeConnections(index_);
    auto childs = nodesModel_->nodeChildsInLowestLevel(index_);
    for (const auto &conn : conns)
    {
        if (auto node = conn->getNode(type_))
        {
            auto found = std::find_if(childs.begin(), childs.end(), [=](const auto &val)
            {
                if (val.id() == node->id())
                {
                    return true;
                }
                return false;
            });
            if (found != childs.end())
            {
                connections_.push_back(conn.get());
            }
        }
    }

    connections_.sort();
    connections_.unique();
    rows = connections_.size();
}

QVariant dscontrol::ObjectConnectionModel::headerData(int section, Qt::Orientation orientation, int role) const
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
            case Columns::Communications:
                return tr("Communications");
            default:
                return QVariant();
        }
    }
    return QVariant();
}

int dscontrol::ObjectConnectionModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return rows;
}

int dscontrol::ObjectConnectionModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return Columns::Size;
}

QVariant dscontrol::ObjectConnectionModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() > rowCount() || index.column() > columnCount())
        return QVariant();

    if (!index_.isValid())
        return {};

    int row = index.row();
    auto it = connections_.begin();
    std::advance(it, row);

    QtNodes::Connection* conn = *it;

    QtNodes::Node* nodePtr = conn->getNode(type_);

    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
            case Columns::Name:
            {
                return nodePtr->caption();
            }
            case Columns::Id:
            {
                return conn->id().msgId_;
            }
            case Columns::Protocol:
            {
                switch (type_)
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
            case Columns::Communications:
            {
                return conn->id().commName_;
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
}

Qt::ItemFlags dscontrol::ObjectConnectionModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void dscontrol::ObjectConnectionModel::clear()
{
    qDebug() << "clear ObjectConnectionModel";
    beginResetModel();
    sourceId_ = QUuid {};
    index_ = {};
    endResetModel();
}
