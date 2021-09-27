#pragma once

#include <fstream>
#include <scopedPtr.h>
#include <queue>
#include <DSServerCommandMessage.h>

#include "../Handler/NetworkAccessory.h"
#include "../Handler/MessageHandler.h"


namespace DS {
namespace example {

class ReceiveApplicationTemplateCommand
{
public:
    explicit ReceiveApplicationTemplateCommand(ds::utils::ScopedPtr<MessageHandler>& msgHandler);
    ~ReceiveApplicationTemplateCommand();

    bool process(Poco::UInt32 msgId, const std::string& ipAddress, std::queue<Poco::SharedPtr<IncomingMsg> >& lastMsgs);

    bool isFinished();

private:
    ds::utils::ScopedPtr<MessageHandler>& m_msgHandler;
    std::ifstream m_isFile;
    Poco::UInt64 m_blockId;
    bool m_isFinished;
    int m_tryCount;
    unsigned short m_lastMsgLen;

    static const char * mcs_sendXml;
    static const char * mcs_filePath;

    bool processMsg(const Poco::UInt32 msgId, const std::string& ipAddress);
};

}}
