#include "WindowsMonitor.h"
#include <Windows.h>

#include <winbase.h>
#include <Poco/Path.h>
#include <VersionHelpers.h>
#include <TlHelp32.h>
#include <intrin.h>
#include <sstream>
#include <Psapi.h>

#include <Poco/Dynamic/Var.h>

#pragma comment(lib, "user32.lib")

#if _WIN32
    #    ifndef PROCESSOR_ARCHITECTURE_ARM64
        #        define PROCESSOR_ARCHITECTURE_ARM64 12
    #    endif
#endif

using namespace dsmonitor::Help;
using namespace Poco::Dynamic;

namespace dsmonitor
{
// getting data about RAM
bool WindowsMonitor::getInfoMemory(std::map<std::string, std::string> &infoMemory)
{
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    if (!GlobalMemoryStatusEx(&statex))
    {
        return true;
    }

    infoMemory.insert(PairString(Help::Memory::toString(Help::Memory::total), toString(statex.ullTotalPhys)));
    infoMemory.insert(PairString(Help::Memory::toString(Help::Memory::free), toString(statex.ullAvailPhys)));
    infoMemory.insert(
    PairString(Help::Memory::toString(Help::Memory::used), toString(statex.ullTotalPhys - statex.ullAvailPhys)));
    infoMemory.insert(PairString(Help::Memory::toString(Help::Memory::load),
                                 toString(statex.ullAvailPhys * 100.0 / statex.ullTotalPhys)));

    return false;
}

// getting data about OsVersion
std::string WindowsMonitor::GetOsVersionName()
{
    if (IsWindows10OrGreater())
    {
        return "10.0";
    }
    if (IsWindows8Point1OrGreater())
    {
        return "8.1";
    }
    if (IsWindows8OrGreater())
    {
        return "8";
    }
    if (IsWindows7OrGreater())
    {
        return "7";
    }
    if (IsWindowsVistaOrGreater())
    {
        return "Vista";
    }
    if (IsWindowsXPOrGreater())
    {
        return "XP";
    }
    return "Unknown";
}

// getting data about System
bool WindowsMonitor::getInfoSystem(std::map<std::string, std::string> &infoSystem)
{
    SYSTEM_INFO systemInfo;
    GetNativeSystemInfo(&systemInfo);
    std::string CpuArchitecture;

    switch (systemInfo.wProcessorArchitecture)
    {
        case PROCESSOR_ARCHITECTURE_INTEL:
        {
            CpuArchitecture = "x86";
            break;
        }
        case PROCESSOR_ARCHITECTURE_AMD64:
        {
            CpuArchitecture = "x64";
            break;
        }
        case PROCESSOR_ARCHITECTURE_ARM:
        {
            CpuArchitecture = "ARM";
            break;
        }
        case PROCESSOR_ARCHITECTURE_ARM64:
        {
            CpuArchitecture = "ARM64";
            break;
        }
        case PROCESSOR_ARCHITECTURE_IA64:
        {
            CpuArchitecture = "Intel Itanium-based";
            break;
        }
        case PROCESSOR_ARCHITECTURE_UNKNOWN:
        {
            CpuArchitecture = "Unknown";
            break;
        }
    }

    infoSystem.insert(PairString(Help::System::toString(Help::System::id), std::string(GetOsVersionName())));
    infoSystem.insert(PairString(Help::System::toString(Help::System::architecture), std::string(CpuArchitecture)));
    infoSystem.insert(PairString(Help::System::toString(Help::System::kernelType), std::string("WinNT")));
    infoSystem.insert(PairString(Help::System::toString(Help::System::productName), std::string(getProductName())));

    return false;
}

// getting data about Storage
bool WindowsMonitor::getInfoStorage(std::map<std::string, std::string> &infoStorage)
{
    std::string tmp = Poco::Path::current();
    std::string rootPath = tmp.substr(0, 3);

    char volumeName[MAX_PATH];
    char fileSystemType[MAX_PATH];

    DWORD volumeSerialNumber;
    DWORD maximumComponentLength;
    DWORD fileSystemFlags;

    if (!GetVolumeInformationA(rootPath.c_str(), volumeName, sizeof(volumeName), &volumeSerialNumber,
                               &maximumComponentLength, &fileSystemFlags, fileSystemType, sizeof(fileSystemType)))
    {
        return true;
    }

    ULARGE_INTEGER freeBytesAvailableToCaller;
    ULARGE_INTEGER totalNumberOfBytes;
    ULARGE_INTEGER totalNumberOfFreeBytes;

    if (GetDiskFreeSpaceExA(rootPath.c_str(), &freeBytesAvailableToCaller, &totalNumberOfBytes,
                            &totalNumberOfFreeBytes))
    {
        infoStorage.insert(
        PairString(Help::Storage::toString(Help::Storage::total), toString(totalNumberOfBytes.QuadPart)));
        infoStorage.insert(
        PairString(Help::Storage::toString(Help::Storage::available), toString(freeBytesAvailableToCaller.QuadPart)));
        infoStorage.insert(PairString(Help::Storage::toString(Help::Storage::used),
                                      toString(totalNumberOfBytes.QuadPart - freeBytesAvailableToCaller.QuadPart)));
        infoStorage.insert(PairString(Help::Storage::toString(Help::Storage::type), fileSystemType));
    }
    else
    {
        return true;
    }

    std::string tmpVolumeMountPoint = rootPath;
    std::wstring wide_string = std::wstring(tmpVolumeMountPoint.begin(), tmpVolumeMountPoint.end());
    const wchar_t* volumeNamePoint = wide_string.c_str();

    const DWORD bufferLength = 50;
    WCHAR volumeGuid[bufferLength];
    if (GetVolumeNameForVolumeMountPointW(volumeNamePoint, volumeGuid, bufferLength))
    {
        infoStorage.insert(PairString(Help::Storage::toString(Help::Storage::device), toString(U16toU8(volumeGuid))));
        infoStorage.insert(PairString(Help::Storage::toString(Help::Storage::path), rootPath));
    }
    else
    {
        return true;
    }

    double usage = double(totalNumberOfBytes.QuadPart - freeBytesAvailableToCaller.QuadPart) * 100.0 / double(
    totalNumberOfBytes.QuadPart);
    infoStorage.insert(PairString(Help::Storage::toString(Help::Storage::used), toString(usage)));

    return false;
}

// getting data about CPU
bool WindowsMonitor::getInfoCpu(std::map<std::string, std::string> &infoCpu)
{
    static ULARGE_INTEGER previousIdleTime, previousKernelTime, previousUserTime;

    /** Getting the following system times **/
    ULARGE_INTEGER followingIdleTime, followingKernelTime, followingUserTime;
    try
    {
        getSystemTimes(&followingIdleTime, &followingKernelTime, &followingUserTime);
    }
    catch (std::runtime_error &ex)
    {
        std::string error = "Can't get the cpu usage, because can't " + std::string(ex.what());
        // std::cerr << (error);
        return true;
    }
    /** Calculating the useful CPU time **/
    ULARGE_INTEGER usefulCPUTime;

    usefulCPUTime.QuadPart = (followingKernelTime.QuadPart - previousKernelTime.QuadPart) + (followingUserTime.QuadPart - previousUserTime.QuadPart) - (followingIdleTime.QuadPart - previousIdleTime.QuadPart);

    /** Calculating the total CPU time **/
    ULARGE_INTEGER totalCPUTime;
    totalCPUTime.QuadPart = (followingKernelTime.QuadPart - previousKernelTime.QuadPart) + (followingUserTime.QuadPart - previousUserTime.QuadPart);

    /** Getting the InfoCpu **/
    double usage = usefulCPUTime.QuadPart * 100.0 / totalCPUTime.QuadPart;
    infoCpu.insert(PairString(Help::Cpu::toString(Help::Cpu::usage), toString(usage)));

    /** Chanding the previous system times on following **/
    try
    {
        getSystemTimes(&previousIdleTime, &previousKernelTime, &previousUserTime);
    }
    catch (std::runtime_error &ex)
    {
        return true;
    }

    SYSTEM_INFO siSysInfo;
    GetSystemInfo(&siSysInfo);

    infoCpu.insert(PairString(Help::Cpu::toString(Help::Cpu::name), ProcessorName()));

    // TODO
    infoCpu.insert(PairString(Help::Cpu::toString(Help::Cpu::min), toString(0)));
    infoCpu.insert(PairString(Help::Cpu::toString(Help::Cpu::max), toString(0)));

    std::vector<PCACHE_DESCRIPTOR> vec;
    if (getVecCache(vec))
    {
        std::string cashs;
        for (int i = 0; i < vec.size(); i++)
        {
            std::string cache = "Level " + Var(
            static_cast<int>(vec.at(i)->Level)).toString() + " (" + ProcessorCacheTypeToString(
            vec.at(i)->Type) + ") " + Var(vec.at(i)->Size / 1024).toString() + "KB, " + Var(
            static_cast<int>(vec.at(i)->Associativity)).toString() + "-way set-associative, " + Var(
            vec.at(i)->LineSize).toString() + "-byte line size";
            cashs += cache + "\n";
        }
    }

    double numberCPUCores = static_cast<u_int>(siSysInfo.dwNumberOfProcessors);
    double numberCPUThreads = static_cast<u_int>(siSysInfo.dwNumberOfProcessors);
    infoCpu.insert(PairString(Help::Cpu::toString(Help::Cpu::cores), toString(numberCPUCores)));
    infoCpu.insert(PairString(Help::Cpu::toString(Help::Cpu::threads), toString(numberCPUThreads)));

    return false;
}

// looking for process c by name and path
bool WindowsMonitor::processes(const std::string &procNameApp, const std::string &procPathApp, ProcessData &processData)
{
    PROCESSENTRY32 processEntry;
    processEntry.dwSize = sizeof(processEntry);

    /** Сreating the process snapshot handle **/

    HANDLE handleSnapshot = INVALID_HANDLE_VALUE;
    do
    {
        handleSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    } while (handleSnapshot == INVALID_HANDLE_VALUE);

    /** Investigating the snapshot handle on processes **/
    bool areThereProcesses = Process32First(handleSnapshot, &processEntry);
    std::string currentProcessPath;

    HANDLE processHandle = INVALID_HANDLE_VALUE;
    HANDLE processTokenHandle = INVALID_HANDLE_VALUE;

    /** Determine the OS platform version to set the needed privilege **/
    OSVERSIONINFO osvi;
    osvi.dwOSVersionInfoSize = sizeof(osvi);
    GetVersionExA(&osvi);

    /** Set the privilege if this is Windows NT **/
    if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
    {
        try
        {
            setPrivilege(processTokenHandle, GetCurrentProcess(), SE_DEBUG_NAME, TRUE);
        }
        catch (std::runtime_error &ex)
        {
            std::string error = "Can't set the needed priviliges to the "
                                "token of the current process, because " + std::string(ex.what()) + ".";
            // std::cerr << (error);
        }
    }

    /** Searching for the needed process by processPath **/
    do
    {
        /** Openning the process handle **/
        processHandle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, FALSE, static_cast<DWORD>(processEntry.th32ProcessID));
        if (processHandle == NULL)
        {
            /** Freeing up resources **/
            CloseHandle(processHandle);
            areThereProcesses = Process32Next(handleSnapshot, &processEntry);
            continue;
        }

        try
        {
            currentProcessPath = getProcessPath(processHandle);
        }
        catch (std::exception &ex)
        {
            currentProcessPath = std::string();
        }

        if (currentProcessPath == procPathApp + procNameApp)
        {
            processData.process.insert(PairString(Help::Process::toString(Help::Process::pid), toString(processEntry.th32ProcessID)));
            processData.process.insert(PairString(Help::Process::toString(Help::Process::ppid), toString(processEntry.th32ParentProcessID)));
            processData.process.insert(PairString(Help::Process::toString(Help::Process::name), std::string(processEntry.szExeFile)));
            processData.process.insert(PairString(Help::Process::toString(Help::Process::status), "true"));
            processData.process.insert(PairString(Help::Process::toString(Help::Process::threads), toString(processEntry.cntThreads)));
            processData.process.insert(PairString(Help::Process::toString(Help::Process::path), procPathApp));

            try
            {
                processData.process.insert(PairString(Help::Process::toString(Help::Process::userName), getProcessUserName(processHandle)));
            }
            catch (std::runtime_error &ex)
            {
                // std::string error = "Can't get the process username, because can't" + std::string(ex.what()) + "Process name: " + proc->name + ", pid: " + std::to_string(proc->pid) + ". ";
                // std::cerr << (error);
            }

            try
            {
                processData.process.insert(PairString(Help::Process::toString(Help::Process::workingSetSize), toString(getProcessWorkingSetSize(processHandle))));
            }
            catch (std::runtime_error &ex)
            {
                // std::string error = "Can't get the process working set size, "
                //                     "because can't " + std::string(ex.what()) + " Process name: " + proc->name + ", pid: " + toString(processEntry.th32ProcessID) + ". ";
                // std::cerr << (error);
            }
            // TODO
            // proc->memoryLoad = proc->workingSetSize * 100.0 / (*infoMachine_).infoMemory->totalPhys;
            // processData.process.insert(PairString(Help::Process::toString(Help::Process::memoryLoad), procPathApp));

            // WindowsMonitorCompare(proc->pid);
            // if (foundedProcess != previousProcesses->second.end())
            // {
            // proc->kernelTime = (*foundedProcess)->kernelTime;
            // proc->userTime = (*foundedProcess)->userTime;
            // proc->lastSystemTime = (*foundedProcess)->lastSystemTime;
            try
            {
                // TODO
                // proc->cpuUsage = getProcessCPUUsage(processHandle, proc->kernelTime, proc->userTime, proc->lastSystemTime);
                // processData.process.insert(PairString(Help::Process::toString(Help::Process::usageCPU), toString(cpuUsage)));
            }
            catch (std::runtime_error &ex)
            {
                // std::string error = "Can't get the process cpu "
                //                     "usage, because can't " + std::string(ex.what()) + " Process name: " + proc->name + ", pid: " + std::to_string(proc->pid);
                // std::cerr << (error);
            }

            if (!getProcessThreads(processEntry.th32ProcessID, processEntry.szExeFile, processData.thread))
            {
                CloseHandle(processTokenHandle);
                CloseHandle(handleSnapshot);

                return false;
            }
        }
        areThereProcesses = Process32Next(handleSnapshot, &processEntry);
        CloseHandle(processHandle);
    } while (areThereProcesses);

