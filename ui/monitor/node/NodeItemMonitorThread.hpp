#pragma once

#include "models/AbstractNodeItemMonitor.hpp"
#include "Thread/Thread.hpp"

namespace monitor
{
class Thread;
class NodeItemComponent;

class NodeItemMonitorThread : public AbstractNodeItemMonitor
{
public:
    explicit NodeItemMonitorThread();
    explicit NodeItemMonitorThread(const QString &key, const QJsonObject &value);
    ~NodeItemMonitorThread() override;

public:
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QVariant data(const QModelIndex &index, int role) override;

private:
    Thread* thread_;
};
}
