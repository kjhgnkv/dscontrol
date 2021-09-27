#pragma once

#include <scopedPtr.h>
#include <queue>
#include <DSServerCommandMessage.h>

#include "../Handler/NetworkAccessory.h"
#include "../Handler/MessageHandler.h"


namespace DS {
namespace example {

class GetDSControlDataCommand
{
public:
    explicit GetDSControlDataCommand(ds::utils::ScopedPtr<MessageHandler>& msgHandler);
    ~GetDSControlDataCommand();

    bool process(Poco::UInt32 msgId, const std::string& ipAddress);

private:
    ds::utils::ScopedPtr<MessageHandler>& m_msgHandler;

    static const char * mcs_getData;
};


}}
