#include <Poco/Delegate.h>
#include <sstream>
#include <Poco/Dynamic/Var.h>

#include <DSServerGlobal.h>

#include "MessageHandler.h"
#include "../Logger.h"

using DS::protocols::RawProtocolFactory;
using DS::protocols::Protocol;
using DS::protocols::Message;
using DS::protocols::RawMessage;

namespace DS {
namespace example {
namespace dsmonitor {

MessageHandler::MessageHandler(
                Sender::Ptr pSender
                , Receiver::Ptr pReceiver
                , ProtocolsImplementationFactoryService::Ptr pImplementationFactoryService
                , ProtocolService::Ptr pProtoService)
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

bool MessageHandler::sendToDSServer(const std::string& msg)
{
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

    Sender::Dispatch::ParameterMap pmap;
    rpMsg->buffer() = RawMessage::Buffer(msg.c_str(), msg.length());
    dispatch->setMessage(rpMsg);
    dispatch->setProperties("", DS::dsserver::MessageReciverId::DSMonitorId, false, pmap);

    m_pSender->forEachSlave(dispatch, DS::dsserver::MessageReciverId::DSMonitorId);

    if(dispatch->error() != NO_ERROR)
        Logger::warning(std::string("cann't send msg to DSControl: ") + msg + ", errId: " + Poco::Dynamic::Var(dispatch->error()).toString());

    return dispatch->error() == NO_ERROR;
}

void MessageHandler::processMessage(const Receiver::ReceivedMessage& rd)
{
    std::ostringstream oss;

    if (rd.msg.error != NO_ERROR)
    {
        oss << "processMessage(Error): code=" << rd.msg.error;

        Logger::warning(oss.str());
        return;
    }

    oss << "processMessage(Raw): id=" << rd.msg.aux.destinationId << ", receiverName="
        << rd.msg.aux.source;/* << ", size=" << rd.buffer.size() << ", topic=" << rd.tp.target
        << ", content=" << std::string(rd.buffer.begin(), rd.buffer.size());*/
    Logger::notice(oss.str());

    packetAvailable.notify(this, rd);
}
}}}
