#include "Poco/OSP/BundleActivator.h"
#include "Poco/OSP/BundleContext.h"
#include "Poco/OSP/ServiceRegistry.h"
#include "Poco/ClassLibrary.h"

#include "DSHost/Protocol/FactoryService.h"
#include "DSHost/Protocol/ConversionService.h"
#include "DSHost/Transport/Headerless/MessageAuxDataModifierService.h"

#include "DSHost/Protocol/%name%/ProtocolFactory.h"
#include "DSHost/Protocol/%name%/MessageAuxDataModifier.h"
#include "DSHost/Protocol/%name%/RawConverter.h"
#include "DSHost/Protocol/%name%/StandardInternalConverter.h"

using DS::protocols::ProtocolFactoryService;
using DS::protocols::ProtocolConversionService;
using DS::transport::MessageAuxDataModifierService;

using DS::protocols::%name%::%name%ProtocolFactory;
using DS::protocols::%name%::%name%ProtocolFactoryImpl;
using DS::protocols::%name%::%name%MessageAuxDataModifier;
using DS::protocols::%name%::%name%MessageAuxDataModifierImpl;
using DS::protocols::%name%::%name%ToRawConverter;
using DS::protocols::%name%::%name%ToRawConverterImpl;
using DS::protocols::%name%::RawTo%name%Converter;
using DS::protocols::%name%::RawTo%name%ConverterImpl;
using DS::protocols::%name%::%name%ToStandardInternalConverter;
using DS::protocols::%name%::%name%ToStandardInternalConverterImpl;
using DS::protocols::%name%::StandardInternalTo%name%Converter;
using DS::protocols::%name%::StandardInternalTo%name%ConverterImpl;

class %name%ProtocolComponentActivator : public Poco::OSP::BundleActivator
{
public:
    %name%ProtocolComponentActivator()
    {
    }

    ~%name%ProtocolComponentActivator()
    {
    }

    void start(Poco::OSP::BundleContext::Ptr pContext)
    {
        Poco::OSP::ServiceRef::Ptr pProtocolFactoryServiceRef =
            pContext->registry().findByName(PROTOCOL_FACTORY_SERVICE_NAME);
        if (!pProtocolFactoryServiceRef)
        {
            pContext->logger().error("The ProtocolFactoryService is not available.");
            return;
        }
        _pProtocolFactoryService = pProtocolFactoryServiceRef->castedInstance<ProtocolFactoryService>();

        Poco::OSP::ServiceRef::Ptr pProtocolConversionServiceRef =
            pContext->registry().findByName(PROTOCOL_CONVERSION_SERVICE_NAME);
        if (!pProtocolConversionServiceRef)
        {
            pContext->logger().error("The ProtocolConversionService is not available.");
            return;
        }
        _pProtocolConversionService = pProtocolConversionServiceRef->castedInstance<ProtocolConversionService>();

        Poco::OSP::ServiceRef::Ptr pMessageAuxDataModifierServiceRef =
            pContext->registry().findByName(MESSAGE_AUX_DATA_MODIFIER_SERVICE_NAME);
        if (!pMessageAuxDataModifierServiceRef)
        {
            pContext->logger().error("The MessageAuxDataModifierService is not available.");
            return;
        }
        _pMessageAuxDataModifierService = pMessageAuxDataModifierServiceRef->castedInstance<MessageAuxDataModifierService>();

        _pProtocolFactory = new %name%ProtocolFactory(
                new %name%ProtocolFactoryImpl(&pContext->logger()));
        _pProtocolFactoryService->registerFactory(_pProtocolFactory);

        _pMessageModifier = new %name%MessageAuxDataModifier(new %name%MessageAuxDataModifierImpl);
        _pMessageAuxDataModifierService->registerMessageAuxDataModifier(_pMessageModifier);

        _p%name%ToRawConverter = new %name%ToRawConverter(new %name%ToRawConverterImpl);
        _pRawTo%name%Converter = new RawTo%name%Converter(new RawTo%name%ConverterImpl);
        _p%name%ToStandardInternalConverter = new %name%ToStandardInternalConverter(new %name%ToStandardInternalConverterImpl);
        _pStandardInternalTo%name%Converter = new StandardInternalTo%name%Converter(new StandardInternalTo%name%ConverterImpl);
        _pProtocolConversionService->registerConverter(_p%name%ToRawConverter);
        _pProtocolConversionService->registerConverter(_pRawTo%name%Converter);
        _pProtocolConversionService->registerConverter(_p%name%ToStandardInternalConverter);
        _pProtocolConversionService->registerConverter(_pStandardInternalTo%name%Converter);
    }

