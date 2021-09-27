#pragma once

#include "DeployWorker.hpp"

namespace network
{
namespace worker
{
class GetServerDataWorker : public DeployWorker
{
Q_OBJECT
public:
    explicit GetServerDataWorker(QObject* parent = nullptr);
    ~GetServerDataWorker() = default;

    void process(unsigned long ipAddress, const DS::test::MessageHandler::SharedMemoryMessage &shmMsg) override;
};
} // namespace worker
} // namespace network
