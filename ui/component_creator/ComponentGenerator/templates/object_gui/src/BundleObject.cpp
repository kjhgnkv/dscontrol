#include "BundleObject.h"
#include "DSHost/Bundle/Activator.h"
#include "DSHost/QtDisplay/WidgetFactory.h"
#include "DSHost/QtDisplay/ImplementationFactoryService.h"
#include "QtWidget.h"
#include <QWidget>

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

///////////////////////////////////////////////////////////////////////////////

class %name%WidgetFactory : public DS::qtdisplay::QtDisplayWidgetFactory
{
public:
    explicit %name%WidgetFactory(const std::string& componentName, const std::string& name,
            DS::qtdisplay::QtDisplayImplementationFactoryService::Ptr pService);

    std::string componentName() const;
    std::string name() const;

    DS::qtdisplay::QtDisplayWidget::Ptr getWidget(const std::string& name) const;

private:
    std::string _componentName;
    std::string _name;

    DS::qtdisplay::QtDisplayImplementationFactoryService::Ptr _pService;
};

inline %name%WidgetFactory::%name%WidgetFactory(const std::string& componentName,
        const std::string& name, DS::qtdisplay::QtDisplayImplementationFactoryService::Ptr pService)
    :_componentName(componentName), _name(name), _pService(pService)
{
    poco_check_ptr(pService);
}

inline std::string %name%WidgetFactory::componentName() const
{
    return _componentName;
}

inline std::string %name%WidgetFactory::name() const
{
    return _name;
}

inline DS::qtdisplay::QtDisplayWidget::Ptr %name%WidgetFactory::getWidget(const std::string& name) const
{
    return _pService->getWidget(_componentName, _name, name, new %name%Widget);
}

%namespace-end%

class %name%GraphicalBusinessComponentActivator : public DS::bundle_objects::BundleObjectActivator<%namespace-prefix%%name%BundleObject>
{
public:
    void onStart(Poco::OSP::BundleContext::Ptr pContext)
    {
        Poco::OSP::ServiceRef::Ptr pAppServiceRef = pContext->registry().findByName(APP_SERVICE_NAME);
        if (!pAppServiceRef)
        {
            pContext->logger().error("The AppService is not available.");
            return;
        }
        DS::app::AppService::Ptr pAppService = pAppServiceRef->castedInstance<DS::app::AppService>();

        if (pAppService->getQtDisplayImplementationFactoryService()->isQtDisplaySupported())
        {
            _pWidgetFactory = new %namespace-prefix%%name%WidgetFactory(pContext->thisBundle()->symbolicName(), "",
                    pAppService->getQtDisplayImplementationFactoryService());
            poco_check_ptr(_pWidgetFactory);

            pAppService->getQtDisplayWidgetFactoryService()->registerFactory(_pWidgetFactory);
        }
    }

    void onStop(Poco::OSP::BundleContext::Ptr pContext)
    {
        DS::qtdisplay::QtDisplayWidgetFactory::Ptr pWidgetFactory = _pWidgetFactory;
        _pWidgetFactory.reset();

        Poco::OSP::ServiceRef::Ptr pAppServiceRef = pContext->registry().findByName(APP_SERVICE_NAME);
        if (!pAppServiceRef)
        {
            pContext->logger().error("The AppService is not available.");
            return;
        }
        DS::app::AppService::Ptr pAppService = pAppServiceRef->castedInstance<DS::app::AppService>();

        if (pWidgetFactory)
            pAppService->getQtDisplayWidgetFactoryService()->unregisterFactory(pWidgetFactory->componentName(), pWidgetFactory->name());
    }

private:
    DS::qtdisplay::QtDisplayWidgetFactory::Ptr _pWidgetFactory;
};

POCO_BEGIN_MANIFEST(Poco::OSP::BundleActivator)
POCO_EXPORT_CLASS(%name%GraphicalBusinessComponentActivator)
POCO_END_MANIFEST

