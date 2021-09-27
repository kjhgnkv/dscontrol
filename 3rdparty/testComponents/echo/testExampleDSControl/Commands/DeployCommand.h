#pragma once

#include <scopedPtr.h>
#include <queue>
#include <DSServerCommandMessage.h>

#include "../Handler/NetworkAccessory.h"
#include "../Handler/MessageHandler.h"


namespace DS {
namespace example {

class DeployCommand
{
public:
    explicit DeployCommand(ds::utils::ScopedPtr<MessageHandler>& msgHandler);
    ~DeployCommand();

    bool process(Poco::UInt32 msgId, const std::string& ipAddress);

private:
    ds::utils::ScopedPtr<MessageHandler>& m_msgHandler;

    static const char * mcs_deploy1Xml;
    static const char * mcs_deploy2Xml;
};

}}
