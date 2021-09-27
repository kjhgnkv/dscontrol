#pragma once

/**
 * @file WindowsMonitor.h
 * @brief The header file with WindowsMonitor class.
 * @details This flie contains the detailed description of the WindowsMonitor
 *class.
 * @authors Zyuzin Nikita.
 **/

#include "../../MonitorType.h"
#include "../../DSMonitorHelp.hpp"
#include "../../ProcessData.hpp"

/**
 * @brief Encapsulates the API for looking for the information about the
 *computer on Windows platforms.
 **/
namespace dsmonitor
{
class WindowsMonitor : public MonitorType
{
public:
    bool processes(const std::string &procNameApp, const std::string &procPathApp, ProcessData &processData);
    bool getInfoMemory(std::map<std::string, std::string> &infoMemory);
    bool getInfoCpu(std::map<std::string, std::string> &infoCpu);
    bool getInfoStorage(std::map<std::string, std::string> &infoStorage);
    bool getInfoSystem(std::map<std::string, std::string> &infoSystem);

private:
    /**
     * @brief Calculates the CPU usage of the specific process.
     * @param [in] A handle to the process whose Cpu usage is sought.
     * @param [in, out] Receives the amount of time that the process has
     *executed in kernel mode.
     * @param [in, out] Receives the amount of time that the process has
     *executed in user mode.
     * @param [in, out] Receives the amount of time that the system has spent
     *executing in kernel and user modes.
     * @return A value of process CPU usage calculated from the last call of
     *this function. Last call determines by last 3 params.
     **/
    double getProcessCPUUsage(HANDLE handleProcessObject, int &pProcessKernelTime, int &pProcessUserTime,
                              int &pSystemTime) const;

    /**
     * @brief Retrieves the full path to the executable file of the process.
     * @param [in] A handle to the process whose path is sought.
     * @return The founded process path in a string format.
     **/
    std::string getProcessPath(HANDLE handleProcessObject) const;

    /**
     * @brief Retrieves the full path to the executable file of the process.
     * @param [in] A handle to the process whose path is sought.
     * @return The founded process path in a string format.
     **/
    int getProcessWorkingSetSize(HANDLE handleProcessObject) const;

    /**
     * @brief Retrieves the username of the process.
     * @param [in] A handle to the process whose username is sought.
     * @return The founded username in a string format.
     **/
    std::string getProcessUserName(HANDLE handleProcessObject) const;

    void getProcessTimes(HANDLE handleProcessObject, ULARGE_INTEGER* creationTime, ULARGE_INTEGER* exitTime,
                         ULARGE_INTEGER* kernelTime, ULARGE_INTEGER* userTime) const;

    bool getProcessThreads(int processID, const std::string &processName,
                           std::vector<dsmonitor::MapString> &threads) const;

    double getThreadCPUUsage(int threadID, int &pThreadKernelTime, int &pThreadUserTime,
                             int &pSystemTime) const;

    void getThreadTimes(HANDLE handleThreadObject, ULARGE_INTEGER* creationTime, ULARGE_INTEGER* exitTime,
                        ULARGE_INTEGER* kernelTime, ULARGE_INTEGER* userTime) const;

    void getSystemTimes(ULARGE_INTEGER* idleTime, ULARGE_INTEGER* kernelTime, ULARGE_INTEGER* userTime) const;

    /**
     * @brief Converts a UTF-16 string in the UTF-8 one.
     * @param [in] C-string in a UTF-16 format.
     * @return Converted UTF-8 std::string object.
     **/
    std::string U16toU8(LPWSTR u16Str) const;

    static std::string GetOsVersionName();

    std::string getProductName() const;

    std::string GetStringFromReg(HKEY keyParent, std::string keyName, std::string keyValName) const;

    std::string ProcessorName();
    int getVecCache(std::vector<PCACHE_DESCRIPTOR> &vec);

    std::string ProcessorCacheTypeToString(PROCESSOR_CACHE_TYPE cacheType);
    DWORD CountSetBits(ULONG_PTR bitMask);

    void setPrivilege(HANDLE processTokenHandle, HANDLE processHandle, LPCTSTR lpszPrivilege,
                      BOOL bEnablePrivilege) const;
};
}