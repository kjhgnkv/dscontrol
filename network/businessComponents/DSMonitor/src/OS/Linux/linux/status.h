#pragma once

#include <sys/utsname.h>
#include <linux/version.h>
#include <ostream>

namespace linuxapi
{
struct INITStatus
{
    enum Enum
    {
        Status,
        Name,

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 7, 0)
        Umask,
        #endif

        State,
        Tgid,

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 13, 0)
        Ngid,
        #endif

        Pid,
        PPid,
        TracerPid,
        Uid,
        Gid,
        FDSize,
        Groups,

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 1, 0)
        NStgid,
        NSpid,
        NSpgid,
        NSsid,
        #endif

        VmPeak,
        VmSize,
        VmLck,

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 2, 0)
        VmPin,
        #endif

        VmHWM,
        VmRSS,

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 5, 0)
        RssAnon,
        RssFile,
        RssShmem,
        #endif

        VmData,
        VmStk,
        VmExe,
        VmLib,
        VmPTE,

        #if LINUX_VERSION_CODE > KERNEL_VERSION(4, 0, 0) && LINUX_VERSION_CODE < KERNEL_VERSION(4, 15, 0)
        VmPMD,
        #endif

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 34)
        VmSwap,
        #endif

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0)
        HugetlbPages,
        #endif

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 15, 0)
        CoreDumping,
        #endif

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 0, 0)
        THP_enabled,
        #endif

        Threads,
        SigQ,
        SigPnd,
        ShdPnd,
        SigBlk,
        SigIgn,
        SigCgt,
        CapInh,
        CapPrm,
        CapEff,
        CapBnd,

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 3, 0)
        CapAmb,
        #endif

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 10, 0)
        NoNewPrivs,
        #endif

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)
        Seccomp,
        #endif

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 17, 0)
        Speculation_Store_Bypass,
        #endif

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24)
        Cpus_allowed,
        #endif

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 26)
        Cpus_allowed_list,
        #endif

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24)
        Mems_allowed,
        #endif

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 26)
        Mems_allowed_list,
        #endif

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 23)
        voluntary_ctxt_switches,
        nonvoluntary_ctxt_switches
        #endif
    };
};

// Provides much of the information in /proc/[pid]/stat and
// /proc/[pid]/statm in a format that's easier for humans to
// parse. Here's an example:

