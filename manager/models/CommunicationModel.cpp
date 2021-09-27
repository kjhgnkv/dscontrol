// CommunicationModel.cpp

#include "CommunicationModel.hpp"

#include <QDebug>
#include <QJsonArray>
#include <QRegExpValidator>

#include "nodeeditor/lists/CommunicationList.hpp"
#include "nodeeditor/common/nodeeditor_globals.hpp"

namespace dscontrol
{
CommunicationModel::CommunicationModel(QtNodes::CommunicationList* commList, QObject* parent)
: QAbstractTableModel(parent)
, commList_(commList)
{
    qDebug() << "CommunicationModel: constructor";

    if (commList_)
    {
        connect(commList_, &QtNodes::CommunicationList::communicationAdded, this, &CommunicationModel::insertRow);
        connect(commList_, qOverload<int>(&QtNodes::CommunicationList::communicationToBeRemoved), this,
                &CommunicationModel::removeRow);
        connect(commList_, &QtNodes::CommunicationList::updateError, this, &CommunicationModel::updateError,
                Qt::UniqueConnection);
    }
}

CommunicationModel::~CommunicationModel()
{
    qDebug() << "CommunicationModel: destructor";
}

QVariant CommunicationModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
            case Columns::DeviceType:
                return tr("Device Type");
            case Columns::DeviceName:
                return tr("Device Name");
            case Columns::CommName:
                return tr("Communication Name");
            case Columns::Id:
                return tr("ID");
            case Columns::Method:
                return tr("Method");
            case Columns::Component:
                return tr("Method\nComponent");
            case Columns::Sender:
                return tr("Sender(of)\n(IP:Port)");
            case Columns::Recv:
                return tr("Receiver(if)\n(IP:Port)");
            case Columns::Fragment:
                return tr("Fragment");
            default:
                return QVariant();
        }
    }
    return QVariant();
}

int CommunicationModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return static_cast<int>(commList_->size());
}

int CommunicationModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return Columns::Size;
}

QVariant CommunicationModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() > rowCount() || index.column() > columnCount())
        return QVariant();

    auto commId = commList_->communicationId(index.row());

    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
            case Columns::DeviceType:
            {
                return QtNodes::Device::types.at(static_cast<int>(commList_->communicationDeviceType(commId)));
            }
            case Columns::DeviceName:
                return commList_->communicationDeviceName(commId);

            case Columns::CommName:
                return commList_->communicationName(commId);

            case Columns::Id:
                return commId;

            case Columns::Method:
                return commList_->communicationConnectionMethod(commId);

            case Columns::Component:
                return commList_->communicationConnectionMethodComponent(commId);

            case Columns::Sender:
                return commList_->communicationSender(commId);

            case Columns::Recv:
                return commList_->communicationReceiver(commId);

            default:
                break;
        }
    }

    if (role == Qt::CheckStateRole)
    {
        switch (index.column())
        {
            case Columns::Fragment:
                return commList_->communicationFragment(commId) ? Qt::CheckState::Checked : Qt::CheckState::Unchecked;

            default:
                break;
        }
    }

    if (role == Qt::TextAlignmentRole)
    {
        return Qt::AlignCenter;
    }
    return QVariant();
}

bool CommunicationModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() > rowCount() || index.column() > columnCount() || value.toString() == "")
        return false;

    QVariantList updates;

    if (role == Qt::EditRole)
    {
        switch (index.column())
        {
            case Columns::DeviceType:
            {
                auto idx = QtNodes::Device::types.indexOf(value.toString());
                updates.push_back(idx);
                break;
            }

            case Columns::DeviceName:
                updates.push_back(value.toString());
                break;

            case Columns::CommName:
                updates.push_back(value.toString());
                break;

            case Columns::Method:
                updates.push_back(value.toString());
                break;

            case Columns::Component:
                updates.push_back(value.toString());
                break;

            case Columns::Sender:
                updates.push_back(value.toString());
                break;

            case Columns::Recv:
                updates.push_back(value.toString());
                break;

            default:
                break;
        }
    }

    if (role == Qt::CheckStateRole)
    {
        switch (index.column())
        {
            case Columns::Fragment:
                updates.push_back(value.toBool());
                break;

            default:
                return false;
        }
    }

    auto commId = commList_->communicationId(index.row());
    commList_->updateCommunication(commId, updates, QVector<int>() << index.column());
    // FIXME fixme in future
    dataChanged(index, index, QVector<int>() << role << Columns::DeviceName);
    return true;
}

Qt::ItemFlags CommunicationModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    switch (index.column())
    {
        case Columns::Id:
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        case Columns::Fragment:
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;

        case DeviceName:
        {
            if (index.model()->index(index.row(),
                                     Columns::DeviceType).data().toString() == QtNodes::Device::types.last())
            {
                return QAbstractTableModel::flags(index) & (~Qt::ItemIsEditable);
            }
        }
        case DeviceType:
        case CommName:
        case Sender:
        case Recv:
        case Method:
        case Component:
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;

        default:
            return Qt::NoItemFlags;
    }

    return QAbstractTableModel::flags(index);
}

bool CommunicationModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (count <= 0 || row < 0 || row > rowCount())
        return false;

    beginInsertRows(parent, row, row + count - 1);
    endInsertRows();

    return true;
}

bool CommunicationModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (count <= 0 || row < 0 || row + count > rowCount())
        return false;

    beginRemoveRows(parent, row, row + count - 1);
    endRemoveRows();

    return true;
}

void CommunicationModel::insertRow(const int &id)
{
    insertRows(id, 1, {});
}

void CommunicationModel::removeRow(const int &id)
{
    removeRows(id, 1, {});
}
} // namespace dscontrol
