#include "NodeItemMonitorComputer.hpp"

#include "models/AbstractItemModelMonitor.hpp"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <QModelIndex>

#include <sstream>

#include "../../network/businessComponents/DSMonitor/src/DSMonitorHelp.hpp"

#include "Computer/Computer.hpp"
#include "Application/Application.hpp"

using namespace dsmonitor::Help;
using namespace monitor;

NodeItemMonitorComputer::NodeItemMonitorComputer(const QString &data)
: computer_(new Computer)
{
    modelApp_ = new AbstractItemModelMonitor(new NodeItemMonitorApplication());
    modelApp_->appendTitle({"Application", "PID", "CPU Usage", "Memory load"});

    setData(QModelIndex(), QVariant(data), Qt::EditRole);
}

NodeItemMonitorComputer::~NodeItemMonitorComputer()
{
    delete computer_;
    delete modelApp_;
}

QVariant NodeItemMonitorComputer::data(const QModelIndex &index, int role)
{
    if (itemRoles_.find(index.column()) != itemRoles_.end() && role != Qt::DisplayRole)
    {
        if (role == Qt::DecorationRole)
        {
            return *computer_->px();
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

bool NodeItemMonitorComputer::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole)
    {
        QJsonDocument jsonDocument = QJsonDocument::fromJson(value.toString().toUtf8());
        QJsonObject jsonObject_ = jsonDocument.object();

        QString keyNetwork {KeySystem::toString(KeySystem::network).c_str()};
        QJsonValue networkJsonValue = jsonObject_.value(keyNetwork);
        computer_->network()->update(networkJsonValue);

        QString keyCpu {KeySystem::toString(KeySystem::cpu).c_str()};
        QJsonValue cpuJsonValue = jsonObject_.value(keyCpu);
        computer_->cpu()->update(cpuJsonValue);

        QString keyMemory {KeySystem::toString(KeySystem::memory).c_str()};
        QJsonValue memoryJsonValue = jsonObject_.value(keyMemory);
        computer_->memory()->update(memoryJsonValue);

        QString keyStorage {KeySystem::toString(KeySystem::storage).c_str()};
        QJsonValue storageJsonValue = jsonObject_.value(keyStorage);
        computer_->storage()->update(storageJsonValue);

        QString keySystem {KeySystem::toString(KeySystem::system).c_str()};
        QJsonValue systemJsonValue = jsonObject_.value(keySystem);
        computer_->system()->update(systemJsonValue);

        QString keyProcess {KeySystem::toString(KeySystem::process).c_str()};
        QJsonValue processJsonValue = jsonObject_.value(keyProcess);
        update(processJsonValue);

        QString networkHostName_ = computer_->network()->hostName();
        QString networkIpAddress_ = computer_->network()->ipAddress();
        QString systemProductName_ = computer_->system()->productName();
        QString cpuUsage_ = computer_->cpu()->usage();
        QString memoryLoad_ = computer_->memory()->load();
        QString storageLoad_ = computer_->storage()->load();

        value_.clear();
        value_ << (networkHostName_ + "\n" + networkIpAddress_) << systemProductName_ << (cpuUsage_ + "%") << (memoryLoad_ + "%") << (storageLoad_ + "%");
    }

    return true;
}

struct TempJson
{
    QString pid;
    QString name;
    bool error;
    QJsonObject xml;
};

void NodeItemMonitorComputer::update(const QJsonValue &data)
{
    std::vector<TempJson> newApp;
    for (const auto &obj : data.toArray())
    {
        QJsonObject json = obj.toObject();

        QString keyProcess = KeySystem::toString(KeySystem::process).c_str();
        QJsonValue process = json[keyProcess];

        QString keyPid = Process::toString(Process::pid).c_str();
        QString pid = process[keyPid].toString();

        QString keyApp = Process::toString(Process::name).c_str();
        QString name = process[keyApp].toString();

        QString keyError = Error::toString(Error::error).c_str();
        QString error = process[keyError].toString();

        newApp.push_back({pid, name, error.isEmpty(), json});
    }

    if (!newApp.empty())
    {
        for (const auto &item : newApp)
        {
            if (!item.error)
            {
                auto root = dynamic_cast<NodeItemMonitorApplication*>(modelApp_->root());
                auto appIt = std::find_if(root->childs().begin(),
                                          root->childs().end(),
                                          [item](const std::pair<QString, AbstractNodeItemMonitor*> &node)
                                          {
                                              auto application = dynamic_cast<NodeItemMonitorApplication*>(node.second);

                                              return application->application()->name() == item.name;
                                          });

                if (appIt != root->childs().end())
                {
                    auto childUpdate = dynamic_cast<NodeItemMonitorApplication*>(appIt->second);
                    Application* application = childUpdate->application();
                    application->setIconColor(Icon::IconColor::Red);
                }
            }
            else
            {
                QJsonObject jsonObj = item.xml;
                QString keyProcess = KeySystem::toString(KeySystem::process).c_str();
                QJsonValue process = jsonObj[keyProcess];

                QJsonValue pid = process[Process::toString(Process::pid).c_str()];
                QString pidApp = pid.toString();

                auto root = dynamic_cast<NodeItemMonitorApplication*>(modelApp_->root());
                auto appIt = std::find_if(root->childs().begin(),
                                          root->childs().end(),
                                          [&](const std::pair<QString, AbstractNodeItemMonitor*> &node)
                                          {
                                              return node.first == pidApp;
                                          });

                if (appIt != root->childs().end())
                {
                    auto itemUpdate = dynamic_cast<NodeItemMonitorApplication*>(appIt->second);
                    modelApp_->update(itemUpdate->modelIndex(), QVariant(jsonObj), Qt::EditRole);
                }
                else
                {
                    auto itemAddApp = new NodeItemMonitorApplication(pidApp, jsonObj);
                    itemAddApp->appendItemDataRole(0, Qt::DecorationRole);
                    QModelIndex modelIndex = modelApp_->insert(std::make_pair(pidApp, itemAddApp));
                    modelApp_->updateData(modelIndex);
                }
            }
        }
    }

    auto root = dynamic_cast<NodeItemMonitorApplication*>(modelApp_->root());
    for (const auto &child : root->childs())
    {
        auto childUpdate = dynamic_cast<NodeItemMonitorApplication*>(child.second);
        Application* application = childUpdate->application();

        auto itApp = std::find_if(newApp.begin(), newApp.end(), [application](const TempJson &tempJson)
        {
            return tempJson.pid == application->pid();
        });

        if (itApp == newApp.end())
        {
            application->setIconColor(Icon::IconColor::Red);
        }
    }
}

AbstractItemModelMonitor* NodeItemMonitorComputer::modelApp() const
{
    return modelApp_;
}

Computer* NodeItemMonitorComputer::computer() const
{
    return computer_;
}
