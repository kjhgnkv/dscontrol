#include "StopCommand.h"
#include "../Logger.h"

#include <DSServerGlobal.h>
#include <sstream>

namespace DS {
namespace example {

const char * StopCommand::mcs_stop1Xml = "<root>\n"
                       "<CommandType>\n"
                       "\tStop\n"
                       "</CommandType>\n"
                       "<ApplicationName>\n"
                       "\tDSHost1\n"
                       "</ApplicationName>\n"
                       "</root>";

const char * StopCommand::mcs_stop2Xml = "<root>\n"
                       "<CommandType>\n"
                       "\tStop\n"
                       "</CommandType>\n"
                       "<ApplicationName>\n"
                       "\tDSHost2\n"
                       "</ApplicationName>\n"
                       "</root>";

StopCommand::StopCommand(ds::utils::ScopedPtr<MessageHandler>& msgHandler)
    : m_msgHandler(msgHandler)
{ }

StopCommand::~StopCommand()
{ }

bool StopCommand::process(Poco::UInt32 msgId, const std::string& ipAddress)
{
    {
    std::vector<char> stop1Data(mcs_stop1Xml, mcs_stop1Xml+strlen(mcs_stop1Xml)), command1;
    if(!DS::dsserver::DSServerCommandMessage::serialize(msgId, DS::dsserver::MessageDataType::XML, stop1Data, command1))
    {
        Logger::error(std::string("Can not serialize stop1Xml"));
        return false;
    }

    if(!m_msgHandler->sendToDSServer(command1, ipAddress))
        return false;

    Logger::notice(std::string("send to server: ") + mcs_stop1Xml);
    }

    {
    std::vector<char> stop2Data(mcs_stop2Xml, mcs_stop2Xml+strlen(mcs_stop2Xml)), command2;
    if(!DS::dsserver::DSServerCommandMessage::serialize(msgId, DS::dsserver::MessageDataType::XML, stop2Data, command2))
    {
        Logger::error(std::string("Can not serialize stop2Xml"));
        return false;
    }

    if(!m_msgHandler->sendToDSServer(command2, ipAddress))
        return false;

    Logger::notice(std::string("send to server: ") + mcs_stop2Xml);
    }

    return true;
}

}}
