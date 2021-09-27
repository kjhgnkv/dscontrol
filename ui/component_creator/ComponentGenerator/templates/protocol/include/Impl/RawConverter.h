#pragma once

#include "DSHost/Protocol/Impl/Converter.h"
#include "DSHost/Protocol/Raw/Impl/Message.h"
#include "DSHost/Protocol/Raw/Converter.h"
#include "Message.h"

namespace DS {
namespace protocols {
namespace %name% {

class %name%ToRawConverterImpl : public ProtocolConverterImpl
{
public:
    std::string inputProtocolFactoryName() const;
    std::string inputProtocolFactoryComponentName() const;

    std::string outputProtocolFactoryName() const;
    std::string outputProtocolFactoryComponentName() const;

    bool convertMessage(Message::Ptr pInput, Message::Ptr pOutput);

    ProtocolConverterFeatures::Ptr features() const;
};

inline std::string %name%ToRawConverterImpl::inputProtocolFactoryName() const
{
    return %name-uppercase%_PROTOCOL_FACTORY_NAME;
}

inline std::string %name%ToRawConverterImpl::inputProtocolFactoryComponentName() const
{
    return %name-uppercase%_PROTOCOL_FACTORY_COMPONENT_NAME;
}

inline std::string %name%ToRawConverterImpl::outputProtocolFactoryName() const
{
    return RAW_PROTOCOL_FACTORY_NAME;
}

inline std::string %name%ToRawConverterImpl::outputProtocolFactoryComponentName() const
{
    return "";
}

inline ProtocolConverterFeatures::Ptr %name%ToRawConverterImpl::features() const
{
    return NULL;
}

///////////////////////////////////////////////////////////////////////////////

class RawTo%name%ConverterFeatures : public RawToProtocolConverterFeatures
{
public:
    bool findMessageInStream(const FIFOBuffer& buffer, size_t& displ, size_t& size) const;
};

class RawTo%name%ConverterImpl : public ProtocolConverterImpl
{
public:
    RawTo%name%ConverterImpl();

    std::string inputProtocolFactoryName() const;
    std::string inputProtocolFactoryComponentName() const;

    std::string outputProtocolFactoryName() const;
    std::string outputProtocolFactoryComponentName() const;

    bool convertMessage(Message::Ptr pInput, Message::Ptr pOutput);

    ProtocolConverterFeatures::Ptr features() const;

private:
    ProtocolConverterFeatures::Ptr _pFeatures;
};

inline RawTo%name%ConverterImpl::RawTo%name%ConverterImpl()
    :_pFeatures(new RawTo%name%ConverterFeatures)
{
}

inline std::string RawTo%name%ConverterImpl::inputProtocolFactoryName() const
{
    return RAW_PROTOCOL_FACTORY_NAME;
}

inline std::string RawTo%name%ConverterImpl::inputProtocolFactoryComponentName() const
{
    return "";
}

inline std::string RawTo%name%ConverterImpl::outputProtocolFactoryName() const
{
    return %name-uppercase%_PROTOCOL_FACTORY_NAME;
}

inline std::string RawTo%name%ConverterImpl::outputProtocolFactoryComponentName() const
{
    return %name-uppercase%_PROTOCOL_FACTORY_COMPONENT_NAME;
}

inline ProtocolConverterFeatures::Ptr RawTo%name%ConverterImpl::features() const
{
    return _pFeatures;
}

} } } // namespace DS::protocols::%name%

