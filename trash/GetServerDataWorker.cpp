#include "GetServerDataWorker.hpp"

using namespace network::worker;

GetServerDataWorker::GetServerDataWorker(QObject* parent)
: DeployWorker(parent)
{
    type_ = WorkerType::GetServerData;
    connect(&DSClientImpl::Instance(), &DSClientImpl::receiveNewGetDataMessage, this, &AbstractWorker::process,
            Qt::QueuedConnection);
}

void GetServerDataWorker::process(unsigned long ipAddress, const DS::test::MessageHandler::SharedMemoryMessage &shmMsg)
{
    // TODO ???
    // responce_ = QVariant {QString::fromStdString(shmMsg->data())};
    DeployWorker::process(ipAddress, shmMsg);
}