    /** Unset the Privilege **/
    try
    {
        setPrivilege(processTokenHandle, GetCurrentProcess(), SE_DEBUG_NAME, FALSE);
    }
    catch (std::runtime_error &ex)
    {
        /** Freing up resourses **/
        CloseHandle(processTokenHandle);
        CloseHandle(handleSnapshot);

        return true;
    }

    /** Freing up resourses **/
    CloseHandle(processTokenHandle);
    CloseHandle(handleSnapshot);

    return true;
}

// get path precess
std::string WindowsMonitor::getProcessPath(HANDLE handleProcessObject) const
{
    wchar_t buf[MAX_PATH];
    GetModuleFileNameExW(handleProcessObject, 0, buf, MAX_PATH);
    std::wstring ws(buf);
    std::string str(ws.begin(), ws.end());

    return str;
}

// get cpu process
double WindowsMonitor::getProcessCPUUsage(HANDLE handleProcessObject, int &pProcessKernelTime, int &pProcessUserTime,
                                          int &pSystemTime) const
{
    /** Getting the following process and system times **/
    ULARGE_INTEGER fProcessKernelTime, fProcessUserTime, fSystemKernelTime, fSystemUserTime;
    getProcessTimes(handleProcessObject, NULL, NULL, &fProcessKernelTime, &fProcessUserTime);
    getSystemTimes(NULL, &fSystemKernelTime, &fSystemUserTime);
    /** Calculating the process CPU time **/
    ULARGE_INTEGER processCPUTime;
    processCPUTime.QuadPart = (fProcessKernelTime.QuadPart - pProcessKernelTime) + (fProcessUserTime.QuadPart - pProcessUserTime);

    /** Calculating the total CPU time **/
    int totalCPUTime = fSystemKernelTime.QuadPart + fSystemUserTime.QuadPart - pSystemTime;

    /** Calculating the process CPU usage **/
    double ret = processCPUTime.QuadPart * 100.0 / totalCPUTime;

    /** Changing the previous process and system times on following **/
    pProcessKernelTime = fProcessKernelTime.QuadPart;
    pProcessUserTime = fProcessUserTime.QuadPart;
    pSystemTime = fSystemKernelTime.QuadPart + fSystemUserTime.QuadPart;

    return ret;
}

