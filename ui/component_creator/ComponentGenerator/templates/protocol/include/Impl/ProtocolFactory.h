#pragma once

#include "DSHost/Protocol/Impl/Factory.h"
#include "Protocol.h"
#include "Poco/RWLock.h"

namespace DS {
namespace protocols {
namespace %name% {

class %name%ProtocolFactoryImpl : public ProtocolFactoryImpl
{
public:
    explicit %name%ProtocolFactoryImpl(Poco::Logger *pLogger);

    std::string name() const;
    std::string componentName() const;

    ParameterMap initialConfiguration() const;
    void setInitialConfiguration(const ParameterMap& config);
    void unsetInitialConfiguration();

    ProtocolImpl* createProtocol(const std::string& name, ProtocolImpl::Id id);

private:
    bool _set;
    ParameterMap _config;
    Poco::Logger *_pLogger;
    mutable Poco::RWLock _lock;
};

inline %name%ProtocolFactoryImpl::%name%ProtocolFactoryImpl(Poco::Logger *pLogger)
    :_set(false), _pLogger(pLogger)
{
}

inline std::string %name%ProtocolFactoryImpl::name() const
{
    return %name-uppercase%_PROTOCOL_FACTORY_NAME;
}

inline std::string %name%ProtocolFactoryImpl::componentName() const
{
    return %name-uppercase%_PROTOCOL_FACTORY_COMPONENT_NAME;
}

inline %name%ProtocolFactoryImpl::ParameterMap %name%ProtocolFactoryImpl::initialConfiguration() const
{
    Poco::ScopedReadRWLock lock(_lock);
    return _config;
}

inline void %name%ProtocolFactoryImpl::setInitialConfiguration(const ParameterMap& config)
{
    Poco::ScopedWriteRWLock lock(_lock);
    _config = config;
    _set = true;
}

inline void %name%ProtocolFactoryImpl::unsetInitialConfiguration()
{
    Poco::ScopedWriteRWLock lock(_lock);
    _config.clear();
    _set = false;
}

inline ProtocolImpl* %name%ProtocolFactoryImpl::createProtocol(const std::string& name, ProtocolImpl::Id id)
{
    Poco::ScopedReadRWLock lock(_lock);

    %name%ProtocolImpl *pProtocolImpl = new %name%ProtocolImpl(name);
    pProtocolImpl->setId(id);

    if (_set)
        pProtocolImpl->configure(_config);
    return pProtocolImpl;
}

} } } // namespace DS::protocols::%name%