struct status
{
    // * Name: Command run by this process.
    char name[_UTSNAME_SYSNAME_LENGTH];

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 7, 0)
    // * Umask: Process umask, expressed in octal with a leading
    // zero; see umask(2). (Since Linux 4.7.)
    unsigned int umask;
    #endif

    // * State: Current state of the process. One of "R (running)",
    // "S (sleeping)", "D (disk sleep)", "T (stopped)", "T (tracing
    // stop)", "Z (zombie)", or "X (dead)".
    char state[_UTSNAME_SYSNAME_LENGTH];

    // * Tgid: Thread group ID (i.e., Process ID).
    unsigned int tgid;

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 13, 0)
    // * Ngid: NUMA group ID (0 if none; since Linux 3.13).
    unsigned int ngid;
    #endif

    // * Pid: Thread ID (see gettid(2)).
    unsigned short int pid;

    // * PPid: PID of parent process.
    unsigned short int pPid;

    // * TracerPid: PID of process tracing this process (0 if not
    // being traced).
    unsigned int tracerPid;

    // * Uid, Gid: Real, effective, saved set, and filesystem UIDs
    // (GIDs).
    char uid[_UTSNAME_SYSNAME_LENGTH], gid[_UTSNAME_SYSNAME_LENGTH];

    // * FDSize: Number of file descriptor slots currently allocated.
    unsigned int fdSize;

    // * Groups: Supplementary group list.
    char groups[_UTSNAME_SYSNAME_LENGTH];

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 1, 0)
    // * NStgid: Thread group ID (i.e., PID) in each of the PID
    // namespaces of which [pid] is a member. The leftmost entry
    // shows the value with respect to the PID namespace of the
    // process that mounted this procfs (or the listXML namespace if
    // mounted by the kernel), followed by the value in succes‐
    // sively nested inner namespaces. (Since Linux 4.1.)
    unsigned int nsTgid;

    // * NSpid: Thread ID in each of the PID namespaces of which
    // [pid] is a member. The fields are ordered as for NStgid.
    // (Since Linux 4.1.)
    unsigned int nspid;

    // * NSpgid: Speculation_Store_BypassProcess group ID in each of the PID namespaces of
    // which [pid] is a member. The fields are ordered as for NSt‐
    // gid. (Since Linux 4.1.)
    unsigned int nspgid;

    // * NSsid: descendant namespace session ID hierarchy Session ID
    // in each of the PID namespaces of which [pid] is a member.
    // The fields are ordered as for NStgid. (Since Linux 4.1.)
    unsigned int nssid;
    #endif

    // * VmPeak: Peak virtual memory size.
    unsigned int vmPeak;

    // * VmSize: Virtual memory size.
    unsigned int vmSize;

    // * VmLck: Locked memory size (see mlock(2)).
    unsigned int vmLck;

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 2, 0)
    // * VmPin: Pinned memory size (since Linux 3.2). These are
    // pages that can't be moved because something needs to
    // directly access physical memory.
    unsigned int vmPin;
    #endif

    // * VmHWM: Peak resident set size ("high water mark").
    unsigned int vmHWM;

    // * VmRSS: Resident set size. Note that the value here is the
    // sum of RssAnon, RssFile, and RssShmem.
    unsigned int vmRSS;

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 5, 0)
    // * RssAnon: Size of resident anonymous memory. (since Linux
    // 4.5).
    unsigned int rssAnon;

    // * RssFile: Size of resident file mappings. (since Linux 4.5).
    unsigned int rssFile;

    // * RssShmem: Size of resident shared memory (includes System V
    // shared memory, mappings from tmpfs(5), and shared anonymous
    // mappings). (since Linux 4.5).
    unsigned int rssShmem;
    #endif

    // * VmData, VmStk, VmExe: Size of data, stack, and text seg‐
    // ments.
    unsigned int vmData, vmStk, vmExe;

    // * VmLib: Shared library code size.
    unsigned int vmLib;

    // * VmPTE: Page table entries size (since Linux 2.6.10).
    unsigned int vmPTE;

    #if LINUX_VERSION_CODE > KERNEL_VERSION(4, 0, 0) && LINUX_VERSION_CODE < KERNEL_VERSION(4, 15, 0)
    // * VmPMD: Size of second-level page tables (added in Linux 4.0;
// removed in Linux 4.15).
// unsigned int VmPMD;
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 34)
    // * VmSwap: Swapped-out virtual memory size by anonymous private
    // pages; shmem swap usage is not included (since Linux 2.6.34).
    unsigned int vmSwap;
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0)
    // * HugetlbPages: Size of hugetlb memory portions (since Linux
    // 4.4).
    unsigned int hugetlbPages;
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 15, 0)
    // * CoreDumping: Contains the value 1 if the process is cur‐
    // rently dumping core, and 0 if it is not (since Linux 4.15).
    // This information can be used by a monitoring process to
    // avoid killing a process that is currently dumping core,
    // which could result in a corrupted core dump file.
    unsigned int coreDumping;
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 0, 0)
    // ???
