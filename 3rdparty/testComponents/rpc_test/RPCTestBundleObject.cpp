#include "RPCTestBundleObject.h"
#include "DSHost/Bundle/Activator.h"
#include "DSHost/Protocol/ImplementationFactoryService.h"
#include "TestProcedure.h"
#include <sstream>

using DS::protocols::ProtocolsImplementationFactoryService;
using DS::protocols::ProtocolFactory;
using DS::protocols::Message;
using DS::protocols::RawMessage;

RPCTestBundleObject::BUNDLE_OBJECT_CTOR_IMPL(RPCTestBundleObject),
    _dontRegister(false), _flag(1), _stopped(1)
{
    {
        ParameterMap::const_iterator it;

        it = config.find("dont_register");
        if (it != config.end())
        {
            if (it->second == "true")
                _dontRegister = true;
        }
    }

    Poco::OSP::ServiceRef::Ptr pProtocolsImplFactoryServiceRef = 
        context()->registry().findByName(PROTOCOLS_IMPLEMENTATION_FACTORY_SERVICE_NAME);
    poco_assert(pProtocolsImplFactoryServiceRef);
    ProtocolsImplementationFactoryService::Ptr pProtocolsImplFactoryService = 
        pProtocolsImplFactoryServiceRef->castedInstance<ProtocolsImplementationFactoryService>();

    ProtocolFactory::Ptr pFactory = pProtocolsImplFactoryService->rawProtocolFactory();

    _pRawProtocol = pFactory->createProtocol(PROTOCOL_NAME).cast<RawProtocol>();
    poco_assert(_pRawProtocol);

    Poco::OSP::ServiceRef::Ptr pRPCServiceRef = 
        context()->registry().findByName(RPC_SERVICE_NAME);
    poco_check_ptr(pRPCServiceRef);
    _pRPCService = pRPCServiceRef->castedInstance<RPCService>();

    Poco::OSP::ServiceRef::Ptr pBundleObjectServiceRef = 
        context()->registry().findByName(BUNDLE_OBJECT_SERVICE_NAME);
    poco_check_ptr(pBundleObjectServiceRef);
    _pBundleObjectService = pBundleObjectServiceRef->castedInstance<BundleObjectService>();

    _pLocalProcedure = new TestProcedure;

    _thread.setName(name);
}

class Printer : public DS::RPC::Procedure::Callback
{
public:
    Printer(Poco::Logger *pLogger)
        :_pLogger(pLogger)
    {}

    void process(Message::Ptr pData);
    void error(Error error);

private:
    Poco::Logger *_pLogger;
};

void Printer::process(Message::Ptr pData)
{
    if (!pData)
    {
        if (_pLogger)
            _pLogger->warning("Callback received NULL data");
        return;
    }

    RawMessage::Ptr pRaw = pData.cast<RawMessage>();
    if (!pRaw)
    {
        if (_pLogger)
            _pLogger->warning("Callback received unknown message type: cast failed");
        return;
    }

    std::istringstream iss(std::string(pRaw->buffer().begin(), pRaw->buffer().size()));
    int value;
    iss >> value;

    std::ostringstream oss;
    oss << "Callback received value " << value;

    if (_pLogger)
        _pLogger->information(oss.str());
}

void Printer::error(Error error)
{
    std::ostringstream oss;
    oss << "Callback received error " << static_cast<int>(error);

    if (_pLogger)
        _pLogger->error(oss.str());
}

void RPCTestBundleObject::run()
{
    _stopped = 0;

    bool deactivated = true;
    RPCBundleObject::Ptr pRPCBundleObject;
    Procedure::Ptr pRemoteProcedure;
    Procedure::Callback::Ptr pPrinter = new Printer(&context()->logger());

    int value = 0;
    RawMessage::Ptr pRaw;

    while (!_flag)
    {
        _blocker.block(SEND_PERIOD);

        if (isActive())
        {
            if (deactivated)
            {
                pRPCBundleObject = _pBundleObjectService->object(RPC_BUNDLE_OBJECT_NAME).
                    cast<RPCBundleObject>();
                if (pRPCBundleObject)
                    pRPCBundleObject->requestSynchronization(PORT_NAME);
                deactivated = false;
            }

            if (!pRemoteProcedure)
            {
                if (pRPCBundleObject)
                    pRemoteProcedure = 
                        pRPCBundleObject->remoteProcedure(PORT_NAME, _pLocalProcedure->name());

                if (!pRemoteProcedure)
                    continue;
            }

            {
                std::ostringstream oss;
                oss << "Calling remote procedure with value " << value;
                log(oss.str());
            }

            {
                pRaw = _pRawProtocol->createMessage().cast<RawMessage>();
                std::ostringstream oss;
                oss << value;
                pRaw->buffer().assign(oss.str().c_str(), oss.str().length());
            }

            pRemoteProcedure->call(pRaw, pPrinter);
            value += 3;
        }
        else
        {
            deactivated = true;
            pRemoteProcedure.reset();
            pRPCBundleObject.reset();
        }
    }

    _stopped++;
    Poco::Thread::sleep(100); // Thread::join() failure prevention
}

BUNDLE_ACTIVATOR(RPCTestBundleObjectActivator, RPCTestBundleObject);

POCO_BEGIN_MANIFEST(Poco::OSP::BundleActivator)
    POCO_EXPORT_CLASS(RPCTestBundleObjectActivator)
POCO_END_MANIFEST

