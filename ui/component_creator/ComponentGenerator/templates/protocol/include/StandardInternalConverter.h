#pragma once

#include "DSHost/Protocol/Converter.h"
#include "Impl/StandardInternalConverter.h"

namespace DS {
namespace protocols {
namespace %name% {

class %name%ToStandardInternalConverter : public ProtocolConverter
{
public:
    typedef Poco::AutoPtr<%name%ToStandardInternalConverter> Ptr;

    explicit %name%ToStandardInternalConverter(%name%ToStandardInternalConverterImpl *pImpl);
    ~%name%ToStandardInternalConverter();

    ProtocolConverterImpl* impl() const;

private:
    %name%ToStandardInternalConverterImpl *_pImpl;
};

inline %name%ToStandardInternalConverter::%name%ToStandardInternalConverter(%name%ToStandardInternalConverterImpl *pImpl)
    :_pImpl(pImpl)
{
    poco_check_ptr(pImpl);
}

inline %name%ToStandardInternalConverter::~%name%ToStandardInternalConverter()
{
    delete _pImpl;
}

inline ProtocolConverterImpl* %name%ToStandardInternalConverter::impl() const
{
    return _pImpl;
}

///////////////////////////////////////////////////////////////////////////////

class StandardInternalTo%name%Converter : public ProtocolConverter
{
public:
    typedef Poco::AutoPtr<StandardInternalTo%name%Converter> Ptr;

    explicit StandardInternalTo%name%Converter(StandardInternalTo%name%ConverterImpl *pImpl);
    ~StandardInternalTo%name%Converter();

    ProtocolConverterImpl* impl() const;

private:
    StandardInternalTo%name%ConverterImpl *_pImpl;
};

inline StandardInternalTo%name%Converter::StandardInternalTo%name%Converter(StandardInternalTo%name%ConverterImpl *pImpl)
    :_pImpl(pImpl)
{
    poco_check_ptr(pImpl);
}

inline StandardInternalTo%name%Converter::~StandardInternalTo%name%Converter()
{
    delete _pImpl;
}

inline ProtocolConverterImpl* StandardInternalTo%name%Converter::impl() const
{
    return _pImpl;
}

} } } // namespace DS::protocols::%name%

