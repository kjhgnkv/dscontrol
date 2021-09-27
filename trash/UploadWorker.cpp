#include "UploadWorker.hpp"
#include <cmath>

#include "IncomingLocalMessage.hpp"
#include "LocalConnection.hpp"
#include "libraries/DSUtils/scopedPtr.h"
#include <QDebug>
#include <QMutex>
#include <QThread>
#include <QtConcurrent/QtConcurrent>

static int speed = 102400;
static int tryCount = 0;

void emitTimeoutAndStop(QTimer* timer)
{
    Q_ASSERT(timer);
    Q_ASSERT(QAbstractEventDispatcher::instance());

    if (!timer->isActive())
        return;
    timer->stop();
    timer->QTimer::qt_metacall(QMetaObject::InvokeMetaMethod, 5, {});
}

using namespace network::worker;

UploadWorker::UploadWorker(QObject* parent)
: DeployWorker(parent)
{
    type_ = WorkerType::Upload;
    connect(this, &AbstractWorker::complete, this, &AbstractWorker::stop);
    connect(&DSClientImpl::Instance(), &DSClientImpl::receiveNewUploadMessage, this, &AbstractWorker::process,
            Qt::QueuedConnection);

    timer_ = new QTimer {this};
    timer_->setInterval(2000);

    tryCount = 0;

    connect(timer_, &QTimer::timeout, [this]()
    {
        if (tryCount >= 5)
        {
            timer_->stop();
            emit failed(blockInfo_.ipAddress_, "Cannot receive package from server!");
            qWarning() << "Cannot receive package from server!";
            return;
        }

        if (blockInfo_.blockPos_ == BlockPos::End)
        {
            tryCount++;
        }
        else if (blockInfo_.blockPos_ == BlockPos::Current || blockInfo_.blockPos_ == BlockPos::Finish)
        {
            tryCount = 0;
        }
        uploadComponentBlock();

        emit progressValueChanged(QString::fromStdString(blockInfo_.filePath_), blockInfo_.ipAddress_,
                                  blockInfo_.blockId_ - blockInfo_.lastBlockId_);
        blockInfo_.lastBlockId_ = blockInfo_.blockId_;

        if (blockInfo_.blockPos_ == BlockPos::Finish)
        {
            emit complete(blockInfo_.ipAddress_);
            qDebug() << "Complete upload";
            return;
        }

        if (!timer_->isActive())
        {
            timer_->start();
        }
    });
}

UploadWorker::~UploadWorker()
{
    timer_->disconnect();
    timer_->deleteLater();
}

void UploadWorker::upload(const std::string &filename, unsigned int ipAddress, const std::string &componentName,
                          const std::string &uploadXml, DS::dsserver::CommandType::Enum type)
{
    blockInfo_.filePath_ = filename;
    blockInfo_.ipAddress_ = ipAddress;
    blockInfo_.componentName_ = componentName;
    blockInfo_.blockId_ = 0;
    blockInfo_.type_ = type;

    qDebug() << "Upload to " << blockInfo_.ipAddress_;

    if (blockInfo_.fileStream_.is_open())
    {
        blockInfo_.fileStream_.clear();
        blockInfo_.fileStream_.seekg(0, std::ios::beg);
        blockInfo_.fileStream_.close();
    }
    blockInfo_.fileStream_.open(blockInfo_.filePath_, std::ifstream::binary);
    if (blockInfo_.fileStream_.is_open())
    {
        auto fsize = blockInfo_.fileStream_.tellg();
        blockInfo_.fileStream_.seekg(0, std::ios::end);
        fsize = blockInfo_.fileStream_.tellg() - fsize;

        blockInfo_.blockIdMax_ = std::ceil((double)fsize / speed) - 1;
    }

    DSClientImpl::Instance().publish(ipAddress, uploadXml, DS::dsserver::CommandType::Enum::ReceiveComponent);
    timer_->start();
}

void UploadWorker::stop()
{
    timer_->stop();
    DeployWorker::stop();
}

