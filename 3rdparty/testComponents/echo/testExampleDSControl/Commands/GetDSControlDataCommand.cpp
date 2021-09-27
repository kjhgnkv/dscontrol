#include "GetDSControlDataCommand.h"
#include "../Logger.h"

#include <DSServerGlobal.h>
#include <sstream>

namespace DS {
namespace example {

const char* GetDSControlDataCommand::mcs_getData = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                            "<root>\n"
                                            "<CommandType>\n"
                                            "\tGetDSControlData\n"
                                            "</CommandType>\n"
                                            "</root>";

GetDSControlDataCommand::GetDSControlDataCommand(ds::utils::ScopedPtr<MessageHandler>& msgHandler)
    : m_msgHandler(msgHandler)
{ }

GetDSControlDataCommand::~GetDSControlDataCommand()
{ }

bool GetDSControlDataCommand::process(Poco::UInt32 msgId, const std::string& ipAddress)
{
    std::vector<char> getData(mcs_getData, mcs_getData+strlen(mcs_getData)), command;
    if(!DS::dsserver::DSServerCommandMessage::serialize(msgId, DS::dsserver::MessageDataType::XML, getData, command))
    {
        Logger::error(std::string("Can not serialize mcs_getData"));
        return false;
    }

    if(!m_msgHandler->sendToDSServer(command, ipAddress))
        return false;

    Logger::notice(std::string("send to server: ") + mcs_getData);

    return true;
}
}}
