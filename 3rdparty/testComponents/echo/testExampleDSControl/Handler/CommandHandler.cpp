#include "CommandHandler.h"
#include "../Logger.h"

#include <DSServerGlobal.h>
#include <Poco/Delegate.h>

#include "../Commands/ReceiveComponentCommand.h"
#include "../Commands/ReceiveApplicationTemplateCommand.h"
#include "../Commands/SetDSControlDataCommand.h"
#include "../Commands/GetDSControlDataCommand.h"
#include "../Commands/DeployCommand.h"
#include "../Commands/UpdateCommand.h"
#include "../Commands/StopCommand.h"


namespace DS {
namespace example {

const long CommandHandler::msc_waitMS = 1000;

CommandHandler::CommandHandler(Sender::Ptr pSender
                               , Receiver::Ptr pReceiver
                               , ProtocolService::Ptr pProtoService)
    : m_messageHandler(new MessageHandler(pSender, pReceiver, pProtoService))
    , m_msgId(0)
{
    m_messageHandler->packetAvailable +=
        Poco::delegate(this, &CommandHandler::processPacket);
}

CommandHandler::~CommandHandler()
{
    m_messageHandler->packetAvailable -=
        Poco::delegate(this, &CommandHandler::processPacket);
}

bool CommandHandler::init()
{
    return true;
}

bool CommandHandler::exec()
{
    std::set<std::string> ipAddresses;
    {
        Poco::ScopedWriteRWLock lock(m_ipAddressLock);

        ipAddresses = m_ipAddress;
    }

    if(ipAddresses.empty())
        return true;

    for(std::set<std::string>::iterator itIp = ipAddresses.begin();
        itIp != ipAddresses.end();
        itIp++)
    {
        {
            ReceiveApplicationTemplateCommand receiveAppTemplateCommand(m_messageHandler);
            while(!receiveAppTemplateCommand.isFinished())
            {
                std::queue<Poco::SharedPtr<IncomingMsg> > lastMsgs;
                {
                    Poco::ScopedWriteRWLock lock(m_lastMsgsLock);
                    std::swap(m_lastMsgs, lastMsgs);
                }
                if (!receiveAppTemplateCommand.process(m_msgId++, *itIp, lastMsgs))
                {
                    Logger::warning("-----> ReceiveApplicationTemplateCommand: receiveAppTemplateCommand.process == false");
                    return true;
                }
            }
        }

        Poco::Thread::sleep(1000);

        {
            ReceiveComponentCommand receiveComponentCommand(m_messageHandler);
            while(!receiveComponentCommand.isFinished())
            {
                std::queue<Poco::SharedPtr<IncomingMsg> > lastMsgs;
                {
                    Poco::ScopedWriteRWLock lock(m_lastMsgsLock);
                    std::swap(m_lastMsgs, lastMsgs);
                }
                if (!receiveComponentCommand.process(m_msgId++, *itIp, lastMsgs))
                {
                    Logger::warning("-----> ReceiveComponentCommand: receiveAppTemplateCommand.process == false");
                    return true;
                }
            }
        }

        Poco::Thread::sleep(1000);

        {
            SetDSControlDataCommand setDSControlDataCommand(m_messageHandler);
            setDSControlDataCommand.process(m_msgId++, *itIp);
        }

        {
            GetDSControlDataCommand getDSControlDataCommand(m_messageHandler);
            getDSControlDataCommand.process(m_msgId++, *itIp);
        }

        Poco::Thread::sleep(1000);

        {
            DeployCommand deployCommand(m_messageHandler);
            deployCommand.process(m_msgId++, *itIp);
        }

        Poco::Thread::sleep(3000);

        {
            UpdateCommand updateCommand(m_messageHandler);
            updateCommand.process(m_msgId++, *itIp);
        }

        Poco::Thread::sleep(3000);

        {
            StopCommand stopCommand(m_messageHandler);
            stopCommand.process(m_msgId++, *itIp);
        }

        Poco::Thread::sleep(5000);
    }
    return true;
}

void CommandHandler::processPacket(const Receiver::ReceivedData& rd)
{
    Poco::ScopedWriteRWLock lock(m_lastMsgsLock);
    if(rd.buffer.empty())
        return;
    addIpAddress(rd);
    Poco::SharedPtr<IncomingMsg> msg = IncomingMsg::parse(rd.buffer);
    if(!msg)
    {
        Logger::warning(std::string("can not parse raw message: ")
            + (!rd.buffer.empty() ? "NULL" : (
                "id=" + Poco::Dynamic::Var(rd.destinationId).toString()
                + ", topic=" + rd.tp.target
                + ", buffer size=" + Poco::Dynamic::Var(rd.buffer.size()).toString())));
        return;
    }
    if(DS::dsserver::ApplicationType::fromString(msg->appType) == DS::dsserver::ApplicationType::DSMonitor)
        return;

    m_lastMsgs.push(msg);
}

void CommandHandler::addIpAddress(const Receiver::ReceivedData& rd)
{
    Poco::ScopedWriteRWLock lock(m_ipAddressLock);

    if(rd.tp.target.empty())
        return;

    std::size_t pos = rd.tp.target.find_last_of("/");
    if(pos == std::string::npos)
        return;

    std::string ipAddress = rd.tp.target.substr(pos+1);
    if(ipAddress.empty())
        return;

    std::set<std::string>::iterator itIp = m_ipAddress.find(ipAddress);
    if(itIp != m_ipAddress.end())
        return;

    m_ipAddress.insert(ipAddress);
}
}}
