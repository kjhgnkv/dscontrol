#include "LinuxMonitor.h"

#include <Poco/DirectoryIterator.h>
#include <numeric>
#include <pwd.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <dirent.h>

#include "linux/memory.h"
#include "linux/cpu.h"
#include "linux/cache.h"
#include "linux/stat.h"
#include "linux/status.h"
#include "linux/osrelease.h"
#include "linux/mount.h"

namespace dsmonitor
{
// getting data about RAM
// в этой функции мы получаем данные о mem
bool LinuxMonitor::getInfoMemory(std::map<std::string, std::string> &infoMemory)
{
    struct linuxapi::memory umemory = {};
    // getting data about mem
    if (!linuxapi::memory(&umemory))
    {
        uint32_t totalPhys = umemory.memTotal;
        uint32_t freePhys = (umemory.memFree + umemory.cached);
        uint32_t usedPhys = (umemory.memTotal) - ((umemory.memFree + umemory.cached));
        uint32_t memoryLoad = (umemory.memFree + umemory.cached) * 100.0 / umemory.memTotal;

        // generating data
        infoMemory.insert(PairString(Help::Memory::toString(Help::Memory::total), Help::toString(totalPhys)));
        infoMemory.insert(PairString(Help::Memory::toString(Help::Memory::free), Help::toString(freePhys)));
        infoMemory.insert(PairString(Help::Memory::toString(Help::Memory::used), Help::toString(usedPhys)));
        infoMemory.insert(PairString(Help::Memory::toString(Help::Memory::load), Help::toString(memoryLoad)));

        return false;
    }

    return true;
}

// getting data about CPU
// в этой функции мы получаем данные о cpu
bool LinuxMonitor::getInfoCpu(std::map<std::string, std::string> &infoCpu)
{
    static int previous_idle_time;
    static int previous_total_time;

    int idle_time, total_time;
    cpuTimes(idle_time, total_time);
    const double idle_time_delta = double(idle_time) - double(previous_idle_time);
    const double total_time_delta = double(total_time) - double(previous_total_time);

    previous_idle_time = idle_time;
    previous_total_time = total_time;

    struct linuxapi::cpu ucpu = {};
    // getting data about cpu
    if (linuxapi::cpu(&ucpu))
    {
        return true;
    }

    double usage = double(100.0) * (1.0 - idle_time_delta / total_time_delta);
    std::string nameCpu = std::string(ucpu.model_name);
    uint32_t cpuCores = ucpu.cpu_cores;
    uint32_t cpuThreads = ucpu.siblings;
    uint32_t min = ucpu.min / 1000;
    uint32_t max = ucpu.max / 1000;

    // generating data
    infoCpu.insert(PairString(Help::Cpu::toString(Help::Cpu::name), nameCpu));
    infoCpu.insert(PairString(Help::Cpu::toString(Help::Cpu::usage), Help::toString(usage)));
    infoCpu.insert(PairString(Help::Cpu::toString(Help::Cpu::cores), Help::toString(cpuCores)));
    infoCpu.insert(PairString(Help::Cpu::toString(Help::Cpu::threads), Help::toString(cpuThreads)));
    infoCpu.insert(PairString(Help::Cpu::toString(Help::Cpu::min), Help::toString(min)));
    infoCpu.insert(PairString(Help::Cpu::toString(Help::Cpu::max), Help::toString(max)));

    struct linuxapi::cache ucache = {};
    // getting data about cache
    if (!linuxapi::cache(&ucache))
    {
        std::string cache;
        std::vector<linuxapi::cache_info>::const_iterator it, end;
        for (it = ucache.cache_list.begin(), end = ucache.cache_list.end(); it != end; it++)
        {
            struct linuxapi::cache_info info = *it;
            unsigned int level = info.level;
            std::string type = info.type;
            unsigned int size = info.size;
            unsigned int way = info.way;
            unsigned int sets = info.sets;

            cache += "Level " + Poco::Dynamic::Var(level).toString() + " (" + type + ") " + Poco::Dynamic::Var(
            size).toString() + "KB, " + Poco::Dynamic::Var(
            way).toString() + "-way set-associative, " + Poco::Dynamic::Var(sets).toString() + " sets\n";
        }

        infoCpu.insert(PairString(Help::Cpu::toString(Help::Cpu::cache), Help::toString(cache)));
    }

    return false;
}

// getting data about Storage
// в этой функции мы получаем данные о storage
bool LinuxMonitor::getInfoStorage(std::map<std::string, std::string> &infoStorage)
{
    struct linuxapi::mount umount = {};
    // getting data about storage
    if (!linuxapi::mount(&umount, exePath()))
    {
        infoStorage.insert(PairString(Help::Storage::toString(Help::Storage::free),
                                      Help::toString(umount.total - umount.used)));
        infoStorage.insert(PairString(Help::Storage::toString(Help::Storage::total), Help::toString(umount.total)));
        infoStorage.insert(PairString(Help::Storage::toString(Help::Storage::available),
                                      Help::toString(umount.available)));
        infoStorage.insert(PairString(Help::Storage::toString(Help::Storage::used), Help::toString(umount.used)));
        infoStorage.insert(PairString(Help::Storage::toString(Help::Storage::device), umount.device));
        double t_load = 100.0 / double(umount.total / umount.available);
        infoStorage.insert(PairString(Help::Storage::toString(Help::Storage::load), Help::toString(t_load)));
        infoStorage.insert(PairString(Help::Storage::toString(Help::Storage::used), Help::toString(umount.usage)));
        infoStorage.insert(PairString(Help::Storage::toString(Help::Storage::type), umount.fstype));
        infoStorage.insert(PairString(Help::Storage::toString(Help::Storage::path), umount.destination));

        return false;
    }

    return true;
}

// getting data about System
bool LinuxMonitor::getInfoSystem(std::map<std::string, std::string> &infoSystem)
{
    struct linuxapi::osrelease os = {};
    if (linuxapi::osrelease(&os))
    {
        // Logger::warning("no access to 'osrelease'");
        return true;
    }

    struct utsname sname = {};
    if (uname(&sname))
    {
        // Logger::warning("no access to 'utsname'");
        return true;
    }

    infoSystem.insert(PairString(Help::System::toString(Help::System::id), std::string(os.id)));
    infoSystem.insert(PairString(Help::System::toString(Help::System::architecture), std::string(sname.machine)));
    infoSystem.insert(PairString(Help::System::toString(Help::System::kernelType), std::string(sname.sysname)));
    infoSystem.insert(PairString(Help::System::toString(Help::System::productName), std::string(os.version)));

    return false;
}

// returning the path to the application
std::string LinuxMonitor::exePath()
{
    char argc[20];
    char exepath[PATH_MAX + 1] = {0};

    sprintf(argc, "/proc/%d/exe", getpid());
    readlink(argc, exepath, 1024);
    return std::string(exepath);
}

// getting the time for the cpu
std::vector<int> LinuxMonitor::getCpuTimes()
{
    std::ifstream proc_stat("/proc/stat");
    proc_stat.ignore(5, ' ');
    std::vector<int> times;

    for (size_t time; proc_stat >> time; times.push_back(time));
    return times;
}

// getting the time
bool LinuxMonitor::cpuTimes(int &idleTime, int &totalTime)
{
    const std::vector<int> cpu_times = getCpuTimes();
    if (cpu_times.size() < 4)
    {
        return true;
    }

    idleTime = cpu_times[3];
    totalTime = std::accumulate(cpu_times.begin(), cpu_times.end(), 0);

    return false;
}

// looking for process c by name and path
bool LinuxMonitor::processes(const std::string &procNameApp, const std::string &procPathApp, ProcessData &processData)
{
    for (Poco::DirectoryIterator it(std::string("/proc")), end; it != end; ++it)
    {
        std::string path = it->path() + std::string("/");
        std::string cmd = cmdline(path);
        std::string app = procPathApp + procNameApp;

        if (strcmp(app.c_str(), cmd.c_str()) == 0)
        {
            return processInfo(processData, path);
        }
    }

    return true;
}

// making a calculation to get how much CPU is used
double LinuxMonitor::calcCpuUsage(const double &uTime, const double &sTime, const double &cuTime, const double &csTime,
                                  const double &startTime)
{
    double Hertz = double(sysconf(_SC_CLK_TCK));
    double total_time = uTime + sTime;
    double seconds = upTime() - (startTime / Hertz);
    double cpu_usage = 100 * (((total_time + cuTime + csTime) / Hertz) / seconds);

    return cpu_usage / sysconf(_SC_NPROCESSORS_ONLN);
}

// getting information about the process
bool LinuxMonitor::processInfo(ProcessData &processData, const std::string &pid)
{
    struct linuxapi::status ustatus = {};
    if (linuxapi::status(&ustatus, pid))
    {
        return true;
    }

    struct linuxapi::stat ustat = {};
    if (linuxapi::stat(&ustat, pid))
    {
        return true;
    }

    processData.process.insert(PairString(Help::Process::toString(Help::Process::pid), Help::toString(ustat.pid)));
    processData.process.insert(PairString(Help::Process::toString(Help::Process::ppid), Help::toString(ustat.ppid)));
    processData.process.insert(PairString(Help::Process::toString(Help::Process::name), std::string(ustatus.name)));
    processData.process.insert(PairString(Help::Process::toString(Help::Process::userName), userName()));
    processData.process.insert(PairString(Help::Process::toString(Help::Process::status), "true"));
    processData.process.insert(
    PairString(Help::Process::toString(Help::Process::threads), Help::toString(ustatus.threads)));
    processData.process.insert(
    PairString(Help::Process::toString(Help::Process::workingSetSize), Help::toString(ustatus.vmRSS)));
    processData.process.insert(PairString(Help::Process::toString(Help::Process::path), cmdline(pid)));

    struct linuxapi::memory umemory = {};
    if (!linuxapi::memory(&umemory))
    {
        double memoryLoad = (ustatus.vmRSS * 100.0 / (umemory.memFree + umemory.cached) * 100.0 / umemory.memTotal);
        processData.process.insert(PairString(Help::Process::toString(Help::Process::memoryLoad), Help::toString(memoryLoad)));
    }

    double cpuUsage = calcCpuUsage(double(ustat.utime), double(ustat.stime), ustat.cutime, ustat.cstime,
                                   double(ustat.starttime));
    processData.process.insert(PairString(Help::Process::toString(Help::Process::usageCPU), Help::toString(cpuUsage)));

    threadInfo(processData.thread, pid);

    return false;
}

// getting information about processor threads
bool LinuxMonitor::threadInfo(std::vector<MapString> &infoThreads, const std::string &pid)
{
    try
    {
        std::string path = pid + "task/";
        for (Poco::DirectoryIterator it(path), end; it != end; ++it)
        {
            std::string file = it->path() + std::string("/");

            struct linuxapi::stat ustat = {};
            if (!linuxapi::stat(&ustat, file))
            {
                MapString mapString;
                mapString.insert(PairString(Help::Thread::toString(Help::Thread::pid), Help::toString(ustat.pid)));
                mapString.insert(PairString(Help::Thread::toString(Help::Thread::ppid), Help::toString(ustat.ppid)));
                double usage = calcCpuUsage(double(ustat.utime), double(ustat.stime), ustat.cutime, ustat.cstime,
                                            double(ustat.starttime));
                mapString.insert(PairString(Help::Thread::toString(Help::Thread::usage), Help::toString(usage)));

                infoThreads.push_back(mapString);
            }
        }

        return false;
    }
    catch (Poco::FileNotFoundException &ex)
    {
        return true;
    }
}

// reading the file along the path + cmdline
std::string LinuxMonitor::cmdline(const std::string &path)
{
    std::string str;
    std::string fileName = path + "cmdline";

    if (!openFile(fileName, str))
    {
        return str;
    }

    return std::string();
}

// getting the user name
std::string LinuxMonitor::userName()
{
    uid_t uid = geteuid();
    passwd* pw = getpwuid(uid);

    if (!pw)
    {
        return std::string();
    }

    return pw->pw_gecos;
}

// getting the time
double LinuxMonitor::upTime()
{
    struct sysinfo sinfo = {};
    if (sysinfo(&sinfo))
    {
        return 0;
    }

    return sinfo.uptime;
}

// opening the file for reading
bool LinuxMonitor::openFile(const std::string &file, std::string &str)
{
    std::ifstream fs;
    std::string line;

    fs.open(file.c_str());

    if (!fs.is_open())
    {
        return true;
    }

    str.clear();
    while (getline(fs, line))
    {
        str += line;
    }

    fs.close();

    return false;
}
}