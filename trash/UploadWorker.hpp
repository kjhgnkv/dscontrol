#pragma once

#include <QObject>
#include <QTimer>

//
#include "DeployWorker.hpp"
#include "DSServerGlobal.h"

namespace network
{
namespace worker
{
class UploadWorker : public DeployWorker
{
Q_OBJECT
public:
    explicit UploadWorker(QObject* parent = nullptr);
    ~UploadWorker();

    void upload(const std::string &filename, unsigned int ipAddress, const std::string &componentName,
                const std::string &uploadXml, DS::dsserver::CommandType::Enum type);

    void stop() override;

private slots:
    void process(unsigned long ipAddress, const DS::test::MessageHandler::SharedMemoryMessage &msg) override;

private:
    void uploadComponentBlock();

signals:
    void progressValueChanged(const QString &filePath, unsigned long ipAddress, double progress);

private:
    PublisherBlocksInfo blockInfo_;

    QTimer* timer_;
};

} // namespace worker
} // namespace network
