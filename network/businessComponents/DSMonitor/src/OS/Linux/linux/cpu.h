#pragma once

#include <sys/utsname.h>
#include <string>
#include <ostream>

#define CPU "/proc/cpuinfo"
#define MIN_FREQ "/sys/devices/system/cpu/cpufreq/policy0/scaling_min_freq"
#define MAX_FREQ "/sys/devices/system/cpu/cpufreq/policy0/scaling_max_freq"

namespace linuxapi
{
struct cpu
{
    char vendor_id[_UTSNAME_LENGTH];
    char cpu_family[_UTSNAME_LENGTH];
    char model_name[_UTSNAME_LENGTH];
    unsigned int siblings;
    unsigned int cpu_cores;
    unsigned int min;
    unsigned int max;

    friend std::ostream &operator<<(std::ostream &os, const cpu &__cpu)
    {
        os << "CPU:" << "\n" << "\tvendor_id: " << __cpu.vendor_id << "\n" << "\tcpu_family: " << __cpu.cpu_family << "\n" << "\tmodel_name: " << __cpu.model_name << "\n" << "\tsiblings: " << __cpu.siblings << "\n" << "\tcpu_cores: " << __cpu.cpu_cores << "\n" << "\tmin: " << __cpu.min << "\n" << "\tmax: " << __cpu.max << "\n";

        return os;
    }
};

/* Put information about the cpu.  */
bool cpu(struct cpu* __cpu)
{
    std::ifstream ifcpu;
    ifcpu.open(CPU);
    if (!ifcpu.is_open())
    {
        std::cerr << "Error opening \"" + std::string(CPU) + "\" file!\n\t" << strerror(errno) << std::endl;
        return true;
    }

    std::vector<std::string> vec;
    std::vector<std::string>::iterator it;

    std::string line;
    while (getline(ifcpu, line))
    {
        vec.push_back(line);
    }
    ifcpu.close();

    if (vec.empty())
    {
        return true;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("vendor_id"));
    if (it != vec.end())
    {
        strcpy(__cpu->vendor_id, substr(*it).c_str());
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("model name"));
    if (it != vec.end())
    {
        strcpy(__cpu->model_name, substr(*it).c_str());
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("cpu family"));
    if (it != vec.end())
    {
        strcpy(__cpu->cpu_family, substr(*it).c_str());
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("siblings"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __cpu->siblings;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("cpu cores"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __cpu->cpu_cores;
    }

    std::ifstream min_stream(MIN_FREQ, std::ios_base::in);
    if (!min_stream.is_open())
    {
        std::cerr << "Error opening \"" + std::string(MIN_FREQ) + "\" file!\n\t" << strerror(errno) << std::endl;
        __cpu->min = 0;
    }
    else
    {
        min_stream >> __cpu->min;
    }

    std::ifstream max_stream(MAX_FREQ, std::ios_base::in);
    if (!max_stream.is_open())
    {
        std::cerr << "Error opening \"" + std::string(MAX_FREQ) + "\" file!\n\t" << strerror(errno) << std::endl;
        __cpu->max = 0;
    }
    else
    {
        max_stream >> __cpu->max;
    }

    return false;
}
}