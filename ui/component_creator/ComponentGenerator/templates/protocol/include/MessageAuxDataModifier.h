#pragma once

#include "DSHost/Transport/Headerless/MessageAuxDataModifier.h"
#include "Impl/MessageAuxDataModifier.h"

namespace DS {
namespace protocols {
namespace %name% {

class %name%MessageAuxDataModifier : public DS::transport::MessageAuxDataModifier
{
public:
    typedef Poco::AutoPtr<%name%MessageAuxDataModifier> Ptr;

    explicit %name%MessageAuxDataModifier(%name%MessageAuxDataModifierImpl *pImpl);
    ~%name%MessageAuxDataModifier();

    DS::transport::MessageAuxDataModifierImpl* impl() const;

private:
    %name%MessageAuxDataModifierImpl *_pImpl;
};

inline %name%MessageAuxDataModifier::%name%MessageAuxDataModifier(%name%MessageAuxDataModifierImpl *pImpl)
    :_pImpl(pImpl)
{
    poco_check_ptr(pImpl);
}

inline %name%MessageAuxDataModifier::~%name%MessageAuxDataModifier()
{
    delete _pImpl;
}

inline DS::transport::MessageAuxDataModifierImpl* %name%MessageAuxDataModifier::impl() const
{
    return _pImpl;
}

} } } // namespace DS::protocols::%name%

