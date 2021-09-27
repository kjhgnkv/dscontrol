#include "SetDSControlDataCommand.h"
#include "../Logger.h"

#include <DSServerGlobal.h>
#include <sstream>

namespace DS {
namespace example {

const char * SetDSControlDataCommand::mcs_dsControlXml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                                         "<root>\n"
                                                         "<CommandType>\n"
                                                         "\tSetDSControlData\n"
                                                         "</CommandType>\n"
                                                         "<Data>\n"
                                                         "\t1111 Alex LOL\n" //Pasha wrote this message)
                                                         "</Data>\n"
                                                         "</root>";

SetDSControlDataCommand::SetDSControlDataCommand(ds::utils::ScopedPtr<MessageHandler>& msgHandler)
    : m_msgHandler(msgHandler)
{ }

SetDSControlDataCommand::~SetDSControlDataCommand()
{ }

bool SetDSControlDataCommand::process(Poco::UInt32 msgId, const std::string& ipAddress)
{
    std::vector<char> dsControlData(mcs_dsControlXml, mcs_dsControlXml+strlen(mcs_dsControlXml)), command;
    if(!DS::dsserver::DSServerCommandMessage::serialize(msgId, DS::dsserver::MessageDataType::XML, dsControlData, command))
    {
        Logger::error(std::string("Can not serialize dsControlXml"));
        return false;
    }

    if(!m_msgHandler->sendToDSServer(command, ipAddress))
        return false;

    Logger::notice(std::string("send to server: ") + mcs_dsControlXml);

    return true;
}


}}