// get work time process
int WindowsMonitor::getProcessWorkingSetSize(HANDLE handleProcessObject) const
{
    /** Getting the working set size **/
    PROCESS_MEMORY_COUNTERS_EX processMemoryCounters;
    processMemoryCounters.cb = sizeof(processMemoryCounters);

    if (!GetProcessMemoryInfo(handleProcessObject, reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&processMemoryCounters),
                              processMemoryCounters.cb))
    {
        return 0;
    }

    return processMemoryCounters.WorkingSetSize;
}

// get user name
std::string WindowsMonitor::getProcessUserName(HANDLE handleProcessObject) const
{
    /** Openning the process token handle **/
    HANDLE handleProcessTokenObject = INVALID_HANDLE_VALUE;
    if (!OpenProcessToken(handleProcessObject, TOKEN_QUERY, &handleProcessTokenObject))
    {
        /** Freeing up resources and throwing the exception **/
        CloseHandle(handleProcessTokenObject);

        return std::string();
    }

    /** Getting the process token information **/
    DWORD returnLength;
    if (!GetTokenInformation(handleProcessTokenObject, TokenOwner, NULL, 0, &returnLength))
    {
        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
        {
            CloseHandle(handleProcessTokenObject);

            return std::string();
        }
    }

    PTOKEN_OWNER tokenOwner = static_cast<PTOKEN_OWNER>(LocalAlloc(LPTR, returnLength));

    if (!tokenOwner)
    {
        CloseHandle(handleProcessTokenObject);

        return std::string();
    }

    if (!GetTokenInformation(handleProcessTokenObject, TokenOwner, tokenOwner, returnLength, &returnLength))
    {
        /** Freeing up resources and throwing the exception **/
        CloseHandle(handleProcessTokenObject);
        LocalFree(tokenOwner);

        return std::string();
    }

    /** Freeing up resources **/
    setlocale(LC_ALL, "");
    /** Getting the process username **/
    SID_NAME_USE SidType;
    DWORD userNameSize = 0;
    LPWSTR userName = NULL;
    DWORD domainNameSize = 0;
    LPWSTR domainName;
    if (!LookupAccountSidW(NULL, tokenOwner->Owner, userName, &userNameSize, NULL, &domainNameSize, &SidType))
    {
        userName = new WCHAR[userNameSize];
        domainName = new WCHAR[domainNameSize];
        if (!LookupAccountSidW(NULL, tokenOwner->Owner, userName, &userNameSize, domainName, &domainNameSize, &SidType))
        {
            /** Freeing up resources and throwing the exception **/
            CloseHandle(handleProcessTokenObject);
            LocalFree(tokenOwner);
            delete[] userName;
            delete[] domainName;
            userName = NULL;
            domainName = NULL;

            return std::string();
        }
    }

    std::string ret = U16toU8(userName);
    /** Freeing up resources **/
    CloseHandle(handleProcessTokenObject);
    LocalFree(tokenOwner);
    delete[] userName;
    delete[] domainName;
    userName = NULL;
    domainName = NULL;
    return ret;
}

