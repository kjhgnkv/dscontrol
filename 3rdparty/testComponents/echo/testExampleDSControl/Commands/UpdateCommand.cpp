#include "UpdateCommand.h"
#include "../Logger.h"

#include <DSServerGlobal.h>
#include <sstream>

namespace DS {
namespace example {

const char * UpdateCommand::mcs_update1Xml =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<root>\n"
        "<CommandType>\n"
        "\tUpdate\n"
        "</CommandType>\n"
        "<ApplicationName>\n"
        "\tDSHost1\n"
        "</ApplicationName>\n"
        "<Components>"
            "<Component name=\"ds.host.samples.echo\" content_id=\"echo_msg\">"
                "<Object name=\"object1\" ds_content=\"object1-content\">"
                    "<Comm name=\"tcp-l\" id=\"2\" method=\"udp\" if=\"127.0.0.1:61000\"/>"
                    "<Send name=\"object1.sender2\" comm=\"tcp-l\" protocol=\"raw\" id=\"all\"/>"
                "</Object>"
                "<Object name=\"Obj2\" ds_content=\"object2-content\">"
                    "<Comm name=\"tcp-l\" id=\"1\" method=\"udp\" if=\"127.0.0.1:61000\"/>"
                    "<Recv name=\"Obj2.receiver2\" comm=\"tcp-l\" protocol=\"raw\" id=\"all\"/>"
                "</Object>"
            "</Component>"
        "</Components>"
        "</root>";

const char * UpdateCommand::mcs_update2Xml =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<root>\n"
        "<CommandType>\n"
        "\tUpdate\n"
        "</CommandType>\n"
        "<ApplicationName>\n"
        "\tDSHost2\n"
        "</ApplicationName>\n"
        "<Components>"
            "<Component name=\"ds.host.samples.echo\" content_id=\"echo_msg\">"
                "<Object name=\"object1\" ds_content=\"object1-content\">"
                    "<Comm name=\"tcp-l\" id=\"2\" method=\"udp\" if=\"127.0.0.1:61001\"/>"
                    "<Send name=\"object1.sender2\" comm=\"tcp-l\" protocol=\"raw\" id=\"all\"/>"
                "</Object>"
                "<Object name=\"Obj2\" ds_content=\"object2-content\">"
                    "<Comm name=\"tcp-l\" id=\"1\" method=\"udp\" if=\"127.0.0.1:61001\"/>"
                    "<Recv name=\"Obj2.receiver2\" comm=\"tcp-l\" protocol=\"raw\" id=\"all\"/>"
                "</Object>"
            "</Component>"
        "</Components>"
        "</root>";

UpdateCommand::UpdateCommand(ds::utils::ScopedPtr<MessageHandler>& msgHandler)
    : m_msgHandler(msgHandler)
{ }

UpdateCommand::~UpdateCommand()
{ }

bool UpdateCommand::process(Poco::UInt32 msgId, const std::string& ipAddress)
{
    {
    std::vector<char> update1Data(mcs_update1Xml, mcs_update1Xml+strlen(mcs_update1Xml)), command1;
    if(!DS::dsserver::DSServerCommandMessage::serialize(msgId, DS::dsserver::MessageDataType::XML, update1Data, command1))
    {
        Logger::error(std::string("Can not serialize update1Xml"));
        return false;
    }

    if(!m_msgHandler->sendToDSServer(command1, ipAddress))
        return false;

    Logger::notice(std::string("send to server: ") + mcs_update1Xml);
    }

    {
    std::vector<char> update2Data(mcs_update2Xml, mcs_update2Xml+strlen(mcs_update2Xml)), command2;
    if(!DS::dsserver::DSServerCommandMessage::serialize(msgId, DS::dsserver::MessageDataType::XML, update2Data, command2))
    {
        Logger::error(std::string("Can not serialize update2Xml"));
        return false;
    }

    if(!m_msgHandler->sendToDSServer(command2, ipAddress))
        return false;

    Logger::notice(std::string("send to server: ") + mcs_update2Xml);
    }

    return true;
}

}}
