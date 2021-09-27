#pragma once

#include "DSHost/Protocol/Protocol.h"
#include "Impl/Protocol.h"
#include "Message.h"

namespace DS {
namespace protocols {
namespace %name% {

class %name%Protocol : public Protocol
{
public:
    typedef Poco::AutoPtr<%name%Protocol> Ptr;

    explicit %name%Protocol(%name%ProtocolImpl *pImpl);
    ~%name%Protocol();

    Message::Ptr createMessage();

    ProtocolImpl* impl() const;

private:
    %name%ProtocolImpl *_pImpl;
};

inline %name%Protocol::%name%Protocol(%name%ProtocolImpl *pImpl)
    :_pImpl(pImpl)
{
    poco_check_ptr(pImpl);
}

inline %name%Protocol::~%name%Protocol()
{
    delete _pImpl;
}

inline Message::Ptr %name%Protocol::createMessage()
{
    return new %name%Message(dynamic_cast<%name%MessageImpl*>(_pImpl->createMessage()));
}

inline ProtocolImpl* %name%Protocol::impl() const
{
    return _pImpl;
}

} } } // namespace DS::protocols::%name%


