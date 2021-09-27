#pragma once

#include <sstream>
#include "../../MonitorType.h"
#include "../../ProcessData.hpp"

namespace linuxapi
{
struct memory;
struct cpu;
}

namespace dsmonitor
{
class LinuxMonitor : public MonitorType
{
public:
    bool processes(const std::string &procNameApp, const std::string &procPathApp, ProcessData &processData);
    bool getInfoMemory(std::map<std::string, std::string> &infoMemory);
    bool getInfoCpu(std::map<std::string, std::string> &infoCpu);
    bool getInfoStorage(std::map<std::string, std::string> &infoStorage);
    bool getInfoSystem(std::map<std::string, std::string> &infoSystem);

private:
    static bool openFile(const std::string &file, std::string &str);
    static std::vector<int> getCpuTimes();
    static bool cpuTimes(int &idleTime, int &totalTime);

    static double calcCpuUsage(const double &uTime, const double &sTime, const double &cuTime, const double &csTime,
                               const double &startTime);

    static bool processInfo(ProcessData &processData, const std::string &pid);

    static bool threadInfo(std::vector<MapString> &infoThreads, const std::string &pid);

    static std::string userName();

    static std::string cmdline(const std::string &path);
    static double upTime();
    static std::string exePath();
};
}