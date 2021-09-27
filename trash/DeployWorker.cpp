#include "DeployWorker.hpp"
#include "DSClientImpl.hpp"

DeployWorker* DeployWorker::deployWorker_ = nullptr;

DeployWorker::DeployWorker(QObject* parent)
: AbstractWorker(parent)
, responce_ {}
{
    type_ = WorkerType::Deploy;

    connect(&DSClientImpl::Instance(), &DSClientImpl::receiveError, this, &AbstractWorker::process,
            Qt::QueuedConnection);
}

void DeployWorker::deploy(const unsigned long &ipAddress, const QByteArray &xml)
{
    publish(ipAddress, uploadXml.toStdString(), DS::dsserver::CommandType::Enum::Deploy);

    // DSClientImpl::Instance().publish(ipAddress, xml.toStdString(), DS::dsserver::CommandType::Enum::Deploy);
}

void DeployWorker::stop()
{
    emit finished();
}

void DeployWorker::process(unsigned long ipAddress, const DS::test::MessageHandler::SharedMemoryMessage &shmMsg)
{
    emit complete(ipAddress);
    Q_UNUSED(ipAddress)
    Q_UNUSED(shmMsg)
}

QVariant DeployWorker::response() const
{
    return responce_;
}