void UploadWorker::process(unsigned long ipAddress, const DS::test::MessageHandler::SharedMemoryMessage &msg)
{
    if (ipAddress != blockInfo_.ipAddress_)
    {
        return;
    }

    // if (msg->msgType != "Component" && msg->msgType != "Application")
    // {
    //     // qDebug() << QString::fromStdString(msg->error());
    //     qDebug() << "UploadWorker::process ERROR";
    //     return;
    // }
    //
    // if (msg->block == "finish")
    // {
    //     qDebug() << "Get finish block";
    //     blockInfo_.blockPos_ = BlockPos::Finish;
    // }
    // else if (msg->block == "error")
    // {
    //     qDebug() << "Get error block";
    //     blockInfo_.blockPos_ = BlockPos::Error;
    //     // blockInfo_.lastError_ = msg->error();
    // }
    // else
    // {
    //     bool ok = false;
    //     uint64_t blockId = QVariant(QString::fromStdString(msg->block)).toULongLong(&ok);
    //
    //     if (!ok)
    //     {
    //         qCritical() << "Cannot parce block id!";
    //         return;
    //     }
    //
    //     qDebug() << "blockInfo_.blockId_" << blockInfo_.blockId_;
    //     blockInfo_.blockId_ = blockId;
    //     blockInfo_.blockPos_ = BlockPos::Current;
    // }
    // qDebug() << "Undefined: " << blockInfo_.blockId_;
    // qDebug() << "Undefined: " << QString::fromStdString(msg->block);
    //
    // emitTimeoutAndStop(timer_);
}

void UploadWorker::uploadComponentBlock()
{
    // qDebug() << "Upload component!";
    //
    // if (blockInfo_.blockPos_ == BlockPos::Finish)
    // {
    //     qDebug() << "Info block id: " << blockInfo_.blockId_;
    //     qDebug() << "BlockPos in finish!";
    //     blockInfo_.blockId_++;
    //     return;
    // }
    //
    // if (!blockInfo_.fileStream_.is_open())
    // {
    //     qDebug() << "Cannot open file stream!";
    //     return;
    // }
    //
    // const unsigned char sizeBlockId = sizeof(unsigned long long);
    //
    // std::vector<char> buffer(speed + sizeBlockId);
    // qDebug() << "speed + sizeBlockId = " << speed + sizeBlockId;
    //
    // std::ifstream::pos_type pos = static_cast<std::ifstream::pos_type>(blockInfo_.blockId_ * speed);
    // blockInfo_.fileStream_.seekg(pos, std::ifstream::beg);
    //
    // blockInfo_.fileStream_.read((char*)(buffer.data() + sizeBlockId), speed);
    // blockInfo_.lastMessageLength_ = blockInfo_.fileStream_.gcount();
    // if (blockInfo_.lastMessageLength_ == 0)
    // {
    //     qDebug() << "Cannot read file: " << QString::fromStdString(blockInfo_.filePath_);
    //     return;
    // }
    //
    // qDebug() << "\n\n--------------------------------------------";
    // qDebug() << "info.lastMessageLength_ = " << blockInfo_.lastMessageLength_;
    // qDebug() << "sizeBlockId = " << sizeBlockId;
    // qDebug() << "blockInfo_.blockId_ = " << blockInfo_.blockId_;
    //
    // memcpy(buffer.data(), (char*)(&blockInfo_.blockId_), sizeBlockId);
    // buffer.resize(sizeBlockId + blockInfo_.lastMessageLength_);
    //
    // auto test = *reinterpret_cast<const unsigned long*>(buffer.data());
    // qDebug() << test;
    // qDebug() << "buffer" << buffer.data();
    // qDebug() << "buffer" << buffer.size();
    //
    // qDebug() << "sizeBlockId + info.lastMessageLength_ = " << sizeBlockId + blockInfo_.lastMessageLength_;
    //
    // blockInfo_.blockPos_ = BlockPos::End;
    //
    // DSClientImpl::Instance().publish(blockInfo_.ipAddress_, buffer, blockInfo_.type_);
    //
    // qDebug() << "Complete block upload!";
}
