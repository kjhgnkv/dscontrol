#pragma once

#include <QJsonObject>

namespace QtNodes
{
class Serializable
{
public:
    explicit Serializable() = default;

    virtual ~Serializable() = default;

    virtual QJsonObject save() const = 0;

    virtual void restore(const QJsonObject &restoringObject) = 0;
};

} // namespace QtNodes
