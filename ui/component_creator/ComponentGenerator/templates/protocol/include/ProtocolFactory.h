#pragma once

#include "DSHost/Protocol/Factory.h"
#include "Impl/ProtocolFactory.h"
#include "Protocol.h"

namespace DS {
namespace protocols {
namespace %name% {

class %name%ProtocolFactory : public ProtocolFactory
{
public:
    typedef Poco::AutoPtr<%name%ProtocolFactory> Ptr;

    explicit %name%ProtocolFactory(%name%ProtocolFactoryImpl *pImpl);
    ~%name%ProtocolFactory();

    Protocol::Ptr createProtocol(const std::string& name, Protocol::Id id = 0);

    ProtocolFactoryImpl* impl() const;

private:
    %name%ProtocolFactoryImpl *_pImpl;
};

inline %name%ProtocolFactory::%name%ProtocolFactory(%name%ProtocolFactoryImpl *pImpl)
    :_pImpl(pImpl)
{
    poco_check_ptr(pImpl);
}

inline %name%ProtocolFactory::~%name%ProtocolFactory()
{
    delete _pImpl;
}

inline Protocol::Ptr %name%ProtocolFactory::createProtocol(const std::string& name, Protocol::Id id)
{
    return new %name%Protocol(dynamic_cast<%name%ProtocolImpl*>(_pImpl->createProtocol(name, id)));
}

inline ProtocolFactoryImpl* %name%ProtocolFactory::impl() const
{
    return _pImpl;
}

} } } // namespace DS::protocols::%name%

