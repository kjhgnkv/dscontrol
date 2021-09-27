#pragma once

#include "DSHost/Protocol/Impl/Converter.h"
#include "DSHost/Protocol/StandardInternal/Impl/Message.h"
#include "Message.h"

namespace DS {
namespace protocols {
namespace %name% {

class %name%ToStandardInternalConverterImpl : public ProtocolConverterImpl
{
public:
    std::string inputProtocolFactoryName() const;
    std::string inputProtocolFactoryComponentName() const;

    std::string outputProtocolFactoryName() const;
    std::string outputProtocolFactoryComponentName() const;

    bool convertMessage(Message::Ptr pInput, Message::Ptr pOutput);

    ProtocolConverterFeatures::Ptr features() const;
};

inline std::string %name%ToStandardInternalConverterImpl::inputProtocolFactoryName() const
{
    return %name-uppercase%_PROTOCOL_FACTORY_NAME;
}

inline std::string %name%ToStandardInternalConverterImpl::inputProtocolFactoryComponentName() const
{
    return %name-uppercase%_PROTOCOL_FACTORY_COMPONENT_NAME;
}

inline std::string %name%ToStandardInternalConverterImpl::outputProtocolFactoryName() const
{
    return STANDARD_INTERNAL_PROTOCOL_FACTORY_NAME;
}

inline std::string %name%ToStandardInternalConverterImpl::outputProtocolFactoryComponentName() const
{
    return "";
}

inline ProtocolConverterFeatures::Ptr %name%ToStandardInternalConverterImpl::features() const
{
    return NULL;
}

///////////////////////////////////////////////////////////////////////////////

class StandardInternalTo%name%ConverterImpl : public ProtocolConverterImpl
{
public:
    std::string inputProtocolFactoryName() const;
    std::string inputProtocolFactoryComponentName() const;

    std::string outputProtocolFactoryName() const;
    std::string outputProtocolFactoryComponentName() const;

    bool convertMessage(Message::Ptr pInput, Message::Ptr pOutput);

    ProtocolConverterFeatures::Ptr features() const;
};

inline std::string StandardInternalTo%name%ConverterImpl::inputProtocolFactoryName() const
{
    return STANDARD_INTERNAL_PROTOCOL_FACTORY_NAME;
}

inline std::string StandardInternalTo%name%ConverterImpl::inputProtocolFactoryComponentName() const
{
    return "";
}

inline std::string StandardInternalTo%name%ConverterImpl::outputProtocolFactoryName() const
{
    return %name-uppercase%_PROTOCOL_FACTORY_NAME;
}

inline std::string StandardInternalTo%name%ConverterImpl::outputProtocolFactoryComponentName() const
{
    return %name-uppercase%_PROTOCOL_FACTORY_COMPONENT_NAME;
}

inline ProtocolConverterFeatures::Ptr StandardInternalTo%name%ConverterImpl::features() const
{
    return NULL;
}

} } } // namespace DS::protocols::%name%

