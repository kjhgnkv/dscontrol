#include <Poco/Delegate.h>
#include <Poco/Thread.h>
#include <Poco/SharedLibrary.h>
#include <ProcessSearch/ProcessSearch.h>

#include "Controller.h"
#include "Logger/Logger.h"

typedef DS::threeRdParty::AbstractSharedMemory* (* FCreateSharedMemory)(const char* channelName,
                                                                        const unsigned short memorySize);

const std::string Controller::msc_sharedMemoryFuncName = "create";

Controller::Controller()
: m_commandHandler(new CommandHandler())
{
}

Controller::~Controller() = default;

bool Controller::init()
{
    Poco::SharedPtr<Poco::SharedLibrary> pSharedLibrary;

    if (!Logger::instance().open())
    {
        std::cerr << "cann't open logger\n";
        return false;
    }

    try
    {
        pSharedLibrary = new Poco::SharedLibrary(SHARED_MEMORY_LIB_PATH);
    }
    catch (Poco::Exception &exc)
    {
        Logger::instance().log(exc.displayText() + ": " + SHARED_MEMORY_LIB_PATH);
        return false;
    }

    if (!pSharedLibrary)
    {
        Logger::instance().log("SharedLibrary is NULL");
        return false;
    }

    if (!pSharedLibrary->hasSymbol(msc_sharedMemoryFuncName))
    {
        Logger::instance().log(std::string("library don't have function: ") + msc_sharedMemoryFuncName);
        return false;
    }

    void* ptr = pSharedLibrary->getSymbol(msc_sharedMemoryFuncName);
    if (!ptr)
    {
        Logger::instance().log(std::string("function: ") + msc_sharedMemoryFuncName + " is NULL");
        return false;
    }

    FCreateSharedMemory func = reinterpret_cast<FCreateSharedMemory>(ptr);
    if (!func)
    {
        Logger::instance().log(std::string("cann't convert function: ") + msc_sharedMemoryFuncName);
        return false;
    }

    DS::threeRdParty::AbstractSharedMemory::Ptr sharedMemory = func(SHARED_MEMORY_CHANEL, SHARED_MEMORY_MEMORY_SIZE);
    if (!sharedMemory)
    {
        Logger::instance().log("cann't create Shared Memory chanel");
        return false;
    }

    return m_commandHandler->init(sharedMemory);
}

bool Controller::exec()
{
    return m_commandHandler->exec();
}

void Controller::stop()
{
    m_commandHandler->stop();
}

bool Controller::receiveApplicationTemplateCommand(const std::string &itIp, const char* mcs_sendXml)
{
    return m_commandHandler->receiveApplicationTemplateCommand(itIp, mcs_sendXml);
}

bool Controller::receiveComponentCommand(const std::string &itIp, const char* mcs_uploadXml)
{
    return m_commandHandler->receiveComponentCommand(itIp, mcs_uploadXml);
}

bool Controller::setDSControlDataCommand(const std::string &itIp)
{
    return m_commandHandler->setDSControlDataCommand(itIp);
}

bool Controller::getDSControlDataCommand(const std::string &itIp)
{
    return m_commandHandler->getDSControlDataCommand(itIp);
}

bool Controller::deployCommand(const std::string &itIp, const char* mcs_deployXml)
{
    return m_commandHandler->deployCommand(itIp, mcs_deployXml);
}

bool Controller::updateCommand(const std::string &itIp, const char* mcs_updateXml)
{
    return m_commandHandler->updateCommand(itIp, mcs_updateXml);
}

bool Controller::stopCommand(const std::string &itIp, const char* mcs_stopXml)
{
    return m_commandHandler->stopCommand(itIp, mcs_stopXml);
}

ds::utils::ScopedPtr<MessageHandler> &Controller::messageHandler()
{
    return m_commandHandler->mMessageHandler();
}
