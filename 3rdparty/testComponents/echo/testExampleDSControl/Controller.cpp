#include <Poco/Delegate.h>
#include <Description.h>
#include <Poco/Thread.h>

#include "Controller.h"
#include "Logger.h"

using namespace Poco;

namespace DS {
namespace example {

Controller::Controller(Sender::Ptr pSender
                       , Receiver::Ptr pReceiver
                       , EventService::Ptr pEventService
                       , ProtocolService::Ptr pProtoService)
    : m_commandHandler(new CommandHandler(pSender, pReceiver, pProtoService))
    , m_pEventService(pEventService)
    , m_isStop(0)
{ }

Controller::~Controller()
{
    if(m_pOpenEvent)
        *m_pOpenEvent.get() -= delegate(this, &Controller::onConnectionOpened);
    if(m_pCloseEvent)
        *m_pCloseEvent.get() -= delegate(this, &Controller::onConnectionClosed);
}

bool Controller::init()
{
    m_pOpenEvent = m_pEventService->event(MQTT_CONNECTION_OPENNED_EVENT);
    m_pCloseEvent = m_pEventService->event(MQTT_CONNECTION_CLOSED_EVENT);
    if(!m_pOpenEvent || !m_pCloseEvent)
    {
        Logger::error(std::string("cann't get event: ") + MQTT_CONNECTION_OPENNED_EVENT + " or " + MQTT_CONNECTION_CLOSED_EVENT);
        return false;
    }

    *m_pOpenEvent.get() += delegate(this, &Controller::onConnectionOpened);
    *m_pCloseEvent.get() += delegate(this, &Controller::onConnectionClosed);

    return m_commandHandler->init();
}

bool Controller::exec()
{
    if(m_isStop)
        return false;

    if(!m_isConnectionOpened)
    {
        Poco::Thread::sleep(500);
        return true;
    }

    return m_commandHandler->exec();
}

void Controller::stop()
{
    m_isStop = 1;
}

void Controller::onConnectionOpened(Poco::Dynamic::Var&)
{
    m_isConnectionOpened = 1;
}

void Controller::onConnectionClosed(Poco::Dynamic::Var&)
{
    m_isConnectionOpened = 0;
}

}}
