#pragma once

#include <QObject>

#include "DSClientImpl.hpp"
#include "network_globals.hpp"

namespace network
{
namespace worker
{
struct IncomingMsg
{
    const std::string appType;
    const std::string block;
    const std::string msgType;

    std::vector<char> msg;
    std::string ip;
};

class AbstractWorker : public QObject
{
Q_OBJECT
public:
    explicit AbstractWorker(QObject* parent = nullptr);
    virtual ~AbstractWorker() = default;

    virtual void deploy(const unsigned long &ipAddress, const QByteArray &xml) = 0;
    virtual void stop() = 0;
    virtual void process(unsigned long ipAddress,
                         const DS::test::MessageHandler::SharedMemoryMessage &shmMsg) = 0;

    virtual WorkerType type() const;

signals:
    void complete(unsigned long ipAddress);
    void failed(unsigned long ipAddress, QString error);
    void finished();

protected:
    WorkerType type_;

    IncomingMsg parser(
    const DS::test::MessageHandler::SharedMemoryMessage &shmMsg);
};
}
}
