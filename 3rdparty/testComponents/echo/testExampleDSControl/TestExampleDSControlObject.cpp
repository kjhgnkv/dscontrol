#include "Poco/Runnable.h"
#include "Poco/AtomicCounter.h"
#include "Poco/Thread.h"
#include "Poco/ThreadPool.h"
#include "Poco/Delegate.h"
#include "Poco/OSP/SystemEvents.h"

#include "DSHost/Bundle/Object.h"
#include "DSHost/Communication/Service.h"
#include "DSHost/Communication/Sender.h"
#include "DSHost/Communication/Receiver.h"
#include "DSHost/Event/Service.h"
#include "DSHost/Bundle/Activator.h"
#include "DSHost/Protocol/Service.h"

#include "Controller.h"
#include "Logger.h"
#include <vector>

using DS::communication::CommunicationService;

using DS::bundle_objects::BundleObject;
using DS::communication::CommunicationService;
using DS::communication::Sender;
using DS::communication::Receiver;
using DS::protocols::ProtocolService;

using DS::events::EventService;
using DS::example::Logger;
using DS::example::Controller;

#define DEFAULT_CONTENT_ID "DSControl"

class TestExampleDSControlObject : public BundleObject, public Poco::Runnable
{
public:
    BUNDLE_OBJECT_CTOR_DECL(TestExampleDSControlObject)
        :DS::bundle_objects::BundleObject(name, pContext, parameters)
        , m_version(pContext->thisBundle()->version().toString())
        , m_contentId(DEFAULT_CONTENT_ID)
        , m_isRunning(false)
        , m_flag(1)
        , m_stopped(1)
    {
        Logger::init(pContext, name, m_version);

        Poco::OSP::ServiceRef::Ptr pCommServiceRef =
            context()->registry().findByName(COMMUNICATION_SERVICE_NAME);
        if (!pCommServiceRef)
        {
            Logger::error("Reference of CommunicationService is not available.");
            return;
        }
        m_pCommService = pCommServiceRef->castedInstance<CommunicationService>();
        if(!m_pCommService)
        {
            Logger::error("CommunicationService is not available.");
            return;
        }

        Poco::OSP::ServiceRef::Ptr pEventServiceRef =
            context()->registry().findByName(EVENT_SERVICE_NAME);
        if (!pEventServiceRef)
        {
            Logger::error("Reference of EventService is not available.");
            return;
        }
        m_pEventService =
            pEventServiceRef->castedInstance<EventService>();
        if(!m_pEventService)
        {
            Logger::error("Event service is not available.");
            return;
        }

        Poco::OSP::ServiceRef::Ptr pProtoServiceRef =
            context()->registry().findByName(PROTOCOL_SERVICE_NAME);
        if (!pProtoServiceRef)
        {
            Logger::error("Reference of ProtoService is not available.");
            return;
        }
        m_pProtoService = pProtoServiceRef->castedInstance<ProtocolService>();
        if(!m_pProtoService)
        {
            Logger::error("ProtoService service is not available.");
            return;
        }

        m_thread.setName(name);
    }

    ~TestExampleDSControlObject()
    {
        stopSender();
    }

    void onInitialize()
    {
        Logger::information("onInitialize()");

        m_pSender = m_pCommService->sender(name());
        m_pReceiver = m_pCommService->receiver(name());
    }

    void onStart()
    {
        Logger::information("onStart()");

        startSender();
    }

    void onStop()
    {
        Logger::information("onStop()");

        stopSender();
    }

    void onUninitialize()
    {
        Logger::information("onUninitialize()");

        m_pReceiver.reset();
        m_pSender.reset();
    }

    void startSender()
    {
        if (m_isRunning)
            return;

        if(!m_pSender || !m_pReceiver)
        {
            Logger::error("MessageSender or MessageReceiver is not available.");
            return;
        }

        m_controller.reset(new Controller(m_pSender, m_pReceiver, m_pEventService, m_pProtoService));
        if(!m_controller->init())
            return;

        m_isRunning = true;
        m_thread.start(*this);
    }

    void stopSender()
    {
        if (!m_isRunning)
            return;

        if(m_controller)
            m_controller->stop();

        m_isRunning = false;

        m_flag++;
        m_thread.join();
    }

    void run()
    {
        m_flag = m_stopped = 0;
        while (!m_flag && m_controller->exec());
        m_stopped++;
    }

private:
    std::string m_version;
    std::string m_contentId;

    CommunicationService::Ptr m_pCommService;
    ProtocolService::Ptr m_pProtoService;

    Sender::Ptr m_pSender;
    Receiver::Ptr m_pReceiver;
    EventService::Ptr m_pEventService;

    ds::utils::ScopedPtr<Controller> m_controller;

    bool m_isRunning;
    Poco::Thread m_thread;
    Poco::AtomicCounter m_flag, m_stopped;
};

BUNDLE_ACTIVATOR(TestExampleDSControlBundleActivator, TestExampleDSControlObject);

POCO_BEGIN_MANIFEST(Poco::OSP::BundleActivator)
    POCO_EXPORT_CLASS(TestExampleDSControlBundleActivator)
POCO_END_MANIFEST
