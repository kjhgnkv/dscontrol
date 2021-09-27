#include "ExportJSON.h"
#include <algorithm>

#include <DSServerGlobal.h>
#include "../MonitorData.hpp"
#include "../ProcessData.hpp"

#include "../DSMonitorHelp.hpp"

using namespace Poco::Dynamic;

namespace dsmonitor
{
// передаем сюда по ссылке объект json, ключ и данные
// passing the json object, key, and data here by reference
void ExportJSON::addObjectPtr(JsonObj::Ptr &prt, const std::string &key, const Poco::Dynamic::Var &value)
{
    prt->set(key, value);
}

// передаем сюда по ссылке объект, контейнер с данными "ключ/данные"
// we pass here an object, a container with the data "key/data" by reference"
void ExportJSON::addObjectPtr(Poco::JSON::Object::Ptr &prt, const MapString &data)
{
    MapString::const_iterator it, end;
    for (it = data.begin(), end = data.end(); it != end; it++)
    {
        prt->set(it->first, it->second);
    }
}

// делаем експорт всех данных в json
// making an export of all data in json
std::string ExportJSON::exportJsonFile(const MonitorData &data)
{
    // создаем обхект json
    JsonObj::Ptr root = new JsonObj(true);

    // добавляем данные, что бы сервер понимал что принимает
    // adding data so that the server understands what it accepts
    root->set(Help::DSMonitor::toString(Help::DSMonitor::appType),
              DS::dsserver::ApplicationType::toString(DS::dsserver::ApplicationType::DSMonitor));
    root->set(Help::DSMonitor::toString(Help::DSMonitor::msgType), Help::DSMonitor::toString(Help::DSMonitor::status));

    // ----------------- NETWORK -----------------
    {
        // формируем данные NETWORK
        // generating data NETWORK
        Poco::JSON::Object::Ptr Network = new Poco::JSON::Object(true);
        addObjectPtr(Network, data.infoNetwork);
        std::string keyNetwork = Help::KeySystem::toString(Help::KeySystem::network);
        root->set(keyNetwork, Network);
    }


    // ----------------- CPU -----------------
    {
        // формируем данные CPU
        // generating data CPU
        JsonObj::Ptr CPU = new JsonObj(true);
        addObjectPtr(CPU, data.infoCpu);
        std::string keyCPU = Help::KeySystem::toString(Help::KeySystem::cpu);
        root->set(keyCPU, CPU);
    }


    // ----------------- MEMORY -----------------
    {
        // формируем данные MEMORY
        // generating data MEMORY
        JsonObj::Ptr Memory = new JsonObj(true);
        addObjectPtr(Memory, data.infoMemory);
        std::string keyMemory = Help::KeySystem::toString(Help::KeySystem::memory);
        root->set(keyMemory, Memory);
    }


    // ----------------- STORAGE -----------------
    {
        // формируем данные STORAGE
        // generating data STORAGE
        JsonObj::Ptr Storage = new JsonObj(true);
        addObjectPtr(Storage, data.infoStorage);
        std::string keyStorage = Help::KeySystem::toString(Help::KeySystem::storage);
        root->set(keyStorage, Storage);
    }


    // ----------------- SYSTEM -----------------
    {
        // формируем данные SYSTEM
        // generating data SYSTEM
        JsonObj::Ptr System = new JsonObj(true);
        addObjectPtr(System, data.infoSystem);
        std::string keySystem = Help::KeySystem::toString(Help::KeySystem::system);
        root->set(keySystem, System);
    }


    // ----------------- PROCESS'S -----------------
    {
        // формируем данные PROCESS'S
        // generating data PROCESS'S
        JsonArray::Ptr rootArrayProcess = new JsonArray(true);

        std::vector<ParserData>::const_iterator it_app, end_app;

        int count = 0;
        std::map<std::string, ProcessData>::const_iterator it, end;
        // переберираем все пропарсенные данные
        // going through all the parsed data
        for (it = data.process.begin(), end = data.process.end(); it != end; it++)
        {
            JsonObj::Ptr objProcess = new JsonObj(true);

            // если не пустое, то, добавляем все данные
            // if not empty, then add all the data
            if (!it->second.process.empty())
            {
                // Process
                {
                    JsonObj::Ptr process = new JsonObj(true);
                    addObjectPtr(process, it->second.process);
                    std::string keyProcess = Help::KeySystem::toString(Help::KeySystem::process);
                    objProcess->set(keyProcess, process);
                }

                // Thread
                {
                    JsonArray::Ptr arrayThread = new JsonArray(true);

                    int count_thread = 0;
                    std::vector<MapString>::const_iterator it_thread, end_thread;
                    for (it_thread = it->second.thread.begin(), end_thread = it->second.thread.end(); it_thread != end_thread; it_thread++)
                    {
                        JsonObj::Ptr thread = new JsonObj(true);
                        MapString mapStringThread = *it_thread;
                        addObjectPtr(thread, mapStringThread);

                        arrayThread->set(count_thread, thread);
                        ++count_thread;
                    }

                    std::string keyThread = Help::KeySystem::toString(Help::KeySystem::threads);
                    objProcess->set(keyThread, arrayThread);
                }

                // XML
                {
                    JsonObj::Ptr xml = new JsonObj(true);

                    std::string keyName = Help::XML::toString(Help::XML::key);
                    addObjectPtr(xml, keyName, it->second.xml.key);

                    std::string keyPath = Help::XML::toString(Help::XML::path);
                    addObjectPtr(xml, keyPath, it->second.xml.path);

                    std::string keyXml = Help::XML::toString(Help::XML::xml);
                    addObjectPtr(xml, keyXml, it->second.xml.xml);

                    std::string keyXML = Help::KeySystem::toString(Help::KeySystem::xml);
                    objProcess->set(keyXML, xml);
                }
            }
            else
            {
                // если процесс не задплоен или апп кританула
                // то, добавляем данные "апп/путь/xml"
                // if the process is not set or the app is blocked
                // then, add the data " app/path/xml"

                // Process
                {
                    JsonObj::Ptr process = new JsonObj(true);
                    std::string keyName = Help::Process::toString(Help::Process::name);
                    addObjectPtr(process, keyName, it->second.xml.key);

                    std::string keyError = Help::Error::toString(Help::Error::error);
                    std::string messageError = Help::Error::message();
                    addObjectPtr(process, keyError, messageError);

                    std::string keyProcess = Help::KeySystem::toString(Help::KeySystem::process);
                    objProcess->set(keyProcess, process);
                }

                // XML
                {
                    JsonObj::Ptr xml = new JsonObj(true);

                    std::string keyName = Help::XML::toString(Help::XML::key);
                    addObjectPtr(xml, keyName, it->second.xml.key);

                    std::string keyPath = Help::XML::toString(Help::XML::path);
                    addObjectPtr(xml, keyPath, it->second.xml.path);

                    std::string keyXml = Help::XML::toString(Help::XML::xml);
                    addObjectPtr(xml, keyXml, it->second.xml.xml);

                    std::string keyXML = Help::KeySystem::toString(Help::KeySystem::xml);
                    objProcess->set(keyXML, xml);
                }
            }

            rootArrayProcess->set(count++, objProcess);
        }

        std::string keyProcess = Help::KeySystem::toString(Help::KeySystem::process);
        root->set(keyProcess, rootArrayProcess);
    }

    // generating json and returning
    std::ostringstream os;
    root->stringify(os, 4);

    return os.str();
}
}