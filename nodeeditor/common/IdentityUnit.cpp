#include "IdentityUnit.h"

using namespace QtNodes;

IdentityUnit::IdentityUnit(const QUuid &id)
: id_(id)
{
    if (id_.isNull())
    {
        id_ = QUuid::createUuid();
    }
}

bool IdentityUnit::operator<(const IdentityUnit &rhs)
{
    return this->id_ < rhs.id_;
}

QUuid IdentityUnit::id() const
{
    return id_;
}

bool IdentityUnit::setId(const QUuid &newId)
{
    if (newId != id_)
    {
        id_ = newId;
        return true;
    }
    return false;
}

IdentityUnit &IdentityUnit::operator=(const IdentityUnit &rhs)
{
    if (this != &rhs)
    {
        this->setId(rhs.id());
    }

    return *this;
}

bool IdentityUnit::operator>(const IdentityUnit &rhs)
{
    return this->id_ > rhs.id_;
}

bool IdentityUnit::operator<=(const IdentityUnit &rhs)
{
    return this->id_ <= rhs.id_;
}

bool IdentityUnit::operator>=(const IdentityUnit &rhs)
{
    return this->id_ >= rhs.id_;
}

bool IdentityUnit::operator!=(const IdentityUnit &rhs)
{
    return this->id_ != rhs.id_;
}

bool IdentityUnit::operator==(const IdentityUnit &rhs)
{
    return this->id_ == rhs.id_;
}