    void stop(Poco::OSP::BundleContext::Ptr pContext)
    {
        _pProtocolConversionService->unregisterConverter(
                _pStandardInternalTo%name%Converter->inputProtocolFactoryName(),
                _pStandardInternalTo%name%Converter->inputProtocolFactoryComponentName(),
                _pStandardInternalTo%name%Converter->outputProtocolFactoryName(),
                _pStandardInternalTo%name%Converter->outputProtocolFactoryComponentName());
        _pProtocolConversionService->unregisterConverter(
                _p%name%ToStandardInternalConverter->inputProtocolFactoryName(),
                _p%name%ToStandardInternalConverter->inputProtocolFactoryComponentName(),
                _p%name%ToStandardInternalConverter->outputProtocolFactoryName(),
                _p%name%ToStandardInternalConverter->outputProtocolFactoryComponentName());
        _pProtocolConversionService->unregisterConverter(
                _pRawTo%name%Converter->inputProtocolFactoryName(),
                _pRawTo%name%Converter->inputProtocolFactoryComponentName(),
                _pRawTo%name%Converter->outputProtocolFactoryName(),
                _pRawTo%name%Converter->outputProtocolFactoryComponentName());
        _pProtocolConversionService->unregisterConverter(
                _p%name%ToRawConverter->inputProtocolFactoryName(),
                _p%name%ToRawConverter->inputProtocolFactoryComponentName(),
                _p%name%ToRawConverter->outputProtocolFactoryName(),
                _p%name%ToRawConverter->outputProtocolFactoryComponentName());
        _pProtocolFactoryService->unregisterFactory(_pProtocolFactory->name(),
                _pProtocolFactory->componentName());

        _pMessageAuxDataModifierService->unregisterMessageAuxDataModifier(
                _pMessageModifier->protocolFactoryName(),
                _pMessageModifier->protocolFactoryComponentName(),
                _pMessageModifier->protocolName());

        _pStandardInternalTo%name%Converter.reset();
        _p%name%ToStandardInternalConverter.reset();
        _pRawTo%name%Converter.reset();
        _p%name%ToRawConverter.reset();
        _pMessageModifier.reset();
        _pProtocolFactory.reset();
        _pMessageAuxDataModifierService.reset();
        _pProtocolConversionService.reset();
        _pProtocolFactoryService.reset();
    }

private:
    ProtocolFactoryService::Ptr _pProtocolFactoryService;
    ProtocolConversionService::Ptr _pProtocolConversionService;
    MessageAuxDataModifierService::Ptr _pMessageAuxDataModifierService;

    %name%ProtocolFactory::Ptr _pProtocolFactory;
    %name%MessageAuxDataModifier::Ptr _pMessageModifier;
    %name%ToRawConverter::Ptr _p%name%ToRawConverter;
    RawTo%name%Converter::Ptr _pRawTo%name%Converter;
    %name%ToStandardInternalConverter::Ptr _p%name%ToStandardInternalConverter;
    StandardInternalTo%name%Converter::Ptr _pStandardInternalTo%name%Converter;
};

POCO_BEGIN_MANIFEST(Poco::OSP::BundleActivator)
    POCO_EXPORT_CLASS(%name%ProtocolComponentActivator)
POCO_END_MANIFEST

