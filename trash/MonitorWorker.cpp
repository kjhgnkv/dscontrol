#include "MonitorWorker.hpp"

//
#include "IncomingLocalMessage.hpp"

using namespace network::worker;

MonitorWorker::MonitorWorker(QObject* parent)
: DeployWorker(parent)
{
    type_ = WorkerType::Monitor;
    connect(&DSClientImpl::Instance(), &DSClientImpl::receiveNewMonitorMessage, this, &AbstractWorker::process);
}

void MonitorWorker::process(unsigned long ipAddress, const DS::test::MessageHandler::SharedMemoryMessage &shmMsg)
{
    // if (shmMsg->msgType == "Error")
    // {
    //     return;
    // }

    // TODO ???

    // responce_ = QVariant {QString::fromStdString(msg->json())};
    ipAddress_ = ipAddress;
    DeployWorker::process(ipAddress, shmMsg);
}

unsigned long MonitorWorker::ipAddress() const
{
    return ipAddress_;
}