// get process time
void WindowsMonitor::getProcessTimes(HANDLE hProcess, ULARGE_INTEGER* creationTime, ULARGE_INTEGER* exitTime,
                                     ULARGE_INTEGER* kernelTime, ULARGE_INTEGER* userTime) const
{
    /** Getting the following process times **/
    FILETIME processCreationTime, processExitTime, processKernelTime, processUserTime;
    if (!GetProcessTimes(hProcess, &processCreationTime, &processExitTime, &processKernelTime, &processUserTime))
    {
        /** Freeing up resources and throwing the exception **/
        CloseHandle(hProcess);
    }

    /** Converting from the FILETIME to ULARGEINTEGER **/
    if (creationTime != NULL)
    {
        memcpy(creationTime, &processCreationTime, sizeof(FILETIME));
    }
    if (exitTime != NULL)
    {
        memcpy(exitTime, &processExitTime, sizeof(FILETIME));
    }
    if (kernelTime != NULL)
    {
        memcpy(kernelTime, &processKernelTime, sizeof(FILETIME));
    }
    if (userTime != NULL)
    {
        memcpy(userTime, &processUserTime, sizeof(FILETIME));
    }
}

/** Private methods, serching info about threads **/
void WindowsMonitor::getThreadTimes(HANDLE hThread, ULARGE_INTEGER* creationTime, ULARGE_INTEGER* exitTime,
                                    ULARGE_INTEGER* kernelTime, ULARGE_INTEGER* userTime) const
{
    /** Getting the following process times **/
    FILETIME threadCreationTime, threadExitTime, threadKernelTime, threadUserTime;
    if (!GetThreadTimes(hThread, &threadCreationTime, &threadExitTime, &threadKernelTime, &threadUserTime))
    {
        /** Freeing up resources and throwing the exception **/
        CloseHandle(hThread);
    }
    else
    {
        /** Converting from the FILETIME to ULARGEINTEGER **/
        if (creationTime != NULL)
        {
            memcpy(creationTime, &threadCreationTime, sizeof(FILETIME));
        }
        if (exitTime != NULL)
        {
            memcpy(exitTime, &threadExitTime, sizeof(FILETIME));
        }
        if (kernelTime != NULL)
        {
            memcpy(kernelTime, &threadKernelTime, sizeof(FILETIME));
        }
        if (userTime != NULL)
        {
            memcpy(userTime, &threadUserTime, sizeof(FILETIME));
        }
    }
}

