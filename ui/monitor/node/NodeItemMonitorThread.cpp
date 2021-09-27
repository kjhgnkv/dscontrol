#include "NodeItemMonitorThread.hpp"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include <QModelIndex>

#include <QString>
#include <vector>

#include "../../network/businessComponents/DSMonitor/src/DSMonitorHelp.hpp"

using namespace dsmonitor::Help;
using namespace monitor;

NodeItemMonitorThread::NodeItemMonitorThread()
: thread_ {new Thread}
{
}

NodeItemMonitorThread::NodeItemMonitorThread(const QString &pid, const QJsonObject &value)
: thread_ {new Thread}
{
    setData(QModelIndex(), QVariant(value), Qt::EditRole);
}

NodeItemMonitorThread::~NodeItemMonitorThread()
{
    delete thread_;
}

bool NodeItemMonitorThread::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole)
    {
        QJsonObject object = value.toJsonObject();

        QString keyPid = dsmonitor::Help::Thread::toString(dsmonitor::Help::Thread::pid).c_str();
        QJsonValue qJsonPid = object[keyPid];
        QString pid = qJsonPid.toString();

        QString keyUsage = dsmonitor::Help::Thread::toString(dsmonitor::Help::Thread::usage).c_str();
        QJsonValue cpuUsage = object[keyUsage];
        QString cpu = cpuUsage.toString();

        thread_->update(pid, cpu);

        value_.clear();
        value_ << thread_->pid() << thread_->cpu();

        return false;
    }

    return true;
}

QVariant NodeItemMonitorThread::data(const QModelIndex &index, int role)
{
    if (itemRoles_.find(index.column()) != itemRoles_.end() && role != Qt::DisplayRole)
    {
        if (role == Qt::TextAlignmentRole)
        {
            return Qt::AlignCenter;
        }
    }
    else if (role == Qt::DisplayRole)
    {
        return QVariant(value_.value(index.column()));
    }
    else if (role == Qt::TextAlignmentRole)
    {
        return Qt::AlignCenter;
    }

    return QVariant();
}
