#include "DSHost/Transport/%name%/Impl/Channel.h"

namespace DS {
namespace transport {
namespace %name% {

TransportImplementationFactoryService::Ptr %name%ChannelImpl::s_pImplementationFactoryService;

bool %name%ChannelImpl::configure(const ParameterMap& config)
{
    Poco::ScopedWriteRWLock lock(_lock);

    // TODO: implement channel configurator here

    return true;
}

bool %name%ChannelImpl::open()
{
    Poco::FastMutex::ScopedLock lock(_startStopMutex);
    _lock.writeLock();

    // TODO: implement channel opening method here

    return true;
}

bool %name%ChannelImpl::close()
{
    Poco::FastMutex::ScopedLock lock(_startStopMutex);
    _lock.writeLock();

    // TODO: implement channel closing method here

    return true;
}

void %name%ChannelImpl::reset()
{
    // TODO: implement channel reset method here
}

int %name%ChannelImpl::send(FIFOBuffer& buffer, const ParameterMap& params)
{
    Poco::ScopedWriteRWLock lock(_lock);

    if (buffer.isEmpty())
        return 0;

    ParameterMap full_params = params;
    for (ParameterMap::const_iterator it = _defaultSendParameters.begin(),
            end = _defaultSendParameters.end(); it != end; it++)
    {
        if (full_params.find(it->first) == full_params.end())
            full_params.insert(std::make_pair(it->first, it->second));
    }

    // TODO: implement channel sending method here

    return 0;
}

int %name%ChannelImpl::receive(FIFOBuffer& buffer, ParameterMap& info)
{
    Poco::ScopedWriteRWLock lock(_lock);

    // TODO: implement channel receiving method here

    return 0;
}

bool %name%ChannelImpl::setCallback(Callback *pCallback)
{
    Poco::ScopedWriteRWLock lock(_lock);

    // TODO: implement callback setter here

    return true;
}

} } } // namespace DS::transport::%name%

