#pragma once

#include <QJsonValue>
#include <QString>

namespace monitor
{
class Network
{
public:
    void update(const QJsonValue &value);

public:
    const QString &interface() const;
    const QString &hostName() const;
    const QString &ipAddress() const;

private:
    QString hostName_;
    QString interface_;
    QString ipAddress_;
};
}
