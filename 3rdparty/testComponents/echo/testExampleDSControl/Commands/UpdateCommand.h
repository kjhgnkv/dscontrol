#pragma once

#include <scopedPtr.h>
#include <queue>
#include <DSServerCommandMessage.h>

#include "../Handler/NetworkAccessory.h"
#include "../Handler/MessageHandler.h"


namespace DS {
namespace example {

class UpdateCommand
{
public:
    explicit UpdateCommand(ds::utils::ScopedPtr<MessageHandler>& msgHandler);
    ~UpdateCommand();

    bool process(Poco::UInt32 msgId, const std::string& ipAddress);

private:
    ds::utils::ScopedPtr<MessageHandler>& m_msgHandler;

    static const char * mcs_update1Xml;
    static const char * mcs_update2Xml;
};

}}
