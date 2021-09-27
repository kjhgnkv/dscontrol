#include "NodeItemMonitorTree.hpp"

NodeItemMonitorTree::NodeItemMonitorTree()
{
}

NodeItemMonitorTree::NodeItemMonitorTree(const QString &ip, const QString &item1, const QString &item2,
                                         const LevelTreeMonitor::TYPE &level)
{
    value_.clear();
    ip_ = ip;

    switch (level)
    {
        case LevelTreeMonitor::TYPE::Computer:
        {
            ip_ = item1;
            hostName_ = item2;
            value_ << "[" + ip_ + "] " + hostName_;
            break;
        }

        case LevelTreeMonitor::TYPE::Application:
        {
            pid_ = item1;
            nameApp_ = item2;
            value_ << "[" + pid_ + "] " + nameApp_;
            break;
        }
    }
}

NodeItemMonitorTree::NodeItemMonitorTree(const QString &ip, const QString &value, const QJsonObject &json,
                                         const LevelTreeMonitor::TYPE &level)
{
    value_.clear();

    ip_ = ip;
    level_ = level;
    json_ = json;

    value_ << value;
}

NodeItemMonitorTree::~NodeItemMonitorTree()
{
}

bool NodeItemMonitorTree::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole)
    {
        value_.clear();
        value_ << value.toString();
    }
    if (role == Qt::CheckStateRole)
    {
        checkState_ = value.toInt();
    }

    return true;
}

QVariant NodeItemMonitorTree::data(const QModelIndex &index, int role)
{
    if (itemRoles_.find(index.column()) != itemRoles_.end() && role != Qt::DisplayRole)
    {
        if (role == Qt::CheckStateRole)
        {
            return checkState_;
        }
    }
    else if (role == Qt::DisplayRole)
    {
        return QVariant(value_.value(index.column()));
    }

    return QVariant();
}

const QJsonObject &NodeItemMonitorTree::getJson() const
{
    return json_;
}

quint32 NodeItemMonitorTree::getLevel() const
{
    return level_;
}

const QString &NodeItemMonitorTree::getHostName() const
{
    return hostName_;
}

const QString &NodeItemMonitorTree::getIp() const
{
    return ip_;
}

const QString &NodeItemMonitorTree::getPid() const
{
    return pid_;
}

const QString &NodeItemMonitorTree::getNameApp() const
{
    return nameApp_;
}
