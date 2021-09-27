#pragma once

#include "DSHost/Transport/Channel.h"
#include "Impl/Channel.h"

namespace DS {
namespace transport {
namespace %name% {

class %name%Channel : public TransportChannel
{
public:
    typedef Poco::AutoPtr<%name%Channel> Ptr;

    explicit %name%Channel(%name%ChannelImpl *pImpl);
    ~%name%Channel();

    // TODO: declare channel interface here

    TransportChannelImpl* impl() const;

private:
    %name%ChannelImpl *_pImpl;
};

inline %name%Channel::%name%Channel(%name%ChannelImpl *pImpl)
    :_pImpl(pImpl)
{
    poco_check_ptr(pImpl);
}

inline %name%Channel::~%name%Channel()
{
    delete _pImpl;
}

inline TransportChannelImpl* %name%Channel::impl() const
{
    return _pImpl;
}

} } } // namespace DS::transport::%name%

