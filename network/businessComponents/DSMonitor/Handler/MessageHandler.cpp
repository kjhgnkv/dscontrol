#include <Poco/Delegate.h>
#include <sstream>
#include <Poco/Dynamic/Var.h>

#include "MessageHandler.h"
#include "../Logger.h"

using DS::protocols::RawProtocolFactory;
using DS::protocols::Protocol;
using DS::protocols::Message;
using DS::protocols::RawMessage;

namespace DS
{
namespace dsmonitor
{

MessageHandler::MessageHandler(
Sender::Ptr pSender, Receiver::Ptr pReceiver, ProtocolsImplementationFactoryService::Ptr pImplementationFactoryService, ProtocolService::Ptr pProtoService)
: m_pSender(pSender)
, m_pReceiver(pReceiver)
, m_pImplementationFactoryService(pImplementationFactoryService)
, m_pProtoService(pProtoService)
{
    m_pReceiver->reception +=
    Poco::delegate(this, &MessageHandler::processMessage);
}

MessageHandler::~MessageHandler()
{
    m_pReceiver->reception -=
    Poco::delegate(this, &MessageHandler::processMessage);
}

bool MessageHandler::sendToDSServer(const std::string &msg)
{
    Sender::Dispatch::ParameterMap pmap;
    DS::transport::Error errorId;

    bool result = sendRawMessage(msg.c_str(), msg.size(), DS::dsserver::MessageReciverId::DSMonitorId, pmap, errorId);
    if(!result
       && errorId != NO_ERROR)
    {
        Logger::debug(std::string("cann't send msg to DSControl: ") + msg + ", errId: " + Poco::Dynamic::Var(errorId).toString());
    }

    return result;
}

bool MessageHandler::sendRawMessage(const char* msg
                                    , const std::size_t msgLen
                                    , const MessageReciverId::Enum reciverId
                                    , const Sender::Dispatch::ParameterMap& pmap
                                    , DS::transport::Error& outErrorId)
{
    outErrorId = NO_ERROR;

    RawProtocolFactory::Ptr protocolFactory = m_pImplementationFactoryService->rawProtocolFactory();
    if(!protocolFactory)
    {
        Logger::warning(std::string("cann't get RawProtocolFactory"));
        return false;
    }
    Protocol::Ptr protocol = protocolFactory->createProtocol("");
    if(!protocol)
    {
        Logger::warning(std::string("cann't create RawProtocol"));
        return false;
    }
    Message::Ptr pMsg = protocol->createMessage();
    if(!pMsg)
    {
        Logger::warning(std::string("cann't create RawMessage"));
        return false;
    }
    RawMessage::Ptr rpMsg = pMsg.cast<RawMessage>();
    if(!rpMsg)
    {
        Logger::warning(std::string("cann't cast msg to RawMessage"));
        return false;
    }

    Sender::Dispatch::Ptr dispatch = m_pSender->getDispatch();
    if(!dispatch)
    {
        Logger::warning(std::string("cann't to get Dispatch"));
        return false;
    }

    rpMsg->buffer() = RawMessage::Buffer(msg, msgLen);
    dispatch->setMessage(rpMsg);
    dispatch->setProperties("", reciverId, false, pmap);

    m_pSender->forEachSlave(dispatch, reciverId);

    outErrorId = dispatch->error();

    return dispatch->error() == NO_ERROR;
}

void MessageHandler::processMessage(const Receiver::ReceivedMessage& rm)
{
    std::ostringstream oss;

    if (rm.msg.error != NO_ERROR)
    {
        oss << "processMessage(Error): code=" << rm.msg.error;

        Logger::warning(oss.str());
        return;
    }

    if(!rm.msg.pMessage)
    {
        Logger::warning("pRawMessage is null");
        return;
    }

    ReceivedMsg::Ptr rMsg = ReceivedMsg::create(rm);

    /*TransportChannelWrapper::ParameterMap::const_iterator it = rMsg->info.find(MQTT_PARAM_TOPIC);

    oss << "processMessage(Raw): id=" << rMsg->destinationId
        << ", receiverName=" << rMsg->source
        << ", size=" << rMsg->buffer.size()
        << ", topic=" << (it == rMsg->info.end() ? "" : it->second)
        << ", content=" << std::string(rMsg->buffer.begin(), ((rMsg->buffer.size() > 100) ? 100 : rMsg->buffer.size()));
    Logger::notice(oss.str());*/

    messageAvailable.notify(this, rMsg);
}
}
}
