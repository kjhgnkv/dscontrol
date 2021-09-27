#pragma once

#include "nodeeditor/nodeeditor_export.h"
#include "common/nodeeditor_globals.hpp"

#include <QHash>
#include <QUuid>
#include <utility>

namespace QtNodes
{
struct NODEEDITOR_EXPORT ConnectionID
{
    QUuid lNodeID_ {};
    QUuid rNodeID_ {};
    QUuid sNodeID_ {};

    CommId commId_ {-1};
    MsgId msgId_ {-1};

    QUuid id_ {};

    Device::Type deviceType_ {};
    QString deviceName_ {};
    QString commName_ {};

    bool isValid()
    {
        return !lNodeID_.isNull() && !rNodeID_.isNull() && !id_.isNull() && commId_ != Invalid && msgId_ != Invalid && !commName_.isEmpty();
    }
};

inline bool operator==(ConnectionID const &lhs, ConnectionID const &rhs)
{
    return lhs.lNodeID_ == rhs.lNodeID_ && lhs.rNodeID_ == rhs.rNodeID_ && lhs.sNodeID_ == rhs.sNodeID_ && lhs.commId_ == rhs.commId_ && lhs.deviceName_ == rhs.deviceName_ && lhs.id_ == rhs.id_ && lhs.msgId_ == rhs.msgId_;;
}

inline bool operator!=(const ConnectionID &lhs, const ConnectionID &rhs)
{
    return lhs.lNodeID_ != rhs.lNodeID_ || lhs.rNodeID_ != rhs.rNodeID_ || lhs.sNodeID_ != rhs.sNodeID_ || lhs.commId_ != rhs.commId_ || lhs.deviceName_ != rhs.deviceName_ || lhs.id_ != rhs.id_ || lhs.msgId_ != rhs.msgId_;
}

inline bool operator<(const ConnectionID &lhs, const ConnectionID &rhs)
{
    return lhs.lNodeID_ < rhs.lNodeID_ || lhs.rNodeID_ < lhs.rNodeID_ || lhs.commId_ < rhs.commId_ || lhs.id_ < rhs.id_ || lhs.msgId_ < rhs.msgId_;
}

} // namespace QtNodes

// hash for ConnectionID
namespace std
{
template <>
struct NODEEDITOR_EXPORT hash<::QtNodes::ConnectionID>
{
    size_t operator()(::QtNodes::ConnectionID const &toHash) const
    {
        return qHash(toHash.rNodeID_) ^ qHash(toHash.lNodeID_) ^ qHash(toHash.sNodeID_) ^ qHash(toHash.deviceName_) ^ std::hash<int>()(
        toHash.commId_) ^ qHash(toHash.id_) ^ qHash(toHash.msgId_);
    }
};

} // namespace std
