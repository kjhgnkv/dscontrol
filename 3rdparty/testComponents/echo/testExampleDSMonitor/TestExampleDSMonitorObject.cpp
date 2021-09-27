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

#include "Logger.h"
#include <vector>
#include "Controller.h"

using DS::bundle_objects::BundleObject;

using DS::bundle_objects::BundleObject;
using DS::communication::CommunicationService;
using DS::communication::Sender;
using DS::communication::Receiver;
using DS::protocols::ProtocolService;

/*#include "Communication/Service.h"
#include "Communication/ProtocolService.h"
#include "Communication/Error.h"
#include "Communication/ErrorMessage.h"
#include "Communication/RawMessage.h"
#include "Communication/StructMessage.h"
#include "Bundle/Activator.h"
#include "Event/Service.h"
#include "Controller.h"
#include "Logger.h"
#include <vector>

using DS::bundle_objects::BundleObject;

using DS::communication::CommunicationService;
using DS::communication::ProtocolService;

using DS::communication::MessageError;
using DS::communication::Message;
using DS::communication::ErrorMessage;
using DS::communication::StructMessage;

using DS::communication::MessageSender;
using DS::communication::MessageReceiver;
using DS::communication::Buffer;
using DS::events::EventService;*/

using DS::example::dsmonitor::Logger;
using DS::example::dsmonitor::Controller;

#define DEFAULT_CONTENT_ID "DSMonitor"

class TestExampleDSMonitorObject : public BundleObject, public Poco::Runnable
{
public:
    BUNDLE_OBJECT_CTOR_DECL(TestExampleDSMonitorObject)
        :DS::bundle_objects::BundleObject(name, pContext, config)
        , m_version(pContext->thisBundle()->version().toString())
        , m_contentId(DEFAULT_CONTENT_ID)
        , m_isRunning(false)
        , m_flag(1)
        , m_stopped(1)
    {
        Logger::init(context(), name, m_version);

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

        Poco::OSP::ServiceRef::Ptr pImplementationFactoryServiceRef =
                context()->registry().findByName(PROTOCOLS_IMPLEMENTATION_FACTORY_SERVICE_NAME);
        if (!pImplementationFactoryServiceRef)
        {
            Logger::error("The ImplementationFactoryService is not available.");
            return;
        }
        m_pImplementationFactoryService = pImplementationFactoryServiceRef->castedInstance<ProtocolsImplementationFactoryService>();
        if (!m_pImplementationFactoryService)
        {
            Logger::error("The ImplementationFactoryService is not available.");
            return;
        }

        m_thread.setName(name);
    }

    ~TestExampleDSMonitorObject()
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

        m_controller.reset(new Controller(m_pSender, m_pReceiver, m_pImplementationFactoryService, m_pProtoService));
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
    ProtocolsImplementationFactoryService::Ptr m_pImplementationFactoryService;

    Sender::Ptr m_pSender;
    Receiver::Ptr m_pReceiver;

    ds::utils::ScopedPtr<Controller> m_controller;

    bool m_isRunning;
    Poco::Thread m_thread;
    Poco::AtomicCounter m_flag, m_stopped;
};

BUNDLE_ACTIVATOR(TestExampleDSMonitorBundleActivator, TestExampleDSMonitorObject);

POCO_BEGIN_MANIFEST(Poco::OSP::BundleActivator)
    POCO_EXPORT_CLASS(TestExampleDSMonitorBundleActivator)
POCO_END_MANIFEST
