#include "EchoBundleObject.h"
#include "DSHost/Bundle/Activator.h"
#include "Poco/Dynamic/Struct.h"
#include <sstream>

using DS::protocols::ProtocolsImplementationFactoryService;
using DS::protocols::Protocol;
using DS::protocols::ProtocolFactory;
using DS::transport::DestinationId;

EchoBundleObject::BUNDLE_OBJECT_CTOR_IMPL(EchoBundleObject),
    _version(pContext->thisBundle()->version().toString()), 
    _contentId(DEFAULT_CONTENT_ID), _flag(1), _stopped(1),
    _hide_warnings(true) // this is too dumb, but ordered by director
{
    std::string compatibility;
    std::string text;

    {
        ParameterMap::const_iterator it;

        it = config.find("content_id");
        if (it != config.end())
        {
            _contentId = it->second;
        }

        it = config.find("text");
        if (it != config.end())
        {
            text = it->second;
        }

        it = config.find("compat");
        if (it != config.end())
        {
            compatibility = it->second;
        }

        it = config.find("logerr");
        if (it != config.end())
        {
            if (it->second != "false")
                _hide_warnings = false;
        }
    }

    Poco::OSP::ServiceRef::Ptr pProtocolsImplFactoryServiceRef = 
        context()->registry().findByName(PROTOCOLS_IMPLEMENTATION_FACTORY_SERVICE_NAME);
    poco_assert(pProtocolsImplFactoryServiceRef);
    ProtocolsImplementationFactoryService::Ptr pProtocolsImplFactoryService = 
        pProtocolsImplFactoryServiceRef->castedInstance<ProtocolsImplementationFactoryService>();

    ProtocolFactory::Ptr pFactory = pProtocolsImplFactoryService->defaultProtocolFactory();

    Protocol::Ptr pProtocol = pFactory->createProtocol("");
    poco_assert(pProtocol);

    _pData = pProtocol->createMessage().cast<DefaultMessage>();
    poco_assert(_pData);

    if (compatibility == "stdbin")
        prepareStdBinCompatibleMessage(text);
    else
        prepareXMLCompatibleMessage(text);

    Poco::OSP::ServiceRef::Ptr pCommServiceRef = 
        context()->registry().findByName(COMMUNICATION_SERVICE_NAME);
    poco_assert(pCommServiceRef);
    _pCommService = pCommServiceRef->castedInstance<CommunicationService>();

    _thread.setName(name);
}

void EchoBundleObject::run()
{
    _stopped = 0;

    Sender::Dispatch::Ptr pDispatch = _pSender->getDispatch();
    bool enough = false;

    while (!_flag)
    {
        _blocker.block(SEND_PERIOD);
        if (_flag)
            break;

        if (isActive() && !enough)
        {
            for (DestinationId id = MIN_DESTINATION_ID; id <= MAX_DESTINATION_ID; id++)
            {
                pDispatch->setMessage(_pData);
                pDispatch->setProperties(_contentId, id);
                _pSender->forEachSlave(pDispatch, id);

                if ((pDispatch->error() != NO_ERROR) || (pDispatch->numberOfSent() > 0))
                {
                    std::ostringstream oss;
                    oss << "run(): { destId = " << id
                        << " -> numberOfSent = " << pDispatch->numberOfSent()
                        << "; error = " << pDispatch->error() << " }";
                    log(oss.str(), pDispatch->error() != NO_ERROR);
                }
            }
            
            // enough = true;
        }
    }

    _stopped = 1;
    Poco::Thread::sleep(100); // Thread::join() failure prevention
}

