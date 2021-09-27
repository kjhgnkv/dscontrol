#include "DeployCommand.h"
#include "../Logger.h"

#include <DSServerGlobal.h>
#include <sstream>

namespace DS {
namespace example {

const char * DeployCommand::mcs_deploy1Xml =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<root>\n"
        "<CommandType>\n"
        "\tDeploy\n"
        "</CommandType>\n"
        "<ApplicationName>\n"
        "\tDSHost1\n"
        "</ApplicationName>\n"
        "<Components>"
            "<Component name=\"ds.host.samples.echo\" content_id=\"echo_msg\">"
                "<Object name=\"object1\" ds_content=\"object1-content\">"
                    "<Comm name=\"tcp-l\" id=\"2\" method=\"udp\" if=\"127.0.0.1:60000\"/>"
                    "<Send name=\"object1.sender2\" comm=\"tcp-l\" protocol=\"raw\" id=\"1\"/>"
                "</Object>"
                "<Object name=\"Obj2\" ds_content=\"object2-content\">"
                    "<Comm name=\"tcp-l\" id=\"1\" method=\"udp\" if=\"127.0.0.1:60000\"/>"
                    "<Recv name=\"Obj2.receiver2\" comm=\"tcp-l\" protocol=\"raw\" id=\"1\"/>"
                "</Object>"
            "</Component>"
        "</Components>"
        "</root>";

const char * DeployCommand::mcs_deploy2Xml =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<root>\n"
        "<CommandType>\n"
        "\tDeploy\n"
        "</CommandType>\n"
        "<ApplicationName>\n"
        "\tDSHost2\n"
        "</ApplicationName>\n"
        "<Components>"
            "<Component name=\"ds.host.samples.echo\" content_id=\"echo_msg\">"
                "<Object name=\"object1\" ds_content=\"object1-content\">"
                    "<Comm name=\"tcp-l\" id=\"2\" method=\"udp\" if=\"127.0.0.1:60001\"/>"
                    "<Send name=\"object1.sender2\" comm=\"tcp-l\" protocol=\"raw\" id=\"2\"/>"
                "</Object>"
                "<Object name=\"Obj2\" ds_content=\"object2-content\">"
                    "<Comm name=\"tcp-l\" id=\"1\" method=\"udp\" if=\"127.0.0.1:60001\"/>"
                    "<Recv name=\"Obj2.receiver2\" comm=\"tcp-l\" protocol=\"raw\" id=\"2\"/>"
                "</Object>"
            "</Component>"
        "</Components>"
        "</root>";

DeployCommand::DeployCommand(ds::utils::ScopedPtr<MessageHandler>& msgHandler)
    : m_msgHandler(msgHandler)
{ }

DeployCommand::~DeployCommand()
{ }

bool DeployCommand::process(Poco::UInt32 msgId, const std::string& ipAddress)
{
    {
    std::vector<char> deploy1Data(mcs_deploy1Xml, mcs_deploy1Xml+strlen(mcs_deploy1Xml)), command1;
    if(!DS::dsserver::DSServerCommandMessage::serialize(msgId, DS::dsserver::MessageDataType::XML, deploy1Data, command1))
    {
        Logger::error(std::string("Can not serialize deploy1Xml"));
        return false;
    }

    if(!m_msgHandler->sendToDSServer(command1, ipAddress))
        return false;

    Logger::notice(std::string("send to server: ") + mcs_deploy1Xml);
    }

    {
    std::vector<char> deploy2Data(mcs_deploy2Xml, mcs_deploy2Xml+strlen(mcs_deploy2Xml)), command2;
    if(!DS::dsserver::DSServerCommandMessage::serialize(msgId, DS::dsserver::MessageDataType::XML, deploy2Data, command2))
    {
        Logger::error(std::string("Can not serialize deploy2Xml"));
        return false;
    }

    if(!m_msgHandler->sendToDSServer(command2, ipAddress))
        return false;

    Logger::notice(std::string("send to server: ") + mcs_deploy2Xml);
    }

    return true;
}

}}
