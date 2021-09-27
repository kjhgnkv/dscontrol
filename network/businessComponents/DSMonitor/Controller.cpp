#include <Poco/Delegate.h>
#include <Poco/Thread.h>
#include <DSHost/Transport/BroadcasterWithMQTT/Description.h>
#include <DSHost/Transport/MQTT/Defines.h>
#include <sstream>

// #include "Handler/NetworkAccessory.h"

#include "Controller.h"
#include "Logger.h"

using namespace Poco;

namespace DS
{
namespace dsmonitor
{

Controller::Controller(Sender::Ptr pSender, Receiver::Ptr pReceiver, EventService::Ptr pEventService,
                       ProtocolsImplementationFactoryService::Ptr pImplementationFactoryService,
                       ProtocolService::Ptr pProtoService)
: m_isConnectionOpened(0)
, m_isStop(0)
, m_pEventService(pEventService)
, m_messageHandler(new MessageHandler(pSender, pReceiver, pImplementationFactoryService, pProtoService))
, m_dsMonitor(new ::dsmonitor::Monitor())
{
    m_messageHandler->messageAvailable += Poco::delegate(this, &Controller::processMessage);
}

Controller::~Controller()
{
    m_messageHandler->messageAvailable -= Poco::delegate(this, &Controller::processMessage);

    stop();
}

bool Controller::init()
{
    m_pOpenEvent = m_pEventService->event(MQTT_CONNECTION_OPENNED_EVENT);
    m_pCloseEvent = m_pEventService->event(MQTT_CONNECTION_CLOSED_EVENT);

    if (!m_pOpenEvent || !m_pCloseEvent)
    {
        Logger::error(
        std::string("cann't get event: ") + MQTT_CONNECTION_OPENNED_EVENT + " or " + MQTT_CONNECTION_CLOSED_EVENT);
        return false;
    }

    *m_pOpenEvent.get() += delegate(this, &Controller::onConnectionOpened);
    *m_pCloseEvent.get() += delegate(this, &Controller::onConnectionClosed);

    return true;
}

bool Controller::exec()
{
    if (m_isConnectionOpened)
    {
        sendMsgToServer();
    }

    m_blocker.block(m_dsMonitor->mscWaitMS());

    return true;
}

void Controller::stop()
{
    if (m_pOpenEvent)
    {
        *m_pOpenEvent.get() -= delegate(this, &Controller::onConnectionOpened);
        m_pOpenEvent.reset();
    }
    if (m_pCloseEvent)
    {
        *m_pCloseEvent.get() -= delegate(this, &Controller::onConnectionClosed);
        m_pOpenEvent.reset();
    }

    if (m_blocker.isBlocked())
    {
        m_blocker.unblock();
    }

    // TODO
    if (m_dsMonitor)
    {
        m_dsMonitor.reset();
    }

    m_isStop = 1;
}

void Controller::sendMsgToServer()
{
    std::string msg = m_dsMonitor->update();
    m_messageHandler->sendToDSServer(msg);
}

void Controller::processMessage(const ReceivedMsg::Ptr &rm)
{
    std::ostringstream oss;

    if (rm->error != NO_ERROR)
    {
        oss << "processMessage(Error): code=" << rm->error;

        Logger::warning(oss.str());
        return;
    }

    if (rm->buffer().empty())
    {
        Logger::warning("Raw Message is empty");
        return;
    }

    if (m_dsMonitor->parser(std::string(rm->buffer().begin(), rm->buffer().size())))
    {
        Logger::warning("Error parser Parser!");
        return;
    }

    if (m_blocker.isBlocked())
    {
        m_blocker.unblock();
    }

    TransportChannelWrapper::ParameterMap::const_iterator it = rm->info.find(MQTT_PARAM_TOPIC);

    oss << "processMessage(Raw): id=" << rm->destinationId << ", size=" << rm->buffer().size() << ", topic=" << (it == rm->info.end() ? "" : it->second) << ", content=" << std::string(
    rm->buffer().begin(), rm->buffer().size());
    Logger::notice(oss.str());
}

void Controller::onConnectionOpened(Poco::Dynamic::Var &)
{
    m_isConnectionOpened = 1;
}

void Controller::onConnectionClosed(Poco::Dynamic::Var &)
{
    m_isConnectionOpened = 0;
}

}
}
