#pragma once

#include <QUuid>

namespace QtNodes
{
class IdentityUnit
{
public:
    IdentityUnit(const QUuid &id = QUuid::createUuid());
    IdentityUnit(const IdentityUnit &other) = default;
    virtual ~IdentityUnit() = default;

    QUuid id() const;
    bool setId(const QUuid &newId);
    IdentityUnit &operator=(const IdentityUnit &rhs);
    bool operator==(const IdentityUnit &rhs);
    bool operator!=(const IdentityUnit &rhs);
    bool operator>=(const IdentityUnit &rhs);
    bool operator<=(const IdentityUnit &rhs);
    bool operator>(const IdentityUnit &rhs);
    bool operator<(const IdentityUnit &rhs);

private:
    QUuid id_;
};
} // namespace QtNodes
