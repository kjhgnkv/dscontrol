#pragma once

#include "DSHost/Transport/ChannelFactory.h"
#include "Impl/ChannelFactory.h"
#include "Channel.h"

namespace DS {
namespace transport {
namespace %name% {

class %name%ChannelFactory : public TransportChannelFactory
{
public:
    typedef Poco::AutoPtr<%name%ChannelFactory> Ptr;

    explicit %name%ChannelFactory(%name%ChannelFactoryImpl *pImpl);
    ~%name%ChannelFactory();

    TransportChannel::Ptr createChannel(const std::string& name, TransportChannel::Id id = 0);

    TransportChannelFactoryImpl* impl() const;

private:
    %name%ChannelFactoryImpl *_pImpl;
};

inline %name%ChannelFactory::%name%ChannelFactory(%name%ChannelFactoryImpl *pImpl)
    :_pImpl(pImpl)
{
    poco_check_ptr(pImpl);
}

inline %name%ChannelFactory::~%name%ChannelFactory()
{
    delete _pImpl;
}

inline TransportChannel::Ptr %name%ChannelFactory::createChannel(const std::string& name, TransportChannel::Id id)
{
    return new %name%Channel(dynamic_cast<%name%ChannelImpl*>(_pImpl->createChannel(name, id)));
}

inline TransportChannelFactoryImpl* %name%ChannelFactory::impl() const
{
    return _pImpl;
}

} } } // namespace DS::transport::%name%

