#pragma once

#include <scopedPtr.h>
#include <Poco/AtomicCounter.h>
#include "DSHost/Event/Service.h"
#include "src/Monitor.h"
#include "DSServerGlobal.h"

#include "Handler/MessageHandler.h"

#include "DSHost/Event/Service.h"
#include <Poco/RWLock.h>
#include <queue>
#include <blocker.h>

using namespace DS::events;

namespace DS
{
namespace dsmonitor
{
class Controller
{
public:
    explicit Controller(Sender::Ptr pSender, Receiver::Ptr pReceiver, EventService::Ptr pEventService,
                        ProtocolsImplementationFactoryService::Ptr pImplementationFactoryService,
                        ProtocolService::Ptr pProtoService);
    ~Controller();

    bool init();
    bool exec();
    void stop();

private:
    void sendMsgToServer();
    void processMessage(const ReceivedMsg::Ptr &rm);

    void onConnectionOpened(Poco::Dynamic::Var &data);
    void onConnectionClosed(Poco::Dynamic::Var &data);

private:
    Poco::AtomicCounter m_isConnectionOpened, m_isStop;

    EventService::Ptr m_pEventService;

    EventService::EventPtr m_pOpenEvent;
    EventService::EventPtr m_pCloseEvent;

    ds::utils::ScopedPtr<MessageHandler> m_messageHandler;
    ds::utils::ScopedPtr< ::dsmonitor::Monitor> m_dsMonitor;

    ds::utils::Blocker m_blocker;

    Poco::RWLock m_lock;
    Poco::Mutex m_mutex;

};
}
}
