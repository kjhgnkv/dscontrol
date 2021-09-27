#include "Poco/OSP/BundleActivator.h"
#include "Poco/OSP/BundleContext.h"
#include "Poco/OSP/ServiceRegistry.h"
#include "Poco/ClassLibrary.h"

class %name%CustomComponentActivator : public Poco::OSP::BundleActivator
{
public:
    %name%CustomComponentActivator()
    {
    }

    ~%name%CustomComponentActivator()
    {
    }

    void start(Poco::OSP::BundleContext::Ptr pContext)
    {
        // TODO: implement component starting method here
    }

    void stop(Poco::OSP::BundleContext::Ptr pContext)
    {
        // TODO: implement component stopping method here
    }
};

POCO_BEGIN_MANIFEST(Poco::OSP::BundleActivator)
    POCO_EXPORT_CLASS(%name%CustomComponentActivator)
POCO_END_MANIFEST

