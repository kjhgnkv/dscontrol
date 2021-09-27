#include "DSHost/Protocol/%name%/Impl/MessageAuxDataModifier.h"
#include "DSHost/Protocol/%name%/Message.h"
#include "Poco/Dynamic/Struct.h"
#include "Poco/Exception.h"

namespace DS {
namespace protocols {
namespace %name% {

Message::Ptr %name%MessageAuxDataModifierImpl::insert(Message::Ptr pMessage, const AuxData& aux)
{
    %name%Message::Ptr pInputMessage = pMessage.cast<%name%Message>();

    if (!pInputMessage)
        return NULL;

    %name%Message::Ptr pOutputMessage = pInputMessage->clone().cast<%name%Message>();
    poco_check_ptr(pOutputMessage);

    // TODO: implement insertion of auxilliary data here

    return pOutputMessage;
}

bool %name%MessageAuxDataModifierImpl::get(Message::Ptr pMessage, AuxData& aux)
{
    %name%Message::Ptr pInputMessage = pMessage.cast<%name%Message>();

    if (!pInputMessage)
        return false;

    aux = AuxData();

    // TODO: implement fetching of auxilliary data here

    return true;
}

Message::Ptr %name%MessageAuxDataModifierImpl::extract(Message::Ptr pMessage, AuxData& aux)
{
    if (!get(pMessage, aux))
        return NULL;

    %name%Message::Ptr pOutputMessage = pMessage->clone().cast<%name%Message>();
    poco_check_ptr(pOutputMessage);

    // TODO: implement removal of auxilliary data here

    return pOutputMessage;
}

} } } // namespace DS::protocols::%name%

