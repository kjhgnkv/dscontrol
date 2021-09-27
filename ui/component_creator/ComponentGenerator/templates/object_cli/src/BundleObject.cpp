#include "BundleObject.h"
#include "DSHost/Bundle/Activator.h"

%namespace-begin%

%name%BundleObject::BUNDLE_OBJECT_CTOR_IMPL(%name%BundleObject),
    _version(pContext->thisBundle()->version().toString())
{
    Poco::OSP::ServiceRef::Ptr pAppServiceRef = context()->registry().findByName(APP_SERVICE_NAME);
    poco_assert(pAppServiceRef);
    _pAppService = pAppServiceRef->castedInstance<DS::app::AppService>();

    // TODO: implement constructor here
}

void %name%BundleObject::onInitialize()
{
    _pSender = _pAppService->getCommunicationService()->sender(name());
    _pReceiver = _pAppService->getCommunicationService()->receiver(name());
    _pReceiver->reception += delegate(this, &%name%BundleObject::onReceive);

    // TODO: implement object initializing method here
}

void %name%BundleObject::onStart()
{
    // TODO: implement object starting method here
}

void %name%BundleObject::onStop()
{
    // TODO: implement object stopping method here
}

void %name%BundleObject::onUninitialize()
{
    // TODO: implement object uninitializing method here

    _pReceiver->reception -= delegate(this, &%name%BundleObject::onReceive);
    _pReceiver.reset();
    _pSender.reset();
}

void %name%BundleObject::onSystemEvent(Poco::OSP::SystemEvents::EventKind& eventKind)
{
    switch (eventKind)
    {
        case Poco::OSP::SystemEvents::EV_SYSTEM_STARTED:
            // TODO: implement object's actions after DSHost Configurator subsystem finishes reconfiguration
            break;

        case Poco::OSP::SystemEvents::EV_SYSTEM_SHUTTING_DOWN:
            // TODO: implement object's actions after DSHost Configurator subsystem begins reconfiguration
            break;
    }
}

void %name%BundleObject::onReceive(const DS::communication::Receiver::ReceivedMessage& msg)
{
    // TODO: implement object's message receiver callback
}

%namespace-end%

TRIVIAL_BUNDLE_ACTIVATOR(%name%BusinessComponentActivator, %namespace-prefix%%name%BundleObject);

POCO_BEGIN_MANIFEST(Poco::OSP::BundleActivator)
POCO_EXPORT_CLASS(%name%BusinessComponentActivator)
POCO_END_MANIFEST

