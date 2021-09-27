#include "SerializableIdentityUnit.h"

using namespace QtNodes;

SerializableIdentityUnit::SerializableIdentityUnit(const QUuid &id)
: QtNodes::Serializable()
, IdentityUnit(id)
{
}

QJsonObject SerializableIdentityUnit::save() const
{
    return {{"id", id().toString()}};
}

void SerializableIdentityUnit::restore(const QJsonObject &restoringObject)
{
    setId(QUuid::fromString(restoringObject["id"].toString()));
}