// get process threads
bool WindowsMonitor::getProcessThreads(int processID, const std::string &processName,
                                       std::vector<dsmonitor::MapString> &threads) const
{
    /** Сreating the thread snapshot handle **/
    HANDLE handleSnapshot = INVALID_HANDLE_VALUE;
    do
    {
        handleSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    } while (handleSnapshot == INVALID_HANDLE_VALUE);

    /** Getting the first snapshot handle data **/
    THREADENTRY32 threadEntry;
    threadEntry.dwSize = sizeof(threadEntry);

    bool noError = Thread32First(handleSnapshot, &threadEntry);

    /** While snapshot has the thread data **/
    while (noError)
    {
        if (threadEntry.th32OwnerProcessID == processID)
        {
            MapString mapString;
            mapString.insert(PairString(Help::Thread::toString(Help::Thread::pid), toString(threadEntry.th32ThreadID)));
            mapString.insert(PairString(Help::Thread::toString(Help::Thread::ppid), toString(threadEntry.th32OwnerProcessID)));
            // double cpuUsage = getThreadCPUUsage(threadEntry.th32ThreadID, threadEntry.kernelTime, threadEntry->userTime, threadEntry->lastSystemTime);
            double usage = threadEntry.cntUsage;
            mapString.insert(PairString(Help::Thread::toString(Help::Thread::usage), toString(usage)));

            threads.push_back(mapString);
        }
        /** Getting the next snapshot handle data **/
        noError = Thread32Next(handleSnapshot, &threadEntry);
    }

    return false;
}

