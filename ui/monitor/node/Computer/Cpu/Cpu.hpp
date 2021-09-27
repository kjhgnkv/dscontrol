#pragma once

#include <QString>
#include <QJsonValue>

namespace monitor
{
class Cpu
{
public:
    void update(const QJsonValue &value);

public:
    const QString &name() const;
    const QString &usage() const;
    const QString &minMaxbHz() const;
    const QString &cores() const;
    const QString &threads() const;
    const QString &caches() const;

private:
    QString name_;
    QString usage_;
    QString minMaxbHz_;
    QString cores_;
    QString threads_;
    QString caches_;
};
}
