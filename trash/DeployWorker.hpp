#pragma once

#include "AbstractWorker.hpp"

class DeployWorker : public AbstractWorker
{
Q_OBJECT
protected:
    explicit DeployWorker(QObject* parent = nullptr);

public:
    static DeployWorker *init(QObject* parent = nullptr)
    {
        if (deployWorker_ == nullptr)
        {
            deployWorker_ = new DeployWorker{parent};
        }

        return deployWorker_;
    }

    ~DeployWorker() = default;

    void deploy(const unsigned long &ipAddress, const QByteArray &xml) override;
    void stop() override;
    void process(unsigned long ipAddress, const DS::test::MessageHandler::SharedMemoryMessage &shmMsg) override;

    virtual QVariant response() const;

private:
    static DeployWorker *deployWorker_;
protected:
    QVariant responce_;
};