// get cpu threads
double WindowsMonitor::getThreadCPUUsage(int threadID, int &pThreadKernelTime, int &pThreadUserTime,
                                         int &pSystemTime) const
{
    /** Openning the thread handle **/
    HANDLE hThread = INVALID_HANDLE_VALUE;
    hThread = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, static_cast<DWORD>(threadID));
    if (hThread == INVALID_HANDLE_VALUE)
    {
        /** Freeing up resources and throwing the exception **/
        CloseHandle(hThread);

        return 0;
    }

    /** Getting the following system and thread times **/
    ULARGE_INTEGER fThreadKernelTime, fThreadUserTime, fSystemKernelTime, fSystemUserTime;
    try
    {
        getThreadTimes(hThread, NULL, NULL, &fThreadKernelTime, &fThreadUserTime);
        getSystemTimes(NULL, &fSystemKernelTime, &fSystemUserTime);

        /** Calculating the thread CPU time **/
        ULARGE_INTEGER threadCPUTime;
        threadCPUTime.QuadPart = (fThreadKernelTime.QuadPart - pThreadKernelTime) + (fThreadUserTime.QuadPart - pThreadUserTime);

        /** Calculating the total CPU time **/
        int totalCPUTime = fSystemKernelTime.QuadPart + fSystemUserTime.QuadPart - pSystemTime;

        /** Calculating the thread CPU usage **/
        double cpuUsage = threadCPUTime.QuadPart * 100.0 / totalCPUTime;

        /** Changing the previous thread times on following **/
        pThreadKernelTime = fThreadKernelTime.QuadPart;
        pThreadUserTime = fThreadUserTime.QuadPart;
        pSystemTime = fSystemKernelTime.QuadPart + fSystemUserTime.QuadPart;

        /** Freeing up resources **/
        CloseHandle(hThread);

        return cpuUsage;
    }
    catch (...)
    {
        /** Freeing up resources and throwing the exception **/
        CloseHandle(hThread);
    }

    return 0;
}

