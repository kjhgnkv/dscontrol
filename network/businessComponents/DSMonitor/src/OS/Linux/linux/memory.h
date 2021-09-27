#pragma once

#include <string>
#include <ostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include "linuxhelp.hpp"

#define MEMINFO "/proc/meminfo"

namespace linuxapi
{
struct memory
{
    uint32_t memTotal;
    uint32_t memFree;
    uint32_t memAvailable;
    uint32_t buffers;
    uint32_t cached;
    uint32_t swapCached;
    uint32_t active;
    uint32_t inactive;
    uint32_t activeAnon;
    uint32_t inactiveAnon;
    uint32_t activeFile;
    uint32_t inactiveFile;
    uint32_t unevictable;
    uint32_t mlocked;
    uint32_t swapTotal;
    uint32_t swapFree;
    uint32_t dirty;
    uint32_t writeback;
    uint32_t anonPages;
    uint32_t mapped;
    uint32_t shmem;
    uint32_t kReclaimable;
    uint32_t slab;
    uint32_t sReclaimable;
    uint32_t sUnreclaim;
    uint32_t kernelStack;
    uint32_t pageTables;
    uint32_t nfsUnstable;
    uint32_t bounce;
    uint32_t writebackTmp;
    uint32_t commitLimit;
    uint32_t committedAs;
    uint32_t vmallocTotal;
    uint32_t vmallocUsed;
    uint32_t vmallocChunk;
    uint32_t percpu;
    uint32_t hardwareCorrupted;
    uint32_t anonHugePages;
    uint32_t shmemHugePages;
    uint32_t shmemPmdMapped;
    uint32_t cmaTotal;
    uint32_t cmaFree;
    uint32_t hugePagesTotal;
    uint32_t hugePagesFree;
    uint32_t hugePagesRsvd;
    uint32_t hugePagesSurp;
    uint32_t hugepagesize;
    uint32_t hugetlb;
    uint32_t directMap4K;
    uint32_t directMap2M;
    uint32_t directMap1G;