unsigned int thp_enabled;
    #endif

    // * threads: Number of threads in process containing this
    // thread.
    unsigned int threads;

    // * SigQ: This field contains two slash-separated numbers that
    // relate to queued signals for the real user ID of this
    // process. The first of these is the number of currently
    // queued signals for this real user ID, and the second is the
    // resource limit on the number of queued signals for this
    // process (see the description of RLIMIT_SIGPENDING in
    // getrlimit(2)).
    // unsigned int SigQ;
    char sigQ[_UTSNAME_SYSNAME_LENGTH];

    // * SigPnd, ShdPnd:
    // Mask (expressed in hexadecimal) of signals pending for
    // thread and for process as a whole (see pthreads(7) and
    // signal(7)).
    // unsigned int SigPnd, ShdPnd;
    char sigPnd[_UTSNAME_SYSNAME_LENGTH], shdPnd[_UTSNAME_SYSNAME_LENGTH];

    // * SigBlk, SigIgn, SigCgt: Masks (expressed in hexadeximal)
    // indicating signals being blocked, ignored, and caught (see
    // signal(7)).
    // unsigned int SigBlk, SigIgn, SigCgt;
    char sigBlk[_UTSNAME_SYSNAME_LENGTH], sigIgn[_UTSNAME_SYSNAME_LENGTH], sigCgt[_UTSNAME_SYSNAME_LENGTH];

    // * CapInh, CapPrm, CapEff: Masks (expressed in hexadeximal) of
    // capabilities enabled in inheritable, permitted, and effec‐
    // tive sets (see capabilities(7)).
    // unsigned int CapInh, CapPrm, CapEff;
    char capInh[_UTSNAME_SYSNAME_LENGTH], capPrm[_UTSNAME_SYSNAME_LENGTH], capEff[_UTSNAME_SYSNAME_LENGTH];

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 26)
    // * CapBnd: Capability bounding set, expressed in hexadecimal
    // (since Linux 2.6.26, see capabilities(7)).
    // unsigned int CapBnd;
    char capBnd[_UTSNAME_SYSNAME_LENGTH];
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 3, 0)
    // * CapAmb: Ambient capability set, expressed in hexadecimal
    // (since Linux 4.3, see capabilities(7)).
    // unsigned int CapAmb;
    char capAmb[_UTSNAME_SYSNAME_LENGTH];
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 10, 0)
    // * NoNewPrivs: Value of the no_new_privs bit (since Linux 4.10,
    // see prctl(2)).
    unsigned int noNewPrivs;
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)
    // * Seccomp: Seccomp mode of the process (since Linux 3.8, see
    // seccomp(2)). 0 means SECCOMP_MODE_DISABLED; 1 means SEC‐
    // COMP_MODE_STRICT; 2 means SECCOMP_MODE_FILTER. This field
    // is provided only if the kernel was built with the CON‐
    // FIG_SECCOMP kernel configuration option enabled.
    unsigned int seccomp;
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 17, 0)
    // * Speculation_Store_Bypass: Speculation flaw mitigation state
    // (since Linux 4.17, see prctl(2)).
    char speculation_Store_Bypass[_UTSNAME_SYSNAME_LENGTH];
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24)
    // * Cpus_allowed: Hexadecimal mask of CPUs on which this process
    // may run (since Linux 2.6.24, see cpuset(7)).
    // unsigned int Cpus_allowed;
    char cpus_allowed[_UTSNAME_SYSNAME_LENGTH];
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 26)
    // * Cpus_allowed_list: Same as previous, but in "list format"
    // (since Linux 2.6.26, see cpuset(7)).
    // unsigned int Cpus_allowed_list;
    char cpus_allowed_list[_UTSNAME_SYSNAME_LENGTH];
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24)
    // * Mems_allowed: Mask of memory nodes allowed to this process
    // (since Linux 2.6.24, see cpuset(7)).
    // unsigned int Mems_allowed;
    char mems_allowed[1024];
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 26)
    // * Mems_allowed_list: Same as previous, but in "list format"
    // (since Linux 2.6.26, see cpuset(7)).
    // unsigned int Mems_allowed_list;
    char mems_allowed_list[_UTSNAME_SYSNAME_LENGTH];
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 23)
    // * voluntary_ctxt_switches, nonvoluntary_ctxt_switches: Number
    // of voluntary and involuntary context switches (since Linux
    // 2.6.23).
    unsigned int voluntary_ctxt_switches, nonvoluntary_ctxt_switches;
    #endif

    friend std::ostream &operator<<(std::ostream &os, const status &__status)
    {
        os << "Status:" << "\n" << "\tName: " << __status.name << "\n"
        #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 7, 0)
        << "\tUmask: " << __status.umask << "\n"
        #endif

        << "\tState: " << __status.state << "\n" << "\tTgid: " << __status.tgid << "\n"

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 13, 0)
        << "\tNgid: " << __status.ngid << "\n"
        #endif

        << "\tPid: " << __status.pid << "\n" << "\tPPid: " << __status.pPid << "\n" << "\tTracerPid: " << __status.tracerPid << "\n" << "\tUid: " << __status.uid << "\n" << "\tGid: " << __status.gid << "\n" << "\tFDSize: " << __status.fdSize << "\n" << "\tGroups: " << __status.groups << "\n"

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 1, 0)
        << "\tNStgid: " << __status.nsTgid << "\n" << "\tNSpid: " << __status.nspid << "\n" << "\tNSpgid: " << __status.nspgid << "\n" << "\tNSsid: " << __status.nssid << "\n"
        #endif

        << "\tVmPeak: " << __status.vmPeak << "\n" << "\tVmSize: " << __status.vmSize << "\n" << "\tVmLck: " << __status.vmLck << "\n"

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 2, 0)
        << "\tVmPin: " << __status.vmPin << "\n"
        #endif

        << "\tVmHWM: " << __status.vmHWM << "\n" << "\tVmRSS: " << __status.vmRSS << "\n"

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 5, 0)
        << "\tRssAnon: " << __status.rssAnon << "\n" << "\tRssFile: " << __status.rssFile << "\n" << "\tRssShmem: " << __status.rssShmem << "\n"
        #endif

        << "\tVmData: " << __status.vmData << "\n" << "\tVmStk: " << __status.vmStk << "\n" << "\tVmExe: " << __status.vmExe << "\n" << "\tVmLib: " << __status.vmLib << "\n" << "\tVmPTE: " << __status.vmPTE << "\n"

        #if LINUX_VERSION_CODE > KERNEL_VERSION(4, 0, 0) && LINUX_VERSION_CODE < KERNEL_VERSION(4, 15, 0)
        << "\tVmPMD: " << __status.VmPMD << "\n"
        #endif

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 34)
        << "\tVmSwap: " << __status.vmSwap << "\n"
        #endif

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0)
        << "\tHugetlbPages: " << __status.hugetlbPages << "\n"
        #endif

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 15, 0)
        << "\tCoreDumping: " << __status.coreDumping << "\n"
        #endif

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 0, 0)
        << "\tTHP_enabled: " << __status.thp_enabled << "\n"
        #endif

        << "\tThreads: " << __status.threads << "\n" << "\tSigQ: " << __status.sigQ << "\n" << "\tSigPnd: " << __status.sigPnd << "\n" << "\tShdPnd: " << __status.shdPnd << "\n" << "\tSigBlk: " << __status.sigBlk << "\n" << "\tSigIgn: " << __status.sigIgn << "\n" << "\tSigCgt: " << __status.sigCgt << "\n" << "\tCapInh: " << __status.capInh << "\n" << "\tCapPrm: " << __status.capPrm << "\n" << "\tCapEff: " << __status.capEff << "\n" << "\tCapBnd: " << __status.capBnd << "\n"

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 3, 0)
        << "\tCapAmb: " << __status.capAmb << "\n"
        #endif

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 10, 0)
        << "\tNoNewPrivs: " << __status.noNewPrivs << "\n"
        #endif

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)
        << "\tSeccomp: " << __status.seccomp << "\n"
        #endif

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 17, 0)
        << "\tSpeculation_Store_Bypass: " << __status.speculation_Store_Bypass << "\n"
        #endif

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24)
        << "\tCpus_allowed: " << __status.cpus_allowed << "\n"
        #endif

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 26)
        << "\tCpus_allowed_list: " << __status.cpus_allowed_list << "\n"
        #endif

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24)
        << "\tMems_allowed: " << __status.mems_allowed << "\n"
        #endif

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 26)
        << "\tMems_allowed_list: " << __status.mems_allowed_list << "\n"
        #endif

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 23)
        << "\tvoluntary_ctxt_switches: " << __status.voluntary_ctxt_switches << "\n" << "\tnonvoluntary_ctxt_switches: " << __status.nonvoluntary_ctxt_switches << "\n";
        #else
        ;
        #endif

        return os;
    }
};

