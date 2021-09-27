#pragma once

#include "NodeItemMonitorApplication.hpp"

class AbstractItemModelMonitor;

namespace monitor
{
class Computer;

class NodeItemMonitorComputer : public AbstractNodeItemMonitor
{
public:
    explicit NodeItemMonitorComputer() = default;
    explicit NodeItemMonitorComputer(const QString &data);
    ~NodeItemMonitorComputer() override;

public:
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QVariant data(const QModelIndex &index, int role) override;

    AbstractItemModelMonitor* modelApp() const;
    void update(const QJsonValue &data);

    Computer* computer() const;

private:
    Computer* computer_;
    AbstractItemModelMonitor* modelApp_;
};
}