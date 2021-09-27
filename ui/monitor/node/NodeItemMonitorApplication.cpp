#include "NodeItemMonitorApplication.hpp"

#include "Application/Application.hpp"

#include "models/AbstractItemModelMonitor.hpp"

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

NodeItemMonitorApplication::NodeItemMonitorApplication()
: application_(new Application {})
{
}

NodeItemMonitorApplication::NodeItemMonitorApplication(const QString &pid, const QJsonObject &value)
: application_(new Application {})
{
    modelThreads_ = new AbstractItemModelMonitor(new NodeItemMonitorThread());
    modelThreads_->appendTitle({"PID", "CPU Usage"});

    setData(QModelIndex(), QVariant(value), Qt::EditRole);
}

NodeItemMonitorApplication::~NodeItemMonitorApplication()
{
    delete modelThreads_;
    delete application_;
}

bool NodeItemMonitorApplication::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole)
    {
        QJsonObject jsonObj = value.toJsonObject();

        QString keyProcess = KeySystem::toString(KeySystem::process).c_str();
        QJsonValue process = jsonObj[keyProcess];
        application_->updateApplication(process.toObject());

        QString keyThread = KeySystem::toString(KeySystem::threads).c_str();
        QJsonValue thread = jsonObj[keyThread];
        update(thread);

        value_.clear();
        value_ << application_->name() << application_->pid() << application_->cpuUsage() << application_->memoryLoad();

        return false;
    }

    return true;
}

void NodeItemMonitorApplication::update(const QJsonValue &data)
{
    std::vector<QString> tempVecThreads;

    for (const auto &obj : data.toArray())
    {
        QJsonObject object = obj.toObject();

        QString keyPid = dsmonitor::Help::Thread::toString(dsmonitor::Help::Thread::pid).c_str();
        QJsonValue qJsonPid = object[keyPid];

        QString pid = qJsonPid.toString();

        tempVecThreads.push_back(pid);
    }

    for (const auto &obj : data.toArray())
    {
        QJsonObject object = obj.toObject();

        QString keyPid = dsmonitor::Help::Thread::toString(dsmonitor::Help::Thread::pid).c_str();
        QJsonValue qJsonPid = object[keyPid];
        QString pidThread = qJsonPid.toString();

        NodeItemMonitorThread* root = dynamic_cast<NodeItemMonitorThread*>(modelThreads_->root());
        auto threadIt = std::find_if(root->childs().begin(),
                                     root->childs().end(),
                                     [&](const std::pair<QString, AbstractNodeItemMonitor*> &node)
                                     {
                                         return node.first == pidThread;
                                     });

        if (threadIt != root->childs().end())
        {
            NodeItemMonitorThread* itemUpdate = dynamic_cast<NodeItemMonitorThread*>(threadIt->second);
            modelThreads_->update(itemUpdate->modelIndex(), QVariant(object), Qt::EditRole);
        }
        else
        {
            AbstractNodeItemMonitor* itemAddThread = new NodeItemMonitorThread(pidThread, object);
            itemAddThread->appendItemDataRole(0, Qt::DecorationRole);
            QModelIndex modelIndex = modelThreads_->insert(std::make_pair(pidThread, itemAddThread));
            modelThreads_->updateData(modelIndex);
        }
    }
}

QVariant NodeItemMonitorApplication::data(const QModelIndex &index, int role)
{
    if (itemRoles_.find(index.column()) != itemRoles_.end() && role != Qt::DisplayRole)
    {
        if (role == Qt::DecorationRole)
        {
            return *application_->px();
        }
        else if (role == Qt::TextAlignmentRole)
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

Application* NodeItemMonitorApplication::application() const
{
    return application_;
}

AbstractItemModelMonitor* NodeItemMonitorApplication::modelApp() const
{
    return modelThreads_;
}