/* Put information about the application.  */
int status(struct status* __status, const std::string &__path = "/proc/self/")
{
    std::ifstream ifstatus;
    ifstatus.open(std::string(__path + "status").c_str());
    if (!ifstatus.is_open())
    {
        std::cerr << "Error opening \"" + std::string(__path) + "\" file!\n\t" << strerror(errno) << std::endl;
        return 1;
    }

    std::vector<std::string> vec;
    std::string line;
    while (getline(ifstatus, line))
    {
        vec.push_back(line);
    }
    ifstatus.close();

    if (vec.empty())
    {
        return 0;
    }

    std::vector<std::string>::iterator it;
    it = std::find_if(vec.begin(), vec.end(), FindStr("Name"));
    if (it != vec.end())
    {
        strcpy(__status->name, substr(*it).c_str());
    }

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 7, 0)
    it = std::find_if(vec.begin(), vec.end(), FindStr("Umask"));
    if (it != vec.end())
    {
        __status->umask = std::atol(substr(*it).c_str());
    }
    #endif

    it = std::find_if(vec.begin(), vec.end(), FindStr("State"));
    if (it != vec.end())
    {
        strcpy(__status->state, substr(*it).c_str());
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("Tgid"));
    if (it != vec.end())
    {
        __status->tgid = std::atol(substr(*it).c_str());
    }

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 13, 0)
    it = std::find_if(vec.begin(), vec.end(), FindStr("Ngid"));
    if (it != vec.end())
    {
        __status->ngid = std::atol(substr(*it).c_str());
    }
    #endif

    it = std::find_if(vec.begin(), vec.end(), FindStr("Pid"));
    if (it != vec.end())
    {
        __status->pid = std::atol(substr(*it).c_str());
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("PPid"));
    if (it != vec.end())
    {
        __status->pPid = std::atol(substr(*it).c_str());
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("TracerPid"));
    if (it != vec.end())
    {
        __status->tracerPid = std::atol(substr(*it).c_str());
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("Uid"));
    if (it != vec.end())
    {
        strcpy(__status->uid, substr(*it).c_str());
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("Gid"));
    if (it != vec.end())
    {
        strcpy(__status->gid, substr(*it).c_str());
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("FDSize"));
    if (it != vec.end())
    {
        __status->fdSize = std::atol(substr(*it).c_str());
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("Groups"));
    if (it != vec.end())
    {
        strcpy(__status->groups, substr(*it).c_str());
    }

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 1, 0)
    it = std::find_if(vec.begin(), vec.end(), FindStr("NStgid"));
    if (it != vec.end())
    {
        __status->nsTgid = std::atol(substr(*it).c_str());
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("NSpid"));
    if (it != vec.end())
    {
        __status->nspid = std::atol(substr(*it).c_str());
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("NSpgid"));
    if (it != vec.end())
    {
        __status->nspgid = std::atol(substr(*it).c_str());
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("NSsid"));
    if (it != vec.end())
    {
        __status->nssid = std::atol(substr(*it).c_str());
    }
    #endif

    it = std::find_if(vec.begin(), vec.end(), FindStr("VmPeak"));
    if (it != vec.end())
    {
        __status->vmPeak = std::atol(substr(*it).c_str());
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("VmSize"));
    if (it != vec.end())
    {
        __status->vmSize = std::atol(substr(*it).c_str());
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("VmLck"));
    if (it != vec.end())
    {
        __status->vmLck = std::atol(substr(*it).c_str());
    }

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 2, 0)
    it = std::find_if(vec.begin(), vec.end(), FindStr("VmPin"));
    if (it != vec.end())
    {
        __status->vmPin = std::atol(substr(*it).c_str());
    }
    #endif

    it = std::find_if(vec.begin(), vec.end(), FindStr("VmHWM"));
    if (it != vec.end())
    {
        __status->vmHWM = std::atol(substr(*it).c_str());
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("VmRSS"));
    if (it != vec.end())
    {
        __status->vmRSS = std::atol(substr(*it).c_str());
    }

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 5, 0)
    it = std::find_if(vec.begin(), vec.end(), FindStr("RssAnon"));
    if (it != vec.end())
    {
        __status->rssAnon = std::atol(substr(*it).c_str());

    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("RssFile"));
    if (it != vec.end())
    {
        __status->rssFile = std::atol(substr(*it).c_str());

    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("RssShmem"));
    if (it != vec.end())
    {
        __status->rssShmem = std::atol(substr(*it).c_str());

    }
    #endif

    it = std::find_if(vec.begin(), vec.end(), FindStr("VmData"));
    if (it != vec.end())
    {
        __status->vmData = std::atol(substr(*it).c_str());

    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("VmStk"));
    if (it != vec.end())
    {
        __status->vmStk = std::atol(substr(*it).c_str());

    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("VmExe"));
    if (it != vec.end())
    {
        __status->vmExe = std::atol(substr(*it).c_str());

    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("VmLib"));
    if (it != vec.end())
    {
        __status->vmLib = std::atol(substr(*it).c_str());

    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("VmPTE"));
    if (it != vec.end())
    {
        __status->vmPTE = std::atol(substr(*it).c_str());
    }

    #if LINUX_VERSION_CODE > KERNEL_VERSION(4, 0, 0) && LINUX_VERSION_CODE < KERNEL_VERSION(4, 15, 0)
    it = std::find_if(vec.begin(), vec.end(), FindStr("VmPMD"));
