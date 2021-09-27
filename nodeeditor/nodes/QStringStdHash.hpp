#pragma once

#include "nodeeditor/nodeeditor_export.h"
#include <QString>
#include <QVariant>
#include <functional>

namespace std
{
template <>
struct NODEEDITOR_EXPORT hash<QString>
{
    inline std::size_t operator()(QString const &s) const
    {
        return qHash(s);
    }
};
} // namespace std