    friend std::ostream &operator<<(std::ostream &os, const memory &__memory)
    {
        os << "Memory:" << "\n" << "\tMemTotal: " << __memory.memTotal << "\n" << "\tMemFree: " << __memory.memFree << "\n" << "\tMemAvailable: " << __memory.memAvailable << "\n" << "\tBuffers: " << __memory.buffers << "\n" << "\tCached: " << __memory.cached << "\n" << "\tSwapCached: " << __memory.swapCached << "\n" << "\tActive: " << __memory.active << "\n" << "\tInactive: " << __memory.inactive << "\n" << "\tActive_anon: " << __memory.activeAnon << "\n" << "\tInactive_anon: " << __memory.inactiveAnon << "\n" << "\tActive_file: " << __memory.activeFile << "\n" << "\tInactive_file: " << __memory.inactiveFile << "\n" << "\tUnevictable: " << __memory.unevictable << "\n" << "\tMlocked: " << __memory.mlocked << "\n" << "\tSwapTotal: " << __memory.swapTotal << "\n" << "\tSwapFree: " << __memory.swapFree << "\n" << "\tDirty: " << __memory.dirty << "\n" << "\tWriteback: " << __memory.writeback << "\n" << "\tAnonPages: " << __memory.anonPages << "\n" << "\tMapped: " << __memory.mapped << "\n" << "\tShmem: " << __memory.shmem << "\n" << "\tKReclaimable: " << __memory.kReclaimable << "\n" << "\tSlab: " << __memory.slab << "\n" << "\tSReclaimable: " << __memory.sReclaimable << "\n" << "\tSUnreclaim: " << __memory.sUnreclaim << "\n" << "\tKernelStack: " << __memory.kernelStack << "\n" << "\tPageTables: " << __memory.pageTables << "\n" << "\tNFS_Unstable: " << __memory.nfsUnstable << "\n" << "\tBounce: " << __memory.bounce << "\n" << "\tWritebackTmp: " << __memory.writebackTmp << "\n" << "\tCommitLimit: " << __memory.commitLimit << "\n" << "\tCommitted_AS: " << __memory.committedAs << "\n" << "\tVmallocTotal: " << __memory.vmallocTotal << "\n" << "\tVmallocUsed: " << __memory.vmallocUsed << "\n" << "\tVmallocChunk: " << __memory.vmallocChunk << "\n" << "\tPercpu: " << __memory.percpu << "\n" << "\tHardwareCorrupted: " << __memory.hardwareCorrupted << "\n" << "\tAnonHugePages: " << __memory.anonHugePages << "\n" << "\tShmemHugePages: " << __memory.shmemHugePages << "\n" << "\tShmemPmdMapped: " << __memory.shmemPmdMapped << "\n" << "\tCmaTotal: " << __memory.cmaTotal << "\n" << "\tCmaFree: " << __memory.cmaFree << "\n" << "\tHugePages_Total: " << __memory.hugePagesTotal << "\n" << "\tHugePages_Free: " << __memory.hugePagesFree << "\n" << "\tHugePages_Rsvd: " << __memory.hugePagesRsvd << "\n" << "\tHugePages_Surp: " << __memory.hugePagesSurp << "\n" << "\tHugepagesize: " << __memory.hugepagesize << "\n" << "\tHugetlb: " << __memory.hugetlb << "\n" << "\tDirectMap4k: " << __memory.directMap4K << "\n" << "\tDirectMap2M: " << __memory.directMap2M << "\n" << "\tDirectMap1G: " << __memory.directMap1G << "\n";

        return os;
    }
};

/* Put information about the memory. */
bool memory(struct memory* __memory)
{
    std::ifstream ifmeminfo;
    ifmeminfo.open(MEMINFO);
    if (!ifmeminfo.is_open())
    {
        std::cerr << "Error opening \"" + std::string(MEMINFO) + "\" file!\n\t" << strerror(errno) << std::endl;
        return true;
    }

    std::vector<std::string> vec;
    std::vector<std::string>::iterator it;

    std::string line;
    while (getline(ifmeminfo, line))
    {
        vec.push_back(line);
    }
    ifmeminfo.close();

    if (vec.empty())
    {
        return true;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("MemTotal"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->memTotal;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("MemFree"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->memFree;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("MemAvailable"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->memAvailable;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("Buffers"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->buffers;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("umask"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->cached;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("umask"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->swapCached;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("umask"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->active;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("umask"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->inactive;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("Active(anon)"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->activeAnon;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("Inactive(anon)"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->inactiveAnon;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("Active(file)"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->activeFile;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("Inactive(file)"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->inactiveFile;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("Unevictable"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->unevictable;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("Mlocked"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->mlocked;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("SwapTotal"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->swapTotal;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("SwapFree"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->swapFree;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("Dirty"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->dirty;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("Writeback"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->writeback;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("AnonPages"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->anonPages;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("Mapped"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->mapped;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("shmem"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->shmem;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("KReclaimable"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->kReclaimable;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("Slab"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->slab;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("SReclaimable"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->sReclaimable;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("SUnreclaim"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->sUnreclaim;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("KernelStack"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->kernelStack;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("PageTables"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->pageTables;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("NFS_Unstable"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->nfsUnstable;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("Bounce"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->bounce;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("WritebackTmp"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->writebackTmp;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("CommitLimit"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->commitLimit;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("Committed_AS"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->committedAs;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("VmallocTotal"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->vmallocTotal;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("VmallocUsed"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->vmallocUsed;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("VmallocChunk"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->vmallocChunk;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("Percpu"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->percpu;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("HardwareCorrupted"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->hardwareCorrupted;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("AnonHugePages"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->anonHugePages;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("ShmemHugePages"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->shmemHugePages;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("ShmemPmdMapped"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->shmemPmdMapped;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("CmaTotal"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->cmaTotal;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("CmaFree"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->cmaFree;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("HugePages_Total"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->hugePagesTotal;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("HugePages_Free"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->hugePagesFree;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("HugePages_Rsvd"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->hugePagesRsvd;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("HugePages_Surp"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->hugePagesSurp;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("Hugepagesize"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->hugepagesize;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("Hugetlb"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->hugetlb;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("DirectMap4k"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->directMap4K;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("DirectMap2M"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->directMap2M;
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("DirectMap1G"));
    if (it != vec.end())
    {
        std::istringstream(substr(*it)) >> __memory->directMap1G;
    }

    return false;
}
}