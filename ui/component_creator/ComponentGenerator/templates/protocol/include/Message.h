#pragma once

#include "DSHost/Protocol/Message.h"
#include "Impl/Message.h"

namespace DS {
namespace protocols {
namespace %name% {

class %name%Message : public Message
{
public:
    typedef Poco::AutoPtr<%name%Message> Ptr;

    explicit %name%Message(%name%MessageImpl *pImpl);
    ~%name%Message();

    // TODO: declare message interface here

    Message::Ptr clone() const;
    Message::Ptr clone(const std::string& protocol) const;

    MessageImpl* impl() const;

private:
    %name%MessageImpl *_pImpl;
};

inline %name%Message::%name%Message(%name%MessageImpl *pImpl)
    :_pImpl(pImpl)
{
    poco_check_ptr(pImpl);
}

inline %name%Message::~%name%Message()
{
    delete _pImpl;
}

inline Message::Ptr %name%Message::clone() const
{
    return new %name%Message(dynamic_cast<%name%MessageImpl*>(_pImpl->clone()));
}

inline Message::Ptr %name%Message::clone(const std::string& protocol) const
{
    return new %name%Message(dynamic_cast<%name%MessageImpl*>(_pImpl->clone(protocol)));
}

inline MessageImpl* %name%Message::impl() const
{
    return _pImpl;
}

} } } // namespace DS::protocols::%name%

