#pragma once

#include <QString>
#include <QJsonValue>

namespace monitor
{
class System
{
public:
    void update(const QJsonValue &value);

public:
    const QString &id() const;
    const QString &productName() const;
    const QString &kernelType() const;
    const QString &architecture() const;

private: // System
    QString id_;
    QString productName_;
    QString kernelType_;
    QString architecture_;
};
}
