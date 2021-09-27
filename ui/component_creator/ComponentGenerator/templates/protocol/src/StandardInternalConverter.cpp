#include "DSHost/Protocol/%name%/Impl/StandardInternalConverter.h"
#include "DSHost/Protocol/%name%/Message.h"
#include "DSHost/Protocol/StandardInternal/Message.h"
#include "Poco/Exception.h"

namespace DS {
namespace protocols {
namespace %name% {

bool %name%ToStandardInternalConverterImpl::convertMessage(Message::Ptr pInput, Message::Ptr pOutput)
{
    %name%Message::Ptr p%name%Message = pInput.cast<%name%Message>();
    StandardInternalMessage::Ptr pStandardInternalMessage = pOutput.cast<StandardInternalMessage>();

    if (!p%name%Message || !pStandardInternalMessage)
        return false;

    // TODO: implement conversion of the protocol message into a standard internal message here

    return true;
}

bool StandardInternalTo%name%ConverterImpl::convertMessage(Message::Ptr pInput, Message::Ptr pOutput)
{
    StandardInternalMessage::Ptr pStandardInternalMessage = pInput.cast<StandardInternalMessage>();
    %name%Message::Ptr p%name%Message = pOutput.cast<%name%Message>();

    if (!pStandardInternalMessage || !p%name%Message)
        return false;

    // TODO: implement conversion of a standard internal message into the protocol message here

    return true;
}

} } } // namespace DS::protocols::%name%

