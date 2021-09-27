#pragma once

#include "models/AbstractNodeItemMonitor.hpp"
#include <QJsonObject>

namespace LevelTreeMonitor
{
enum TYPE
{
    System,
    Computer,
    Application,
    Component,
    Object
};
}

class NodeItemMonitorTree : public AbstractNodeItemMonitor
{
public:
    NodeItemMonitorTree();
    NodeItemMonitorTree(const QString &key, const QString &item1, const QString &item2,
                        const LevelTreeMonitor::TYPE &level);
    NodeItemMonitorTree(const QString &key, const QString &value, const QJsonObject &json,
                        const LevelTreeMonitor::TYPE &level);
    ~NodeItemMonitorTree();

    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    QVariant data(const QModelIndex &index, int role) override;
    const QJsonObject &getJson() const;
    quint32 getLevel() const;
    const QString &getHostName() const;
    const QString &getIp() const;
    const QString &getPid() const;
    const QString &getNameApp() const;

private:
    quint32 level_;
    QString hostName_;
    QString ip_;
    QString pid_;
    QString nameApp_;

private:
    QJsonObject json_;
};
