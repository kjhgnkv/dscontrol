#include "AbstractWorker.hpp"
#include "Logger/Logger.h"

using namespace network::worker;
using namespace DS::test;

AbstractWorker::AbstractWorker(QObject* parent)
: QObject(parent)
, type_ {WorkerType::None}
{
    connect(this, &AbstractWorker::failed, this, &AbstractWorker::stop);
}

network::WorkerType AbstractWorker::type() const
{
    return type_;
}

IncomingMsg AbstractWorker::parser(const DS::test::MessageHandler::SharedMemoryMessage &shmMsg)
{
    if (!shmMsg.ip.empty())
    {
        return {};
    }

    std::string err;
    Poco::SharedPtr<DS::dscontrolProvider::IncomingMsg> incomingMsg = DS::dscontrolProvider::IncomingMsg::parse(
    shmMsg.msg, err);
    if (!incomingMsg)
    {
        Logger::instance().log(std::string(
        "can not parse raw message: ") + (!shmMsg.msg.empty() ? "NULL" : (+", ip=" + shmMsg.ip + ", msg size=" + Poco::Dynamic::Var(
        shmMsg.msg.size()).toString())), ds::utils::Logger::Warning);

        return {};
    }

    IncomingMsg msg;

    msg.ip = shmMsg.ip;
    msg.msg = shmMsg.msg;

    // msg.block = incomingMsg->block;
    // msg.appType = incomingMsg->appType;
    // msg.msgType = incomingMsg->msgType;

    return msg;
}