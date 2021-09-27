#pragma once

#include <QString>
#include <QJsonValue>

namespace monitor
{
class Memory
{
public:
    void update(const QJsonValue &value);

public:
    const QString &load() const;
    const QString &total() const;
    const QString &free() const;
    const QString &used() const;

private:
    QString load_;
    QString total_;
    QString free_;
    QString used_;
};
}
