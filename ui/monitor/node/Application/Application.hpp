#pragma once

#include <QString>
#include <QPixmap>
#include "../Icon/Icon.hpp"

#include "node/Thread/Thread.hpp"
#include "component/NodeItemComponent.hpp"

namespace monitor
{
class Application : public Icon
{
public:
    Application();
    virtual ~Application();
    void updateApplication(const QJsonObject &data);
    void updateComponents(const QJsonObject &object);

public:
    const QString &pid() const;
    const QString &memoryLoad() const;
    const QString &cpuUsage() const;
    const int &status() const;
    const QString &name() const;
    const QString &countOfThreads() const;
    const QString &processPath() const;
    const QString &userName() const;
    const QString &workingSetSize() const;

    const std::vector<std::pair<QString, Thread*>> &threads() const;
    const std::vector<NodeItemComponent*> &components() const;

private:
    QString pid_;
    QString memoryLoad_;
    QString cpuUsage_;
    int status_;
    QString name_;
    QString threads_;
    QString path_;
    QString userName_;
    QString workingSetSize_;

private:
    std::vector<std::pair<QString, Thread*>> vecThreads_;
    std::vector<NodeItemComponent*> vecComponents_;
};
}