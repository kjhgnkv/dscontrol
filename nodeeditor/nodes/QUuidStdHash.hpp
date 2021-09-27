#pragma once

#include "nodeeditor/nodeeditor_export.h"
#include <QUuid>
#include <QVariant>
#include <functional>

namespace std
{
template <>
struct NODEEDITOR_EXPORT hash<QUuid>
{
    inline std::size_t operator()(QUuid const &uid) const
    {
        return qHash(uid);
    }
};
} // namespace std
