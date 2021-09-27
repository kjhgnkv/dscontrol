#pragma once

#include "DSHost/Transport/%name%/Defines.h"
#include "DSHost/Transport/Impl/Channel.h"
#include "DSHost/Transport/ImplementationFactoryService.h"
#include "Poco/Logger.h"
#include "Poco/RWLock.h"
#include "Poco/Mutex.h"

class %name%TransportComponentActivator;

namespace DS {
namespace transport {
namespace %name% {

class %name%ChannelImpl : public TransportChannelImpl
{
public:
    %name%ChannelImpl(const std::string& name, Poco::Logger *pLogger);

    std::string factoryName() const;
    std::string factoryComponentName() const;

    std::string name() const;

    ParameterMap configuration() const;
    bool configure(const ParameterMap& config);

    bool isStreaming() const;

    ParameterMap defaultSendParameters() const;
    void setDefaultSendParameters(const ParameterMap& params);

    Id id() const;
    void setId(Id id);

    bool isOpening() const;
    bool isOpen() const;
    bool isClosed() const;

    bool open();
    bool close();

    void reset();

    int send(FIFOBuffer& buffer, const ParameterMap& params);
    int receive(FIFOBuffer& buffer, ParameterMap& info);

    Callback* callback() const;
    bool setCallback(Callback* pCallback);

    // TODO: declare other methods here

private:
    std::string _name;
    ParameterMap _config;
    ParameterMap _defaultSendParameters;
    Id _id;

    Callback *_pCallback;

    Poco::Logger *_pLogger;
    mutable Poco::RWLock _lock;
    Poco::FastMutex _startStopMutex;

    static TransportImplementationFactoryService::Ptr s_pImplementationFactoryService;

    friend class ::%name%TransportComponentActivator;
};

inline %name%ChannelImpl::%name%ChannelImpl(const std::string& name,
        Poco::Logger *pLogger)
    :_name(name), _id(0), _pCallback(NULL), _pLogger(pLogger)
{
}

inline std::string %name%ChannelImpl::factoryName() const
{
    return %name-uppercase%_CHANNEL_FACTORY_NAME;
}

inline std::string %name%ChannelImpl::factoryComponentName() const
{
    return %name-uppercase%_CHANNEL_FACTORY_COMPONENT_NAME;
}

inline std::string %name%ChannelImpl::name() const
{
    return _name;
}

inline %name%ChannelImpl::ParameterMap %name%ChannelImpl::configuration() const
{
    Poco::ScopedReadRWLock lock(_lock);
    return _config;
}

inline bool %name%ChannelImpl::isStreaming() const
{
    // TODO: implement 'streaming' flag getter here
    return false;
}

inline %name%ChannelImpl::ParameterMap %name%ChannelImpl::defaultSendParameters() const
{
    Poco::ScopedReadRWLock lock(_lock);
    return _defaultSendParameters;
}

inline void %name%ChannelImpl::setDefaultSendParameters(const ParameterMap& params)
{
    Poco::ScopedWriteRWLock lock(_lock);
    _defaultSendParameters = params;
}

inline TransportChannelImpl::Id %name%ChannelImpl::id() const
{
    Poco::ScopedReadRWLock lock(_lock);
    return _id;
}

inline void %name%ChannelImpl::setId(TransportChannelImpl::Id id)
{
    Poco::ScopedWriteRWLock lock(_lock);
    _id = id;
}

inline bool %name%ChannelImpl::isOpening() const
{
    // TODO: implement 'opening' status checker here
    return false;
}

inline bool %name%ChannelImpl::isOpen() const
{
    // TODO: implement 'open' status checker here
    return false;
}

inline bool %name%ChannelImpl::isClosed() const
{
    // TODO: implement 'closed' status checker here
    return false;
}

inline TransportChannelImpl::Callback* %name%ChannelImpl::callback() const
{
    Poco::ScopedReadRWLock lock(_lock);
    return _pCallback;
}

} } } // namespace DS::transport::%name%

