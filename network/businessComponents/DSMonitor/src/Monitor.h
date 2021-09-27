#pragma once

#include <Poco/SharedPtr.h>
#include <Poco/Mutex.h>
#include <runnable.h>

namespace dsmonitor
{
class Parser;
class ExportJSON;
class MonitorType;

class Monitor
{
public:
    explicit Monitor(int time = 1000);
    ~Monitor();

    // обновляет содержимое OS + Process's
    std::string update();

    // парсит xml
    bool parser(const std::string &xml);

    // ф-я для сервера, для получения времени
    long mscWaitMS() const;

private:
    const long msc_waitMS;

    Poco::SharedPtr<MonitorType> monitorType_;
    Poco::SharedPtr<Parser> parser_;
    Poco::SharedPtr<ExportJSON> export_;
    Poco::Mutex mutex_;
};
}