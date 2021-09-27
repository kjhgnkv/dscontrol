#include "Monitor.h"

#ifdef _WIN32

    #include "OS/Windows/WindowsMonitor.h"

#elif __linux

    #include "OS/Linux/LinuxMonitor.h"

#endif

// #include "Logger.h"
// using namespace DS::dsserver;

#include "DSMonitorHelp.hpp"

#include "ParserXML/Parser.h"
#include "ExportJson/ExportJSON.h"

#include "MonitorData.hpp"
#include "ProcessData.hpp"

namespace dsmonitor
{
// создаем монитор
// creating a monitor
Monitor::Monitor(int time)
: export_(new ExportJSON)
, parser_(new Parser)
, msc_waitMS(time)
{
    // определяем тип ОС Windows|Linux
    // defining the type of Windows|Linux OS
    #ifdef _WIN32
    monitorType_ = new WindowsMonitor();
    #elif __linux
    monitorType_ = new LinuxMonitor();
    #endif
}

// удаляем все
// delete all
Monitor::~Monitor()
{
    Help::removePtr(monitorType_);
    Help::removePtr(export_);
    Help::removePtr(parser_);
}

// возвращает время для Сервера
// returns the time for the Server
long Monitor::mscWaitMS() const
{
    return msc_waitMS;
}

// Сервер посылает запрос и получает содержимое json
// The server sends a request and receives the json content
std::string Monitor::update()
{
    // блокируем мютекс, что бы парсер не отработал в одно время
    // block the mutex, so that the parser does not work at the same time
    Poco::Mutex::ScopedLock lock(mutex_);
    try
    {
        // посылаем пропарсеные данные
        // sending parsed data
        MonitorData data = monitorType_->update(parser_->list());
        // получаем сформированныую json
        // getting the generated json
        std::string json = export_->exportJsonFile(data);

        return json;
    }
    catch (std::exception &ex)
    {
        // Logger::error(ex.what());
    }

    return std::string("");
}

// parser xml
bool Monitor::parser(const std::string &xml)
{
    // блокируем мютекс, что бы ф-я update не отработала
    // одновременно
    // blocking mutex, so that the f-I update does not work
    // at the same time
    Poco::Mutex::ScopedLock lock(mutex_);
    // отправляет содержимое в парсер
    // sends the content to the parser
    return parser_->parser(xml);
}
}