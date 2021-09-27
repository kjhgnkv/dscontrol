#pragma once

#include "DSHost/Protocol/%name%/Defines.h"
#include "DSHost/Protocol/Impl/Message.h"

namespace DS {
namespace protocols {
namespace %name% {

class %name%MessageImpl : public MessageImpl
{
public:
    explicit %name%MessageImpl(const std::string& protocol);

    std::string protocolName() const;
    std::string protocolFactoryName() const;
    std::string protocolFactoryComponentName() const;

    // TODO: declare message implementation interface here

    MessageImpl* clone() const;
    MessageImpl* clone(const std::string& protocol) const;

private:
    std::string _protocol;

    // TODO: define message implementation data here
};

inline %name%MessageImpl::%name%MessageImpl(const std::string& protocol)
    :_protocol(protocol)
{
}

inline std::string %name%MessageImpl::protocolName() const
{
    return _protocol;
}

inline std::string %name%MessageImpl::protocolFactoryName() const
{
    return %name-uppercase%_PROTOCOL_FACTORY_NAME;
}

inline std::string %name%MessageImpl::protocolFactoryComponentName() const
{
    return %name-uppercase%_PROTOCOL_FACTORY_COMPONENT_NAME;
}

inline MessageImpl* %name%MessageImpl::clone() const
{
    return clone(_protocol);
}

inline MessageImpl* %name%MessageImpl::clone(const std::string& protocol) const
{
    %name%MessageImpl *pMsg = new %name%MessageImpl(protocol);

    // TODO: implement message cloning here

    return pMsg;
}

} } } // namespace DS::protocols::%name%

