#include "Poco/OSP/BundleActivator.h"
#include "Poco/OSP/BundleContext.h"
#include "Poco/OSP/ServiceRegistry.h"
#include "Poco/ClassLibrary.h"

#include "DSHost/Transport/ChannelFactoryService.h"
#include "DSHost/Transport/ImplementationFactoryService.h"

#include "DSHost/Transport/%name%/ChannelFactory.h"

using DS::transport::TransportChannelFactoryService;
using DS::transport::TransportImplementationFactoryService;

using DS::transport::%name%::%name%ChannelFactory;
using DS::transport::%name%::%name%ChannelFactoryImpl;
using DS::transport::%name%::%name%ChannelImpl;

class %name%TransportComponentActivator : public Poco::OSP::BundleActivator
{
public:
    %name%TransportComponentActivator()
    {
    }

    ~%name%TransportComponentActivator()
    {
    }

    void start(Poco::OSP::BundleContext::Ptr pContext)
    {
        Poco::OSP::ServiceRef::Ptr pChannelFactoryServiceRef =
            pContext->registry().findByName(TRANSPORT_CHANNEL_FACTORY_SERVICE_NAME);
        if (!pChannelFactoryServiceRef)
        {
            pContext->logger().error("The TransportChannelFactoryService is not available.");
            return;
        }
        _pChannelFactoryService = pChannelFactoryServiceRef->castedInstance<TransportChannelFactoryService>();

        Poco::OSP::ServiceRef::Ptr pImplementationFactoryServiceRef =
            pContext->registry().findByName(TRANSPORT_IMPLEMENTATION_FACTORY_SERVICE_NAME);
        if (!pImplementationFactoryServiceRef)
        {
            pContext->logger().error("The TransportImplementationFactoryService is not available.");
            return;
        }
        TransportImplementationFactoryService::Ptr pImplementationFactoryService =
            pImplementationFactoryServiceRef->castedInstance<TransportImplementationFactoryService>();

        %name%ChannelImpl::s_pImplementationFactoryService = pImplementationFactoryService;

        _pChannelFactory = new %name%ChannelFactory(
                new %name%ChannelFactoryImpl(pImplementationFactoryService, &pContext->logger()));

        _pChannelFactoryService->registerFactory(_pChannelFactory);
    }

    void stop(Poco::OSP::BundleContext::Ptr pContext)
    {
        _pChannelFactoryService->unregisterFactory(
                _pChannelFactory->name(), _pChannelFactory->componentName());

        %name%ChannelImpl::s_pImplementationFactoryService.reset();

        _pChannelFactory.reset();
        _pChannelFactoryService.reset();
    }

private:
    TransportChannelFactoryService::Ptr _pChannelFactoryService;

    %name%ChannelFactory::Ptr _pChannelFactory;
};

POCO_BEGIN_MANIFEST(Poco::OSP::BundleActivator)
    POCO_EXPORT_CLASS(%name%TransportComponentActivator)
POCO_END_MANIFEST

