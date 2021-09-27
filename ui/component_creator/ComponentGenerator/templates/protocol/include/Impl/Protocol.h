#pragma once

#include "DSHost/Protocol/%name%/Impl/Protocol.h"
#include "Message.h"
#include "Poco/RWLock.h"

namespace DS {
namespace protocols {
namespace %name% {

class %name%ProtocolImpl : public ProtocolImpl
{
public:
    explicit %name%ProtocolImpl(const std::string& name);

    std::string factoryName() const;
    std::string factoryComponentName() const;

    std::string name() const;

    ParameterMap configuration() const;
    bool configure(const ParameterMap& config);

    Id id() const;
    void setId(Id id);

    MessageImpl* createMessage();

private:
    std::string _name;
    Id _id;
    mutable Poco::RWLock _lock;
};

inline %name%ProtocolImpl::%name%ProtocolImpl(const std::string& name)
    :_name(name)
{
}

inline std::string %name%ProtocolImpl::factoryName() const
{
    return %name-uppercase%_PROTOCOL_FACTORY_NAME;
}

inline std::string %name%ProtocolImpl::factoryComponentName() const
{
    return %name-uppercase%_PROTOCOL_FACTORY_COMPONENT_NAME;
}

inline std::string %name%ProtocolImpl::name() const
{
    return _name;
}

inline %name%ProtocolImpl::ParameterMap %name%ProtocolImpl::configuration() const
{
    return ParameterMap();
}

inline bool %name%ProtocolImpl::configure(const ParameterMap& config)
{
    return true;
}

inline ProtocolImpl::Id %name%ProtocolImpl::id() const
{
    Poco::ScopedReadRWLock lock(_lock);
    return _id;
}

inline void %name%ProtocolImpl::setId(ProtocolImpl::Id id)
{
    Poco::ScopedWriteRWLock lock(_lock);
    _id = id;
}

inline MessageImpl* %name%ProtocolImpl::createMessage()
{
    return new %name%MessageImpl(_name);
}

} } } // namespace DS::protocols::%name%

