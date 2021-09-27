#include "ReceiveApplicationTemplateCommand.h"
#include "../Logger.h"

#include <DSServerGlobal.h>
#include <sstream>

namespace DS {
namespace example {

const char * ReceiveApplicationTemplateCommand::mcs_sendXml = "<root>\n"
                             "<CommandType>\n"
                             "\tSendApplicationTemplate\n"
                             "</CommandType>\n"
                             "<FileName>\n"
                             "\tSamples.zip\n"
                             "</FileName>\n"
                             "<FileSize>\n"
                             /*"\t4747378\n" "\t10523185*/"9607388\n"
                             "</FileSize>\n"
                             "<BlocksCount>\n"
                             /*"\t47\n" "\t103\n"*/"\t321\n"
                             "</BlocksCount>\n"
                             "</root>";


const char * ReceiveApplicationTemplateCommand::mcs_filePath = "/home/ubuntu/Samples.zip";

ReceiveApplicationTemplateCommand::ReceiveApplicationTemplateCommand(ds::utils::ScopedPtr<MessageHandler>& msgHandler)
    : m_msgHandler(msgHandler)
    , m_blockId(0)
    , m_isFinished(false)
    , m_tryCount(0)
    , m_lastMsgLen(0)
{ }

ReceiveApplicationTemplateCommand::~ReceiveApplicationTemplateCommand()
{
    if (!m_isFile.is_open())
        m_isFile.close();
}

bool ReceiveApplicationTemplateCommand::isFinished()
{
    return m_isFinished;
}

bool ReceiveApplicationTemplateCommand::process(Poco::UInt32 msgId, const std::string& ipAddress, std::queue<Poco::SharedPtr<IncomingMsg> >& lastMsgs)
{
    if (!m_isFile.is_open()) {
        std::vector<char> uploadData(mcs_sendXml, mcs_sendXml+strlen(mcs_sendXml)), command;
        if(!DS::dsserver::DSServerCommandMessage::serialize(msgId, DS::dsserver::MessageDataType::XML, uploadData, command))
        {
            Logger::error(std::string("Can not serialize sendXml"));
            return false;
        }

        if(!m_msgHandler->sendToDSServer(command, ipAddress))
            return false;

        Logger::notice(std::string("send to server: ") + mcs_sendXml);

        if (!m_isFile.is_open()) {
            m_isFile.open(mcs_filePath, std::ifstream::binary);
            if (!m_isFile) {
                Logger::error(std::string("Can not open file: ") + mcs_filePath);
                return false;
            }
        }

        return true;
    }

    bool result = false;
    while(!lastMsgs.empty()) {
        Poco::SharedPtr<IncomingMsg> localMsg = lastMsgs.front();
        lastMsgs.pop();

        if(localMsg->msgType != "Application")
        {
            continue;
        }

        if(localMsg->block == "finish")
        {
            m_isFinished = true;
            return true;
        }
        else if(localMsg->block == "error")
        {
            m_isFinished = true;
            return false;
        }
        else
        {

            try{
            Poco::Dynamic::Var block(localMsg->block);
            Poco::UInt64 blockId = block.convert<Poco::UInt64>();
            m_blockId = blockId;
            }
            catch(Poco::Exception& exc)
            {
                Logger::error(exc.displayText());
                continue;
            }
        }

        result = processMsg(msgId, ipAddress);
        if(!result)
        {
            Logger::warning("-----> processMsg == false");
        }
    }


    if(!result)
    {
        m_tryCount++;
        if(m_tryCount > 3000)
        {
            m_tryCount = 0;
            Logger::warning("-----> m_tryCount > 3000");
            return false;
        }
        else
        {
            m_tryCount++;
            Poco::Thread::sleep(10);
        }
    }
    else
    {
        Logger::notice("-----> m_tryCount == 0");
        m_tryCount = 0;
    }


    return true;
}

bool ReceiveApplicationTemplateCommand::processMsg(const Poco::UInt32 msgId, const std::string& ipAddress)
{
    if (!m_isFile) {
        Logger::error(std::string("Did not open file: ") + mcs_filePath);
        return false;
    }

    const unsigned char c_sizeBlockId = sizeof(Poco::UInt64);

    std::vector<char> buffer(30000 + c_sizeBlockId), outMsg;
    std::ifstream::pos_type pos = static_cast<std::ifstream::pos_type>(m_blockId*30000);
    m_isFile.seekg(pos, std::ifstream::beg);
    m_isFile.read((char *) (buffer.data() + c_sizeBlockId), 30000);
    m_lastMsgLen = m_isFile.gcount();
    if (m_lastMsgLen == 0) {
        Logger::error(std::string("Can not read file: ") + mcs_filePath);
        return false;
    }

    memcpy(buffer.data(), (char *) &m_blockId, c_sizeBlockId);
    buffer.resize(c_sizeBlockId+m_lastMsgLen);

    std::stringstream ss;
    ss << "\npublishMsg : m_blockId: " << m_blockId << "; m_lastMsgLen: " << m_lastMsgLen << "\n\n";
    Logger::notice(ss.str());

    if(!DS::dsserver::DSServerCommandMessage::serialize(msgId, DS::dsserver::MessageDataType::Application, buffer, outMsg))
    {
        Logger::error(std::string("Can not serialize msg"));
        return false;
    }

    if(!m_msgHandler->sendToDSServer(outMsg, ipAddress))
        return false;

    return true;
}

}}
