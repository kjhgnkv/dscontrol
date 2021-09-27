#pragma once

#include <QString>

namespace monitor
{
class Thread
{
public:
    explicit Thread() = default;
    explicit Thread(const QString &pid, const QString &cpu);
    ~Thread() = default;

public:
    void update(const QString &pid, const QString &cpu);

public:
    QString pid() const;
    QString cpu() const;

private:
    QString pid_;
    QString cpu_;
};
}