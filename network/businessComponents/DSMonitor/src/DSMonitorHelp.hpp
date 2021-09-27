#pragma once

#include <Poco/Net/NetworkInterface.h>

#include <fstream>
#include <string>
#include <sstream>
#include <stdint.h>

#ifdef __linux

    #include <Poco/Dynamic/Var.h>
    #include <iostream>
    #include <fstream>
    #include <unistd.h>

#endif

namespace dsmonitor
{
typedef Poco::Net::NetworkInterface NetworkInterface;

namespace Help
{
// test fun log memory
static bool logMemory()
{
    #ifdef __linux
    using std::ios_base;
    using std::ifstream;
    using std::string;

    double vm_usage = 0.0;
    double resident_set = 0.0;

    // 'file' stat seems to give the most reliable results
    //
    ifstream stat_stream("/proc/self/stat", ios_base::in);

    // dummy vars for leading entries in stat that we don't care about
    //
    string pid, comms, state, ppid, pgrp, session, tty_nr;
    string tpgid, flags, minflt, cminflt, majflt, cmajflt;
    string utime, stime, cutime, cstime, priority, nice;
    string O, itrealvalue, starttime;

    // the two fields we want
    //
    unsigned long vsize;
    long rss;

    stat_stream >> pid >> comms >> state >> ppid >> pgrp >> session >> tty_nr >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt >> utime >> stime >> cutime >> cstime >> priority >> nice >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

    stat_stream.close();

    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
    vm_usage = vsize / 1024.0;
    static double vm_usage_max = 0;
    resident_set = rss * page_size_kb;

    if (vm_usage > vm_usage_max)
    {
        vm_usage_max = vm_usage;

        Poco::Dynamic::Var vvm_usage(vm_usage);
        Poco::Dynamic::Var vresident_set(resident_set);

        std::cout << ("MEMORY: vm_usage=") + vvm_usage.toString() + "; resident_set=" + vresident_set.toString() << std::endl;

        return false;
    }
    #endif

    return true;
}

namespace DSMonitor
{
enum Enum
{
    appType,
    msgType,
    status,
    None,
};

static const std::string toString(Enum value)
{
    switch (value)
    {
        case appType:
            return "appType";
        case msgType:
            return "msgType";
        case status:
            return "Status";
        case None:
        default:
            return std::string();
    }
}

static Enum fromString(const std::string &item)
{
    if (item == "appType")
        return appType;
    if (item == "msgType")
        return msgType;
    if (item == "Status")
        return status;

    return None;
}
};

namespace KeySystem
{
enum Enum
{
    network,
    cpu,
    memory,
    storage,
    system,
    process,
    threads,
    xml,
    None
};

static std::string toString(Enum value)
{
    switch (value)
    {
        case cpu:
            return "Cpu";
        case network:
            return "Network";
        case memory:
            return "Memory";
        case storage:
            return "Storage";
        case system:
            return "System";
        case process:
            return "Process";
        case threads:
            return "Threads";
        case xml:
            return "Xml";
        case None:
        default:
            return std::string();
    }
}

static Enum fromString(const std::string &item)
{
    if (item == "Cpu")
        return cpu;
    if (item == "Network")
        return network;
    if (item == "Interfaces")
        return memory;
    if (item == "Memory")
        return memory;
    if (item == "Storage")
        return storage;
    if (item == "System")
        return system;
    if (item == "Process")
        return process;
    if (item == "Threads")
        return threads;
    if (item == "Xml")
        return xml;

    return None;
}
};

namespace Network
{
enum Enum
{
    IPv4 = NetworkInterface::IP_ADDRESS,
    Mask = NetworkInterface::SUBNET_MASK,
    Gateway = NetworkInterface::BROADCAST_ADDRESS,
    interfaces,
    interface,
    hostName,
    None
};

static const std::string toString(Enum value)
{
    switch (value)
    {
        case hostName:
            return "hostName";
        case interfaces:
            return "interfaces";
        case interface:
            return "interface";
        case IPv4:
            return "IPv4";
        case Mask:
            return "Mask";
        case Gateway:
            return "Gateway";
        case None:
        default:
            return std::string();
    }
}

static Enum fromString(const std::string &item)
{
    if (item == "hostName")
        return hostName;
    if (item == "interfaces")
        return interfaces;
    if (item == "interface")
        return interface;
    if (item == "IPv4")
        return IPv4;
    if (item == "Mask")
        return Mask;
    if (item == "Gateway")
        return Gateway;

    return None;
}
};

namespace Cpu
{
enum Enum
{
    name,
    min,
    max,
    cache,
    cores,
    threads,
    usage,
    None
};

static std::string toString(Enum value)
{
    switch (value)
    {
        case name:
            return "name";
        case min:
            return "min";
        case max :
            return "max";
        case cores :
            return "cores";
        case threads :
            return "threads";
        case usage :
            return "usage";
        case cache:
            return "cache";
        case None:
        default:
            return std::string();
    }
}

static Enum fromString(const std::string &item)
{
    if (item == "name")
        return name;
    if (item == "min")
        return min;
    if (item == "max")
        return max;
    if (item == "cores")
        return cores;
    if (item == "threads")
        return threads;
    if (item == "usage")
        return usage;
    if (item == "cache")
        return cache;

    return None;
}
};

namespace Memory
{
enum Enum
{
    load,
    total,
    free,
    used,
    None
};

static const std::string toString(Enum value)
{
    switch (value)
    {
        case load:
            return "load";
        case total:
            return "total";
        case free:
            return "free";
        case used:
            return "used";
        case None:
        default:
            return std::string();
    }
}

static Enum fromString(const std::string &item)
{
    if (item == "load")
        return load;
    if (item == "total")
        return total;
    if (item == "free")
        return free;
    if (item == "used")
        return used;

    return None;
}
};

namespace Storage
{
enum Enum
{
    free,
    available,
    total,
    used,
    load,
    device,
    type,
    path,
    None
};

static const std::string toString(Enum value)
{
    switch (value)
    {
        case free:
            return "free";
        case total:
            return "total";
        case available:
            return "available";
        case used:
            return "used";
        case load:
            return "load";
        case device:
            return "device";
        case type:
            return "type";
        case path:
            return "path";
        case None:
        default:
            return std::string();
    }
}

static Enum fromString(const std::string &item)
{
    if (item == "free")
        return free;
    if (item == "total")
        return total;
    if (item == "available")
        return available;
    if (item == "used")
        return used;
    if (item == "load")
        return load;
    if (item == "device")
        return device;
    if (item == "type")
        return type;
    if (item == "path")
        return path;

    return None;
}
};

namespace System
{
enum Enum
{
    id,
    productName,
    kernelType,
    architecture,
    None
};

static const std::string toString(System::Enum value)
{
    switch (value)
    {
        case id:
            return "id";
        case productName:
            return "productName";
        case kernelType:
            return "kernelType";
        case architecture:
            return "architecture";
        case None:
        default:
            return std::string();
    }
}

static Enum fromString(const std::string &item)
{
    if (item == "id")
        return id;
    if (item == "productName")
        return productName;
    if (item == "kernelType")
        return kernelType;
    if (item == "architecture")
        return architecture;

    return None;
}
};

namespace Error
{
enum Enum
{
    error,
    None
};

static const std::string message()
{
    // TODO
    // надо добавить будет ИД и прочее
    // временная ошибка
    return "process not running";
}

static const std::string toString(Enum value)
{
    switch (value)
    {
        case error:
            return "error";
        case None:
        default:
            return std::string();
    }
}

static Enum fromString(const std::string &item)
{
    if (item == "error")
        return error;

    return None;
}
}

namespace Process
{
enum Enum
{
    pid,
    ppid,
    memoryLoad,
    usageCPU,
    status,
    name,
    threads,
    kernelTime,
    lastSystemTime,
    path,
    userName,
    userTime,
    workingSetSize,
    None
};

static const std::string toString(Enum value)
{
    switch (value)
    {
        case pid:
            return "pid";
        case ppid:
            return "ppid";
        case memoryLoad:
            return "memoryLoad";
        case usageCPU:
            return "usageCPU";
        case status:
            return "status";
        case name:
            return "name";
        case threads:
            return "threads";
        case kernelTime:
            return "kernelTime";
        case lastSystemTime:
            return "lastSystemTime";
        case path:
            return "path";
        case userName:
            return "userName";
        case userTime:
            return "userTime";
        case workingSetSize:
            return "workingSetSize";
        case None:
        default:
            return std::string();
    }
}

static Enum fromString(const std::string &item)
{
    if (item == "pid")
        return pid;
    if (item == "ppid")
        return ppid;
    if (item == "memoryLoad")
        return memoryLoad;
    if (item == "usageCPU")
        return usageCPU;
    if (item == "status")
        return status;
    if (item == "name")
        return name;
    if (item == "threads")
        return threads;
    if (item == "kernelTime")
        return kernelTime;
    if (item == "lastSystemTime")
        return lastSystemTime;
    if (item == "path")
        return path;
    if (item == "userName")
        return userName;
    if (item == "userTime")
        return userTime;
    if (item == "workingSetSize")
        return workingSetSize;

    return None;
}
};

namespace Thread
{
enum Enum
{
    pid,
    ppid,
    usage,
    None
};

static const std::string toString(Enum value)
{
    switch (value)
    {
        case pid:
            return "pid";
        case ppid:
            return "ppid";
        case usage:
            return "usage";
        case None:
        default:
            return std::string();
    }
}

static Enum fromString(const std::string &item)
{
    if (item == "pid")
        return pid;
    if (item == "ppid")
        return ppid;
    if (item == "usage")
        return usage;

    return None;
}
};

namespace XML
{
enum Enum
{
    key,
    xml,
    path,
    component,
    error,
    None
};

static const std::string toString(XML::Enum value)
{
    switch (value)
    {
        case key:
            return "key";
        case xml:
            return "xml";
        case path:
            return "path";
        case error:
            return "error";
        case component:
            return "component";
        case None:
        default:
            return std::string();
    }
}

static XML::Enum fromString(const std::string &item)
{
    if (item == "key")
        return key;
    if (item == "xml")
        return xml;
    if (item == "path")
        return path;
    if (item == "error")
        return error;
    if (item == "component")
        return component;

    return XML::None;
}

};

// convert int to string
template <typename T>
static std::string toString(const T &t)
{
    std::ostringstream convert;
    convert << t;
    return convert.str();
}

// remove poco ptr
template <typename T>
inline void removePtr(T &t)
{
    if (!t.isNull())
    {
        t.reset();
    }
}
}
}