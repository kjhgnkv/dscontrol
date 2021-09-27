#pragma once

#include "IdentityUnit.h"
#include "nodes/Serializable.hpp"

namespace QtNodes
{
class SerializableIdentityUnit : public QtNodes::Serializable, public IdentityUnit
{
public:
    SerializableIdentityUnit(const QUuid &id = QUuid::createUuid());
    ~SerializableIdentityUnit() override = default;

    QJsonObject save() const override;
    void restore(const QJsonObject &restoringObject) override;
};
} // namespace QtNodes
