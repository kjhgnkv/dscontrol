#include <Poco/Delegate.h>
//#include <Description.h>
#include <Poco/Thread.h>

#include "Controller.h"
#include "Logger.h"

using namespace Poco;

namespace DS {
namespace example {
namespace dsmonitor {

Controller::Controller(Sender::Ptr pSender
                       , Receiver::Ptr pReceiver
                       , ProtocolsImplementationFactoryService::Ptr pImplementationFactoryService
                       , ProtocolService::Ptr pProtoService)
    : m_isStop(0)
    , m_messageHandler(new MessageHandler(pSender, pReceiver, pImplementationFactoryService, pProtoService))
{ }

Controller::~Controller()
{
}

bool Controller::init()
{
    return true;
}

bool Controller::exec()
{
    while(!m_isStop)
    {
        m_messageHandler->sendToDSServer("{\"status\": \"0\", \"appType\": \"DSMonitor\", \"msgType\": \"Status\"}");
        Poco::Thread::sleep(1000);
    }

    return true;
}

void Controller::stop()
{
    m_isStop = 1;
}
}}}