if (it != vec.end())
{
__status->VmPMD = std::atol(substr(*it).c_str());
}
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 34)
    it = std::find_if(vec.begin(), vec.end(), FindStr("VmSwap"));
    if (it != vec.end())
    {
        __status->vmSwap = std::atol(substr(*it).c_str());

    }
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0)
    it = std::find_if(vec.begin(), vec.end(), FindStr("HugetlbPages"));
    if (it != vec.end())
    {
        __status->hugetlbPages = std::atol(substr(*it).c_str());
    }
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0)
    it = std::find_if(vec.begin(), vec.end(), FindStr("CoreDumping"));
    if (it != vec.end())
    {
        __status->coreDumping = std::atol(substr(*it).c_str());
    }
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 0, 0)
    it = std::find_if(vec.begin(), vec.end(), FindStr("THP_enabled"));
    if (it != vec.end())
    {
        __status->thp_enabled = std::atol(substr(*it).c_str());
    }
    #endif

    it = std::find_if(vec.begin(), vec.end(), FindStr("Threads"));
    if (it != vec.end())
    {
        __status->threads = std::atol(substr(*it).c_str());
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("SigQ"));
    if (it != vec.end())
    {
        strcpy(__status->sigQ, substr(*it).c_str());
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("SigPnd"));
    if (it != vec.end())
    {
        strcpy(__status->sigPnd, substr(*it).c_str());
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("ShdPnd"));
    if (it != vec.end())
    {
        strcpy(__status->shdPnd, substr(*it).c_str());
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("SigBlk"));
    if (it != vec.end())
    {
        strcpy(__status->sigBlk, substr(*it).c_str());
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("SigIgn"));
    if (it != vec.end())
    {
        strcpy(__status->sigIgn, substr(*it).c_str());
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("SigCgt"));
    if (it != vec.end())
    {
        strcpy(__status->sigCgt, substr(*it).c_str());
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("CapInh"));
    if (it != vec.end())
    {
        strcpy(__status->capInh, substr(*it).c_str());
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("CapPrm"));
    if (it != vec.end())
    {
        strcpy(__status->capPrm, substr(*it).c_str());
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("CapEff"));
    if (it != vec.end())
    {
        strcpy(__status->capEff, substr(*it).c_str());
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("CapBnd"));
    if (it != vec.end())
    {
        strcpy(__status->capBnd, substr(*it).c_str());
    }

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 3, 0)
    it = std::find_if(vec.begin(), vec.end(), FindStr("CapAmb"));
    if (it != vec.end())
    {
        strcpy(__status->capAmb, substr(*it).c_str());
    }
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 10, 0)
    it = std::find_if(vec.begin(), vec.end(), FindStr("NoNewPrivs"));
    if (it != vec.end())
    {
        __status->noNewPrivs = std::atol(substr(*it).c_str());
    }
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)
    it = std::find_if(vec.begin(), vec.end(), FindStr("Seccomp"));
    if (it != vec.end())
    {
        __status->seccomp = std::atol(substr(*it).c_str());
    }
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 17, 0)
    it = std::find_if(vec.begin(), vec.end(), FindStr("Speculation_Store_Bypass"));
    if (it != vec.end())
    {
        strcpy(__status->speculation_Store_Bypass, substr(*it).c_str());
    }
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24)
    it = std::find_if(vec.begin(), vec.end(), FindStr("Cpus_allowed"));
    if (it != vec.end())
    {
        strcpy(__status->cpus_allowed, substr(*it).c_str());
    }
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 26)
    it = std::find_if(vec.begin(), vec.end(), FindStr("Cpus_allowed_list"));
    if (it != vec.end())
    {
        strcpy(__status->cpus_allowed_list, substr(*it).c_str());
    }
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24)
    it = std::find_if(vec.begin(), vec.end(), FindStr("Mems_allowed"));
    if (it != vec.end())
    {
        strcpy(__status->mems_allowed, substr(*it).c_str());
    }
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 26)
    it = std::find_if(vec.begin(), vec.end(), FindStr("Mems_allowed_list"));
    if (it != vec.end())
    {
        strcpy(__status->mems_allowed_list, substr(*it).c_str());
    }
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 23)
    it = std::find_if(vec.begin(), vec.end(), FindStr("voluntary_ctxt_switches"));
    if (it != vec.end())
    {
        __status->voluntary_ctxt_switches = std::atol(substr(*it).c_str());
    }

    it = std::find_if(vec.begin(), vec.end(), FindStr("nonvoluntary_ctxt_switches"));
    if (it != vec.end())
    {
        __status->nonvoluntary_ctxt_switches = std::atol(substr(*it).c_str());
    }
    #endif

    return 0;
}
}