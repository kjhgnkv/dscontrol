#pragma once

#include "models/AbstractNodeItemMonitor.hpp"

class AbstractItemModelMonitor;

namespace monitor
{
class Application;

class NodeItemMonitorApplication : public AbstractNodeItemMonitor
{
public:
    explicit NodeItemMonitorApplication();
    explicit NodeItemMonitorApplication(const QString &key, const QJsonObject &value);
    ~NodeItemMonitorApplication() override;

public:
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QVariant data(const QModelIndex &index, int role) override;

    AbstractItemModelMonitor* modelApp() const;
    void update(const QJsonValue &data);

    Application* application() const;

private:
    Application* application_;
    AbstractItemModelMonitor* modelThreads_;
};
}
