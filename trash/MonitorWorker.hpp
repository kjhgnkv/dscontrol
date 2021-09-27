#pragma once

#include "DeployWorker.hpp"

namespace network
{
namespace worker
{
class MonitorWorker : public DeployWorker
{
public:
    explicit MonitorWorker(QObject* parent = nullptr);
    ~MonitorWorker() = default;

    void process(unsigned long ipAddress, const DS::test::MessageHandler::SharedMemoryMessage &shmMsg) override;
    unsigned long ipAddress() const;

private:
    unsigned long ipAddress_;
};

} // namespace worker
} // namespace network
