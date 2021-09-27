#include <Poco/Delegate.h>
#include <sstream>
#include <DSServerGlobal.h>

#include "MessageHandler.h"
#include "../Logger.h"

namespace DS {
namespace example {

MessageHandler::MessageHandler(
                Sender::Ptr pSender
                , Receiver::Ptr pReceiver
                , ProtocolService::Ptr pProtoService)
    : m_pSender(pSender)
    , m_pReceiver(pReceiver)
    , m_pProtoService(pProtoService)
{
    m_pReceiver->reception +=
        Poco::delegate(this, &MessageHandler::processPacket);
}

MessageHandler::~MessageHandler()
{
    m_pReceiver->reception -=
        Poco::delegate(this, &MessageHandler::processPacket);
}

bool MessageHandler::sendToDSServer(const std::vector<char>& msg, const std::string& ipAddress)
{
    Sender::BufferDispatch::Ptr bDispatch = m_pSender->getBufferDispatch();
    if(!bDispatch)
    {
        Logger::warning(std::string("cann't to get BufferDispatch"));
        return false;
    }

    Sender::TargetSet tSet;
    tSet.insert(ipAddress);

    Sender::Buffer buffer(msg.data(), msg.size());
    bDispatch->setBuffer(buffer);
    bDispatch->setProperties("", DS::dsserver::MessageReciverId::DSControl_DSServer, tSet);
    m_pSender->forEachSlave(bDispatch, DS::dsserver::MessageReciverId::DSControl_DSServer);

    if(bDispatch->error() != NO_ERROR)
        Logger::warning(std::string("cann't send msg to DSServer: ") + msg.data() + ", errId: " + Poco::Dynamic::Var(bDispatch->error()).toString());

    return bDispatch->error() == NO_ERROR;
}

void MessageHandler::processPacket(const Receiver::ReceivedData& rd)
{
    std::ostringstream oss;

    if (rd.error != NO_ERROR)
    {
        oss << "processMessage(Error): code=" << rd.error;

        Logger::warning(oss.str());
        return;
    }


    oss << "processMessage(Raw): id=" << rd.destinationId << ", receiverName="
        << rd.slaveReceiverName << ", size=" << rd.buffer.size() << ", topic=" << rd.tp.target
        << ", content=" << std::string(rd.buffer.begin(), ((rd.buffer.size() > 100) ? 100 : rd.buffer.size()));
    Logger::notice(oss.str());

    packetAvailable.notify(this, rd);
}
}}
