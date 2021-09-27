#include "Application.hpp"

#include <QJsonObject>
#include <QJsonArray>

#include "../../network/businessComponents/DSMonitor/src/DSMonitorHelp.hpp"

using namespace monitor;

Application::Application()
: Icon()
{
}

Application::~Application()
{
    for (const auto &item : vecThreads_)
    {
        delete item.second;
    }
    vecThreads_.clear();

    for (const auto &item: vecComponents_)
    {
        delete item;
    }
    vecComponents_.clear();
}

void Application::updateApplication(const QJsonObject &object)
{
    // pid
    QString keyPid = dsmonitor::Help::Process::toString(dsmonitor::Help::Process::pid).c_str();
    QJsonValue pid = object[keyPid];
    pid_ = pid.toString();

    // memoryLoad
    QString keyMemoryLoad = dsmonitor::Help::Process::toString(dsmonitor::Help::Process::memoryLoad).c_str();
    QJsonValue memoryLoad = object[keyMemoryLoad];
    memoryLoad_ = memoryLoad.toString();

    // cpuUsage
    QString keyCpuUsage = dsmonitor::Help::Process::toString(dsmonitor::Help::Process::usageCPU).c_str();
    QJsonValue cpuUsage = object[keyCpuUsage];
    cpuUsage_ = cpuUsage.toString();

    // status
    QString keyStatus = dsmonitor::Help::Process::toString(dsmonitor::Help::Process::status).c_str();
    QJsonValue status = object[keyStatus];
    if (status.toString() == "true")
        setIconColor(IconColor::Green);
    else
        setIconColor(IconColor::Red);

    // name
    QString keyName = dsmonitor::Help::Process::toString(dsmonitor::Help::Process::name).c_str();
    QJsonValue name = object[keyName];
    name_ = name.toString();

    // threads
    QString keyThread = dsmonitor::Help::Process::toString(dsmonitor::Help::Process::threads).c_str();
    QJsonValue threads = object[keyThread];
    threads_ = threads.toString();

    // path
    QString keyPath = dsmonitor::Help::Process::toString(dsmonitor::Help::Process::path).c_str();
    QJsonValue processPath = object[keyPath];
    path_ = processPath.toString();

    // userName
    QString keyUserName = dsmonitor::Help::Process::toString(dsmonitor::Help::Process::userName).c_str();
    QJsonValue userName = object[keyUserName];
    userName_ = userName.toString();

    QString keyWorkingSetSize = dsmonitor::Help::Process::toString(dsmonitor::Help::Process::workingSetSize).c_str();
    QJsonValue workingSetSize = object[keyWorkingSetSize];
    workingSetSize_ = workingSetSize.toString();
}

void Application::updateComponents(const QJsonObject &object)
{
    // if (components_.empty())
    // {
    //     for (const auto &component : object["Components"].toArray())
    //     {
    //         if (!component.isNull())
    //         {
    //             auto newComponent = new NodeItemComponent();
    //
    //             QJsonObject objectComponent = component.toObject();
    //
    //             newComponent->json_ = objectComponent;
    //             newComponent->name_ = objectComponent["name"].toString();
    //
    //             auto obj = objectComponent["Object"].toObject();
    //             if (!obj.isEmpty())
    //             {
    //                 auto newObj = new NodeItemObject();
    //                 newObj->json_ = obj;
    //                 newObj->name_ = obj["name"].toString();
    //                 newComponent->objects_.push_back(newObj);
    //             }
    //             components_.push_back(newComponent);
    //         }
    //     }
    // }
}

const QString &Application::pid() const
{
    return pid_;
}

const QString &Application::memoryLoad() const
{
    return memoryLoad_;
}

const QString &Application::cpuUsage() const
{
    return cpuUsage_;
}

const int &Application::status() const
{
    return status_;
}

const QString &Application::name() const
{
    return name_;
}

const QString &Application::countOfThreads() const
{
    return threads_;
}

const QString &Application::processPath() const
{
    return path_;
}

const QString &Application::userName() const
{
    return userName_;
}

const QString &Application::workingSetSize() const
{
    return workingSetSize_;
}

const std::vector<std::pair<QString, Thread*>> &Application::threads() const
{
    return vecThreads_;
}

const std::vector<NodeItemComponent*> &Application::components() const
{
    return vecComponents_;
}