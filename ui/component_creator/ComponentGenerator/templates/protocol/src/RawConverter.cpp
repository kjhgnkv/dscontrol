#include "DSHost/Protocol/%name%/Impl/RawConverter.h"
#include "DSHost/Protocol/%name%/Message.h"
#include "DSHost/Protocol/Raw/Message.h"
#include "Poco/Exception.h"

namespace DS {
namespace protocols {
namespace %name% {

bool %name%ToRawConverterImpl::convertMessage(Message::Ptr pInput, Message::Ptr pOutput)
{
    %name%Message::Ptr p%name%Message = pInput.cast<%name%Message>();
    RawMessage::Ptr pRawMessage = pOutput.cast<RawMessage>();

    if (!p%name%Message || !pRawMessage)
        return false;

    // TODO: implement conversion of the protocol message into a raw message here

    return true;
}

bool RawTo%name%ConverterImpl::convertMessage(Message::Ptr pInput, Message::Ptr pOutput)
{
    RawMessage::Ptr pRawMessage = pInput.cast<RawMessage>();
    %name%Message::Ptr p%name%Message = pOutput.cast<%name%Message>();

    if (!pRawMessage || !p%name%Message)
        return false;

    // TODO: implement conversion of a raw message into the protocol message here

    return true;
}

} } } // namespace DS::protocols::%name%

