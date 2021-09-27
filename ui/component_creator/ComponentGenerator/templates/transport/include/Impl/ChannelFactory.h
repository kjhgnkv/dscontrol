#pragma once

#include "DSHost/Transport/Impl/ChannelFactory.h"
#include "Poco/RWLock.h"
#include "Channel.h"

namespace DS {
namespace transport {
namespace %name% {

class %name%ChannelFactoryImpl : public TransportChannelFactoryImpl
{
public:
    %name%ChannelFactoryImpl(
            TransportImplementationFactoryService::Ptr pImplementationFactoryService,
            Poco::Logger *pLogger);

    std::string name() const;
    std::string componentName() const;

    ParameterMap initialConfiguration() const;
    void setInitialConfiguration(const ParameterMap& config);
    void unsetInitialConfiguration();

    TransportChannelImpl* createChannel(const std::string& name, TransportChannelImpl::Id id);

private:
    TransportImplementationFactoryService::Ptr _pImplementationFactoryService;
    bool _set;
    ParameterMap _config;
    Poco::Logger *_pLogger;
    mutable Poco::RWLock _lock;
};

inline %name%ChannelFactoryImpl::%name%ChannelFactoryImpl(
        TransportImplementationFactoryService::Ptr pImplementationFactoryService,
        Poco::Logger *pLogger)
    :_pImplementationFactoryService(pImplementationFactoryService),
    _set(false), _pLogger(pLogger)
{
    poco_check_ptr(pImplementationFactoryService);
}

inline std::string %name%ChannelFactoryImpl::name() const
{
    return %name-uppercase%_CHANNEL_FACTORY_NAME;
}

inline std::string %name%ChannelFactoryImpl::componentName() const
{
    return %name-uppercase%_CHANNEL_FACTORY_COMPONENT_NAME;
}

inline %name%ChannelFactoryImpl::ParameterMap %name%ChannelFactoryImpl::initialConfiguration() const
{
    Poco::ScopedReadRWLock lock(_lock);
    return _config;
}

inline void %name%ChannelFactoryImpl::setInitialConfiguration(const ParameterMap& config)
{
    Poco::ScopedWriteRWLock lock(_lock);
    _config = config;
    _set = true;
}

inline void %name%ChannelFactoryImpl::unsetInitialConfiguration()
{
    Poco::ScopedWriteRWLock lock(_lock);
    _config.clear();
    _set = false;
}

TransportChannelImpl* %name%ChannelFactoryImpl::createChannel(const std::string& name, TransportChannelImpl::Id id)
{
    Poco::ScopedReadRWLock lock(_lock);

    %name%ChannelImpl *pChannelImpl = new %name%ChannelImpl(name, _pLogger);
    pChannelImpl->setId(id);

    if (_set)
        pChannelImpl->configure(_config);
    return pChannelImpl;
}

} } } // namespace DS::transport::%name%

