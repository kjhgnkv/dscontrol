#include "MonitorType.h"

#include "ParserXML/ParserData.hpp"

#if _WIN32
    #include <Shlobj.h>
#endif

#include "MonitorData.hpp"
#include "ProcessData.hpp"

namespace dsmonitor
{
// принимаем все xml с парсинга и парсим оправляем
// we accept all xml from parsing and send the parsing
MonitorData MonitorType::update(const std::vector<ParserData> &listXML)
{
    MonitorData monitorData;

    // getting the data memory
    getInfoMemory(monitorData.infoMemory);
    // getting the data storage
    getInfoStorage(monitorData.infoStorage);
    // getting the data system
    getInfoSystem(monitorData.infoSystem);
    // getting the data cpu
    getInfoCpu(monitorData.infoCpu);
    // getting the data network
    getInfoNetwork(monitorData.infoNetwork);

    // getting the data process's
    std::vector<ParserData>::const_iterator it, end;
    for (it = listXML.begin(), end = listXML.end(); it != end; ++it)
    {
        const ParserData &app = *it;
        #ifdef  _WIN32
        std::string nameApp = app.key + ".exe";
        #elif __linux
        std::string nameApp = app.key;
        #endif
        std::string pathApp = app.path;

        ProcessData proc;
        processes(nameApp, pathApp, proc);

        proc.xml.get(app);
        monitorData.process.insert(PairProcess(app.key, proc));
    }

    // return all data
    return monitorData;
}
}
