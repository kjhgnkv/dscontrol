#pragma once

#include "DSHost/Transport/Headerless/Impl/MessageAuxDataModifier.h"
#include "Message.h"

namespace DS {
namespace protocols {
namespace %name% {

class %name%MessageAuxDataModifierImpl : public DS::transport::MessageAuxDataModifierImpl
{
public:
    std::string protocolName() const;
    std::string protocolFactoryName() const;
    std::string protocolFactoryComponentName() const;

    Message::Ptr insert(Message::Ptr pMessage, const AuxData& aux);

    bool get(Message::Ptr pMessage, AuxData& aux);
    Message::Ptr extract(Message::Ptr pMessage, AuxData& aux);
};

inline std::string %name%MessageAuxDataModifierImpl::protocolName() const
{
    return "";
}

inline std::string %name%MessageAuxDataModifierImpl::protocolFactoryName() const
{
    return %name-uppercase%_PROTOCOL_FACTORY_NAME;
}

inline std::string %name%MessageAuxDataModifierImpl::protocolFactoryComponentName() const
{
    return %name-uppercase%_PROTOCOL_FACTORY_COMPONENT_NAME;
}

} } } // namespace DS::protocols::%name%

