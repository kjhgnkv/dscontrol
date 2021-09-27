#pragma once

#include <QString>
#include <QJsonValue>

namespace monitor
{
class Storage
{
public:
    void update(const QJsonValue &value);

public:
    const QString &load() const;
    const QString &free() const;
    const QString &total() const;
    const QString &used() const;
    const QString &device() const;
    const QString &type() const;
    const QString &path() const;

private: // Storage
    QString load_;
    QString free_;
    QString total_;
    QString used_;
    QString device_;
    QString type_;
    QString path_;
};
}
