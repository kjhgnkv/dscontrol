#pragma once
// #include "DSMiddlewareTypes.h"
#include <Poco/SharedPtr.h>
#include <vector>

class Receiver;

struct ConnectionKey
{
    ConnectionKey() = default;
    int pid_;
    std::vector<Poco::UInt64> ipAddresses_;
};

namespace std
{
template <>
struct hash<::ConnectionKey>
{
    size_t operator()(::ConnectionKey const &toHash) const
    {
        auto ret = std::hash<int>()(toHash.pid_);
        for (const auto &a : toHash.ipAddresses_)
        {
            ret ^= std::hash<Poco::UInt64>()(a);
        }
        return ret;
    }
};
} // namespace std

inline bool operator==(ConnectionKey const &lhs, ConnectionKey const &rhs)
{
    return lhs.pid_ == rhs.pid_ && lhs.ipAddresses_ == rhs.ipAddresses_;
}

inline bool operator!=(const ConnectionKey &lhs, const ConnectionKey &rhs)
{
    return lhs.pid_ != rhs.pid_ && lhs.ipAddresses_ != rhs.ipAddresses_;
}

inline bool operator<(const ConnectionKey &lhs, const ConnectionKey &rhs)
{
    return lhs.pid_ < rhs.pid_ && lhs.ipAddresses_ < rhs.ipAddresses_;
}

class LocalConnection
{
public:
    ~LocalConnection() = default;

    const std::vector<Poco::UInt64> &ipAddresses() const;
    // ds::middleware::TopicType::Enum topicType();
    int pid() const;

    Poco::SharedPtr<Receiver> &receiver();
    static bool parse(const std::string &json, std::vector<Poco::SharedPtr<LocalConnection>> &connections);
    void setReceiver(Poco::SharedPtr<Receiver> &localReceiver);

    // ds::middleware::ApplicationType::Enum appType();
    const ConnectionKey &key() const;

private:
    explicit LocalConnection(int pid, const std::vector<Poco::UInt64> &ipAddresses
                             // , ds::middleware::ApplicationType::Enum appType
                             // , ds::middleware::TopicType::Enum topicType
                             );

private:
    const int pid_;
    const std::vector<Poco::UInt64> ipAddresses_;
    // const ds::middleware::ApplicationType::Enum appType_;
    // const ds::middleware::TopicType::Enum topicType_;
    ConnectionKey key_;
    Poco::SharedPtr<Receiver> localReceiver_;
};

// namespace std
