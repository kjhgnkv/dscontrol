#pragma once

#include <scopedPtr.h>
#include <Poco/AtomicCounter.h>

#include "Handler/CommandHandler.h"

#ifdef WIN32
    #define SHARED_MEMORY_LIB_PATH "../lib/SharedMemory.dll"
#else
    #define SHARED_MEMORY_LIB_PATH "../lib/libSharedMemory.so"
#endif
#define SHARED_MEMORY_CHANEL "DSControl"

class Controller
{
public:
    explicit Controller();
    ~Controller();

    bool init();
    // bool init(DS::threeRdParty::AbstractSharedMemory::Ptr &sharedMemory);
    bool exec();
    void stop();
    // bool isStop();

    bool receiveApplicationTemplateCommand(const std::string &itIp, const char * mcs_sendXml);
    bool receiveComponentCommand(const std::string &itIp, const char * mcs_uploadXml);

    bool setDSControlDataCommand(const std::string &itIp);
    bool getDSControlDataCommand(const std::string &itIp);

    bool deployCommand(const std::string &itIp, const char* mcs_deployXml);
    bool updateCommand(const std::string &itIp, const char* mcs_updateXml);
    bool stopCommand(const std::string &itIp, const char* mcs_stopXml);

    ds::utils::ScopedPtr<MessageHandler> &messageHandler();

private:
    ds::utils::ScopedPtr <CommandHandler> m_commandHandler;

    static const std::string msc_sharedMemoryFuncName;
};