void EchoBundleObject::onReceive(const Receiver::ReceivedMessage& msg)
{
    std::ostringstream oss;

    oss << "onReceive(): "
        << "\n--------------------------------------------\n"
        << "msg.slaveReceiverName = \"" << msg.slaveReceiverName << "\"; "
        << "msg.fromLocalSource = " << (msg.fromLocalSource ? "true" : "false") << "\n"
        << "msg.msg.aux.source = \"" << msg.msg.aux.source << "\"; " 
        << "msg.msg.aux.destinations = " << stringifyNameList(msg.msg.aux.destinations) << "\n" 
        << "msg.msg.aux.destinationId = " << msg.msg.aux.destinationId << "; " 
        << "msg.msg.aux.contentId = \"" << msg.msg.aux.contentId << "\"; "
        << "msg.msg.aux.logicTime = " << msg.msg.aux.logicTime << "\n";

    oss << "msg.msg.info = { ";
    for (ParameterMap::const_iterator it = msg.msg.info.begin(),
            end = msg.msg.info.end(); it != end; it++)
    {
        oss << "\"" << it->first << "\"=\"" << it->second << "\"; ";
    }
    oss << "}\n";

    if (msg.msg.pMessage)
    {
        oss << "msg.msg.pMessage->protocolName() = " << msg.msg.pMessage->protocolName() << "\n";

        DefaultMessage::Ptr pData = msg.msg.pMessage.cast<DefaultMessage>();
        if (pData)
        {
            oss << "msg.msg.pMessage->data() = " 
                << DefaultMessage::Data::toString(pData->data()) << "\n";
        }
        else
        {
            oss << "> received message of protocol factory \"" << msg.msg.pMessage->protocolFactoryName() << "\", "
                << "component \"" << msg.msg.pMessage->protocolFactoryComponentName() << "\"\n";
        }
    }

    if (msg.msg.pBytesOfMessage)
    {
        size_t len = msg.msg.pBytesOfMessage->buffer().size();
        oss << "msg.msg.pBytesOfMessage->buffer().size() = " << len << "\n";
        oss << "msg.msg.pBytesOfMessage->buffer().begin() = " 
            << std::string(msg.msg.pBytesOfMessage->buffer().begin(), len) << "\n";
    }

    oss << "msg.msg.error = " << msg.msg.error;
    oss << "\n--------------------------------------------";

    log(oss.str(), msg.msg.error != NO_ERROR);
}

void EchoBundleObject::prepareXMLCompatibleMessage(const std::string text)
{
    Poco::Dynamic::Struct<std::string> attr;
    attr["type"] = "text";
    attr["count"] = 1;

    Poco::Dynamic::Struct<std::string> top;
    top["n"] = "Data";
    top["a"] = attr;
    top["v"] = text;

    Poco::Dynamic::Array arr;
    arr.push_back(top);

    _pData->data() = arr;
}

void EchoBundleObject::prepareStdBinCompatibleMessage(const std::string text)
{
    Poco::Dynamic::Array arr;
    arr.push_back("0123456789");
    arr.push_back("012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"); // length = 300

    Poco::Dynamic::Struct<std::string> strct2;
    strct2["10"] = "abc";

    Poco::Dynamic::Struct<std::string> strct;
    strct["3"] = strct2;

    Poco::Dynamic::Struct<std::string> top;
    top["_"] = static_cast<unsigned short>(13);
    top["1"] = text;
    top["2"] = arr;
    top["3"] = strct;

    _pData->data() = top;
}

std::string EchoBundleObject::stringifyNameList(const std::set<std::string>& names)
{
    std::string str;

    std::set<std::string>::const_iterator it = names.begin(), end = names.end();
    if (it != end)
    {
        str = "\"";
        str += *it;
        str += "\"";
        it++;
    }
    for (; it != end; it++)
    {
        str += ",\"";
        str += *it + "\"";
    }

    return str;
}

BUNDLE_ACTIVATOR(EchoBundleObjectActivator, EchoBundleObject);

POCO_BEGIN_MANIFEST(Poco::OSP::BundleActivator)
    POCO_EXPORT_CLASS(EchoBundleObjectActivator)
POCO_END_MANIFEST