/** Private helping methods **/
void WindowsMonitor::getSystemTimes(ULARGE_INTEGER* idleTime, ULARGE_INTEGER* kernelTime, ULARGE_INTEGER* userTime) const
{
    /** Getting the following system times **/
    FILETIME followingIdleTime, followingKernelTime, followingUserTime;
    if (GetSystemTimes(&followingIdleTime, &followingKernelTime, &followingUserTime))
    {
        /** Converting from the FILETIME to ULARGEINTEGER **/
        if (idleTime != NULL)
        {
            memcpy(idleTime, &followingIdleTime, sizeof(FILETIME));
        }
        if (kernelTime != NULL)
        {
            memcpy(kernelTime, &followingKernelTime, sizeof(FILETIME));
        }
        if (userTime != NULL)
        {
            memcpy(userTime, &followingUserTime, sizeof(FILETIME));
        }
    }
}

// converting U16 to U8
std::string WindowsMonitor::U16toU8(LPWSTR u16str) const
{
    std::string ret;
    ret.reserve(wcslen(u16str));
    for (; *u16str; u16str++)
        ret += (char)*u16str;
    return ret;
}

// check the privileges
// whether the program is running as an administrator
void WindowsMonitor::setPrivilege(HANDLE processTokenHandle, HANDLE processHandle, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege) const
{
    TOKEN_PRIVILEGES tp;
    LUID luid;

    if (LookupPrivilegeValue(NULL, lpszPrivilege, &luid))
    {
        tp.PrivilegeCount = 1;
        tp.Privileges[0].Luid = luid;
        if (bEnablePrivilege)
        {
            tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        }
        else
        {
            tp.Privileges[0].Attributes = 0;
        }

        if (OpenProcessToken(processHandle, TOKEN_ADJUST_PRIVILEGES, &processTokenHandle))
        {
            if (AdjustTokenPrivileges(processTokenHandle, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
            {
                if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
                {
                    GetLastError();
                }
            }
        }
    }
}

// getting the computer name from the registry
std::string WindowsMonitor::getProductName() const
{
    std::string CStrName = GetStringFromReg(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
                                            "ProductName");
    return static_cast<std::string>(CStrName);
}

// access to the registry
std::string WindowsMonitor::GetStringFromReg(HKEY keyParent, std::string key, std::string name) const
{
    HKEY hKey;
    if (RegOpenKeyEx(keyParent, key.c_str(), 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
    {
        return std::string();
    }
    else
    {
        DWORD type = REG_SZ;
        DWORD dwBufSize;
        if (RegQueryValueEx(hKey, name.c_str(), NULL, &type, NULL, &dwBufSize) != ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            return std::string();
        }
        else
        {
            if (type != REG_SZ)
            {
                RegCloseKey(hKey);
                return std::string();
            }
            else
            {
                TCHAR buf[255] = {0};
                if (RegQueryValueEx(hKey, TEXT(name.c_str()), NULL, &type, (LPBYTE)buf, &dwBufSize) != ERROR_SUCCESS)
                {
                    RegCloseKey(hKey);
                    return std::string();
                }
                else
                {
                    return std::string(buf);
                }
            }
        }
    }
}

struct FindCache
{
    FindCache(const PCACHE_DESCRIPTOR &Cache)
    : Cache_(Cache)
    {
    }

    bool operator()(const PCACHE_DESCRIPTOR &Cache)
    {
        return (Cache_->Level == Cache->Level) && (Cache_->Type == Cache->Type);
    }

private:
    PCACHE_DESCRIPTOR Cache_;
    BYTE byte_;
    PROCESSOR_CACHE_TYPE type_;
};

typedef BOOL (WINAPI* LPFN_GLPI)(PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, PDWORD);

// Helper function to count set bits in the processor mask.
DWORD WindowsMonitor::CountSetBits(ULONG_PTR bitMask)
{
    DWORD LSHIFT = sizeof(ULONG_PTR) * 8 - 1;
    DWORD bitSetCount = 0;
    ULONG_PTR bitTest = (ULONG_PTR)1 << LSHIFT;
    DWORD i;

    for (i = 0; i <= LSHIFT; ++i)
    {
        bitSetCount += ((bitMask & bitTest) ? 1 : 0);
        bitTest /= 2;
    }

    return bitSetCount;
}

const std::string CacheTypeArray[] = {"Unified",
                                      "Instruction",
                                      "Data",
                                      "Trace"
};

// converting cache type to string
std::string WindowsMonitor::ProcessorCacheTypeToString(PROCESSOR_CACHE_TYPE cacheType)
{
    if (cacheType >= 0 && cacheType <= _countof(CacheTypeArray))
    {
        return CacheTypeArray[static_cast<int>(cacheType)];
    }

    return "Unknown";
}

// get cache
int WindowsMonitor::getVecCache(std::vector<PCACHE_DESCRIPTOR> &vec)
{
    LPFN_GLPI glpi;
    BOOL done = FALSE;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = NULL;
    DWORD returnLength = 0;
    DWORD logicalProcessorCount = 0;
    DWORD numaNodeCount = 0;
    DWORD processorCoreCount = 0;
    DWORD processorL1CacheCount = 0;
    DWORD processorL2CacheCount = 0;
    DWORD processorL3CacheCount = 0;
    DWORD processorPackageCount = 0;
    DWORD byteOffset = 0;
    PCACHE_DESCRIPTOR Cache;

    glpi = (LPFN_GLPI)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "GetLogicalProcessorInformation");
    if (NULL == glpi)
    {
        return 0;
    }

    while (!done)
    {
        DWORD rc = glpi(buffer, &returnLength);

        if (FALSE == rc)
        {
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
                if (buffer)
                {
                    free(buffer);
                }

                buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(returnLength);

                if (NULL == buffer)
                {
                    return 0;
                }
            }
            else
            {
                return 0;
            }
        }
        else
        {
            done = TRUE;
        }
    }

    ptr = buffer;
    while (byteOffset /*+ sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION)*/ <= returnLength)
    {
        switch (ptr->Relationship)
        {
            case RelationNumaNode:
            {
                // Non-NUMA systems report a single record of this type.
                numaNodeCount++;
                break;
            }

            case RelationProcessorCore:
            {
                processorCoreCount++;
                // A hyperthreaded core supplies more than one logical processor.
                logicalProcessorCount += CountSetBits(ptr->ProcessorMask);
                break;
            }

            case RelationCache:
            {
                // Cache data is in ptr->Cache, one CACHE_DESCRIPTOR structure for each cache.
                Cache = &ptr->Cache;

                std::vector<PCACHE_DESCRIPTOR>::iterator it;
                it = std::find_if(vec.begin(), vec.end(), FindCache(Cache));
                if (it == vec.end())
                {
                    vec.push_back(Cache);
                }

                if (Cache->Level == 1)
                {
                    processorL1CacheCount++;
                }
                else if (Cache->Level == 2)
                {
                    processorL2CacheCount++;
                }
                else if (Cache->Level == 3)
                {
                    processorL3CacheCount++;
                }
                break;
            }

            case RelationProcessorPackage:
            {
                // Logical processors share a physical package.
                processorPackageCount++;
                break;
            }
        }
        byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
        ptr++;
    }

    free(buffer);

    return 1;
}

// get CPU name
std::string WindowsMonitor::ProcessorName()
{
    int CPUInfo[4] = {-1};
    char CPUBrandString[0x40];
    __cpuid(CPUInfo, 0x80000000);
    unsigned int nExIds = CPUInfo[0];

    memset(CPUBrandString, 0, sizeof(CPUBrandString));

    // Get the information associated with each extended ID.
    for (int i = 0x80000000; i <= nExIds; ++i)
    {
        __cpuid(CPUInfo, i);
        // Interpret CPU brand string.
        if (i == 0x80000002)
        {
            memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
        }
        else if (i == 0x80000003)
        {
            memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
        }
        else if (i == 0x80000004)
        {
            memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
        }
    }

    return std::string(CPUBrandString);
}
}