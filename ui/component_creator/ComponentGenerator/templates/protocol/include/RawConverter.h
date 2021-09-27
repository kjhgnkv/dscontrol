#pragma once

#include "DSHost/Protocol/Raw/Converter.h"
#include "Impl/RawConverter.h"

namespace DS {
namespace protocols {
namespace %name% {

class %name%ToRawConverter : public ProtocolConverter
{
public:
    typedef Poco::AutoPtr<%name%ToRawConverter> Ptr;

    explicit %name%ToRawConverter(%name%ToRawConverterImpl *pImpl);
    ~%name%ToRawConverter();

    ProtocolConverterImpl* impl() const;

private:
    %name%ToRawConverterImpl *_pImpl;
};

inline %name%ToRawConverter::%name%ToRawConverter(%name%ToRawConverterImpl *pImpl)
    :_pImpl(pImpl)
{
    poco_check_ptr(pImpl);
}

inline %name%ToRawConverter::~%name%ToRawConverter()
{
    delete _pImpl;
}

inline ProtocolConverterImpl* %name%ToRawConverter::impl() const
{
    return _pImpl;
}

///////////////////////////////////////////////////////////////////////////////

class RawTo%name%Converter : public ProtocolConverter
{
public:
    typedef Poco::AutoPtr<RawTo%name%Converter> Ptr;

    explicit RawTo%name%Converter(RawTo%name%ConverterImpl *pImpl);
    ~RawTo%name%Converter();

    ProtocolConverterImpl* impl() const;

private:
    RawTo%name%ConverterImpl *_pImpl;
};

inline RawTo%name%Converter::RawTo%name%Converter(RawTo%name%ConverterImpl *pImpl)
    :_pImpl(pImpl)
{
    poco_check_ptr(pImpl);
}

inline RawTo%name%Converter::~RawTo%name%Converter()
{
    delete _pImpl;
}

inline ProtocolConverterImpl* RawTo%name%Converter::impl() const
{
    return _pImpl;
}

} } } // namespace DS::protocols::%name%

