#pragma once

#include <sys/utsname.h>
#include <linux/version.h>
#include <ostream>
#include <iostream>

/**
 * Status information about the processInfo. This is used by ps(1).
 * It is defined in the kernel source file fs/proc/array.c.

 * The fields, in order, with their proper scanf(3) format speci‐
 * fiers, are listed below. Whether or not certain of these
 * fields display valid information is governed by a ptrace
 * access mode PTRACE_MODE_READ_FSCREDS | PTRACE_MODE_NOAUDIT
 * check (refer to ptrace(2)). If the check denies access, then
 * the field value is displayed as 0. The affected fields are
 * indicated with the marking [PT].
 */

namespace linuxapi
{
struct stat
{
    //(1) %d The process ID.
    int pid;

    // (2) %s The filename of the executable, in parentheses.
    // This is visible whether or not the executable is
    // swapped out.
    char comm[_UTSNAME_SYSNAME_LENGTH];

    // (3) %c One of the following characters, indicating process
    // state:
    // R Running
    // S Sleeping in an interruptible wait
    // D Waiting in uninterruptible disk sleep
    // Z Zombie
    // T Stopped (on a signal) or (before Linux 2.6.33)
    // trace stopped
    // t Tracing stop (Linux 2.6.33 onward)
    // W Paging (only before Linux 2.6.0)
    // X Dead (from Linux 2.6.0 onward)
    // x Dead (Linux 2.6.33 to 3.13 only)
    // K Wakekill (Linux 2.6.33 to 3.13 only)
    // W Waking (Linux 2.6.33 to 3.13 only)
    // P Parked (Linux 3.9 to 3.13 only)
    char state;

    // (4) %d The PID of the parent of this process.
    int ppid;

    // (5) %d The process group ID of the process.
    int pgrp;

    // (6) %d The session ID of the process.
    int session;

    // (7) %d The controlling terminal of the process. (The minor
    // device number is contained in the combination of
    // bits 31 to 20 and 7 to 0; the major device number is
    // in bits 15 to 8.)
    int tty_nr;

    // (8) %d The ID of the foreground process group of the con‐
    // trolling terminal of the process.
    int tpgid;

    // (9) %u The kernel flags word of the process. For bit mean‐
    // ings, see the PF_* defines in the Linux kernel
    // source file include/linux/sched.h. Details depend
    // on the kernel version.
    // The format for this field was %lu before Linux 2.6.
    unsigned int flags;

    // (10) %lu The number of minor faults the process has made
    // which have not required loading a memory page from
    // disk.
    unsigned long minflt;

    // (11) %lu The number of minor faults that the process's
    // waited-for children have made.
    unsigned long cminflt;

    // (12) %lu The number of major faults the process has made
    // which have required loading a memory page from disk.
    unsigned long majflt;

    // (13) %lu
    // The number of major faults that the process's
    // waited-for children have made.
    unsigned long cmajflt;

    // (14) %lu
    // Amount of time that this process has been scheduled
    // in user mode, measured in clock ticks (divide by
    // sysconf(_SC_CLK_TCK)). This includes guest time,
    // guest_time (time spent running a virtual CPU, see
    // below), so that applications that are not aware of
    // the guest time field do not lose that time from
    // their calculations.
    unsigned long utime;

    // (15) %lu
    // Amount of time that this process has been scheduled
    // in kernel mode, measured in clock ticks (divide by
    // sysconf(_SC_CLK_TCK)).
    unsigned long stime;

    // (16) %ld
    // Amount of time that this process's waited-for chil‐
    // dren have been scheduled in user mode, measured in
    // clock ticks (divide by sysconf(_SC_CLK_TCK)). (See
    // also times(2).) This includes guest time,
    // cguest_time (time spent running a virtual CPU, see
    // below).
    unsigned int cutime;

    // (17) %ld
    // Amount of time that this process's waited-for chil‐
    // dren have been scheduled in kernel mode, measured in
    // clock ticks (divide by sysconf(_SC_CLK_TCK)).
    unsigned int cstime;

    // (18) %ld
    // (Explanation for Linux 2.6) For processes running a
    // real-time scheduling policy (policy below; see
    // sched_setscheduler(2)), this is the negated schedul‐
    // ing priority, minus one; that is, a number in the
    // range -2 to -100, corresponding to real-time priori‐
    // ties 1 to 99. For processes running under a non-
    // real-time scheduling policy, this is the raw nice
    // value (setpriority(2)) as represented in the kernel.
    // The kernel stores nice values as numbers in the
    // range 0 (high) to 39 (low), corresponding to the
    // user-visible nice range of -20 to 19.
    //
    // Before Linux 2.6, this was a scaled value based on
    // the scheduler weighting given to this process.
    unsigned int priority;

    // (19) %ld
    // The nice value (see setpriority(2)), a value in the
    // range 19 (low priority) to -20 (high priority).
    unsigned int nice;

    // (20) %ld
    // Number of threads in this process (since Linux 2.6).
    // Before kernel 2.6, this field was hard coded to 0 as
    // a placeholder for an earlier removed field.
    unsigned int num_threads;

    // (21) %ld
    // The time in jiffies before the next SIGALRM is sent
    // to the process due to an interval timer. Since ker‐
    // nel 2.6.17, this field is no longer maintained, and
    // is hard coded as 0.
    unsigned int itrealvalue;

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
    // (22) %llu
    // The time the process started after sys boot. In
    // kernels before Linux 2.6, this value was expressed
    // in jiffies. Since Linux 2.6, the value is expressed
    // in clock ticks (divide by sysconf(_SC_CLK_TCK)).
    //
    // The format for this field was %lu before Linux 2.6.
    unsigned long long starttime;
    #endif

    // (23) %lu
    // Virtual memory size in bytes.
    unsigned long vsize;

    // (24) %ld
    // Resident Set Size: number of pages the process has
    // in real memory. This is just the pages which count
    // toward text, data, or stack space. This does not
    // include pages which have not been demand-loaded in,
    // or which are swapped out.
    unsigned int rss;

    // (25) %lu
    // Current soft limit in bytes on the rss of the
    // process; see the description of RLIMIT_RSS in
    // getrlimit(2).
    unsigned long rsslim;

    // (26) %lu [PT]
    // The address above which program text can run.
    unsigned long startcode;

    // (27) %lu [PT]
    // The address below which program text can run.
    unsigned long endcode;

    // (28) %lu [PT]
    // The address of the start (i.e., bottom) of the
    // stack.
    unsigned long startstack;

    // (29) %lu [PT]
    // The current value of ESP (stack pointer), as found
    // in the kernel stack page for the process.
    unsigned long kstkesp;

    // (30) %lu [PT]
    // The current EIP (instruction pointer).
    unsigned long kstkeip;

    // (31) %lu
    // The bitmap of pending signals, displayed as a deci‐
    // mal number. Obsolete, because it does not provide
    // information on real-time signals; use
    // /proc/[pid]/status instead.
    unsigned long signal;

    // (32) %lu
    // The bitmap of blocked signals, displayed as a deci‐
    // mal number. Obsolete, because it does not provide
    // information on real-time signals; use
    // /proc/[pid]/status instead.
    unsigned long blocked;

    // (33) %lu
    // The bitmap of ignored signals, displayed as a deci‐
    // mal number. Obsolete, because it does not provide
    // information on real-time signals; use
    // /proc/[pid]/status instead.
    unsigned long sigignore;

    // (34) %lu
    // The bitmap of caught signals, displayed as a decimal
    // number. Obsolete, because it does not provide
    // information on real-time signals; use
    // /proc/[pid]/status instead.
    unsigned long sigcatch;

    // (35) %lu [PT]
    // This is the "channel" in which the process is wait‐
    // ing. It is the address of a location in the kernel
    // where the process is sleeping. The corresponding
    // symbolic name can be found in /proc/[pid]/wchan.
    unsigned long wchan;

    // (36) %lu
    // Number of pages swapped (not maintained).
    unsigned long nswap;

    // (37) %lu
    // Cumulative nswap for child processes (not main‐
    // tained).
    unsigned long cnswap;

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 1, 22)
    // (38) %d (since Linux 2.1.22)
    // Signal to be sent to parent when we die.
    int exit_signal;
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 2, 8)
    // (39) %d (since Linux 2.2.8)
    // CPU number last executed on.
    int processor;
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 5, 19)
    // (40) %u (since Linux 2.5.19)
    // Real-time scheduling priority, a number in the range
    // 1 to 99 for processes scheduled under a real-time
    // policy, or 0, for non-real-time processes (see
    // sched_setscheduler(2)).
    unsigned int rt_priority;

    // (41) %u (since Linux 2.5.19)
    // Scheduling policy (see sched_setscheduler(2)).
    // Decode using the SCHED_* constants in linux/sched.h.
    //
    // The format for this field was %lu before Linux
    // 2.6.22.
    unsigned int policy;
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 18)
    // (42) %llu (since Linux 2.6.18)
    // Aggregated block I/O delays, measured in clock ticks
    // (centiseconds).
    unsigned long long delayacct_blkio_ticks;
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24)
    // (43) %lu (since Linux 2.6.24)
    // Guest time of the process (time spent running a vir‐
    // tual CPU for a guest operating sys), measured in
    // clock ticks (divide by sysconf(_SC_CLK_TCK)).
    unsigned long guest_time;

    // (44) %ld (since Linux 2.6.24)
    // Guest time of the process's children, measured in
    // clock ticks (divide by sysconf(_SC_CLK_TCK)).
    unsigned int cguest_time;
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 3, 0)
    // (45) %lu (since Linux 3.3) [PT]
    // Address above which program initialized and unini‐
    // tialized (BSS) data are placed.
    unsigned long start_data;

    //(46) %lu (since Linux 3.3) [PT]
    // Address below which program initialized and unini‐
    // tialized (BSS) data are placed.
    unsigned long end_data;

    // (47) %lu (since Linux 3.3) [PT]
    // Address above which program heap can be expanded
    // with brk(2).
    unsigned long start_brk;
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 5, 0)
    // (48) %lu (since Linux 3.5) [PT]
    // Address above which program command-line arguments
    // (argv) are placed.
    unsigned long arg_start;

    // (49) %lu (since Linux 3.5) [PT]
    // Address below program command-line arguments (argv)
    // are placed.
    unsigned long arg_end;

    // (50) %lu (since Linux 3.5) [PT]
    // Address above which program environment is placed.
    unsigned long env_start;

    // (51) %lu (since Linux 3.5) [PT]
    // Address below which program environment is placed.
    unsigned long env_end;

    // (52) %d (since Linux 3.5) [PT]
    // The thread's exit status in the form reported by
    // waitpid(2).
    int exit_code;
    #endif

    friend std::ostream &operator<<(std::ostream &os, const stat &__stat)
    {
        os << "Stat:" << "\n" << "\tpid: " << __stat.pid << "\n" << "\tcomm: " << __stat.comm << "\n" << "\tstate: " << __stat.state << "\n" << "\tppid: " << __stat.ppid << "\n" << "\tpgrp: " << __stat.pgrp << "\n" << "\tsession: " << __stat.session << "\n" << "\ttty_nr: " << __stat.tty_nr << "\n" << "\ttpgid: " << __stat.tpgid << "\n" << "\tflags: " << __stat.flags << "\n" << "\tminflt: " << __stat.minflt << "\n" << "\tcminflt: " << __stat.cminflt << "\n" << "\tmajflt: " << __stat.majflt << "\n" << "\tcmajflt: " << __stat.cmajflt << "\n" << "\tutime: " << __stat.utime << "\n" << "\tstime: " << __stat.stime << "\n" << "\tcutime: " << __stat.cutime << "\n" << "\tcstime: " << __stat.cstime << "\n" << "\tpriority: " << __stat.priority << "\n" << "\tnice: " << __stat.nice << "\n" << "\tnum_threads: " << __stat.num_threads << "\n" << "\titrealvalue: " << __stat.itrealvalue << "\n" <<

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
        "\tstarttime: " << __stat.starttime << "\n" <<
        #endif

        "\tvsize: " << __stat.vsize << "\n" << "\trss: " << __stat.rss << "\n" << "\trsslim: " << __stat.rsslim << "\n" << "\tstartcode: " << __stat.startcode << "\n" << "\tstartstack: " << __stat.startstack << "\n" << "\tkstkesp: " << __stat.kstkesp << "\n" << "\tkstkeip: " << __stat.kstkeip << "\n" << "\tsignal: " << __stat.signal << "\n" << "\tblocked: " << __stat.blocked << "\n" << "\tsigignore: " << __stat.sigignore << "\n" << "\tsigcatch: " << __stat.sigcatch << "\n" << "\twchan: " << __stat.wchan << "\n" << "\tnswap: " << __stat.nswap << "\n" << "\tcnswap: " << __stat.cnswap << "\n" <<

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 1, 22)
        "\texit_signal: " << __stat.exit_signal << "\n" <<
        #endif

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 2, 8)
        "\tprocessor: " << __stat.processor << "\n" <<
        #endif

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 5, 19)
        "\trt_priority: " << __stat.rt_priority << "\n" << "\tpolicy: " << __stat.policy << "\n" <<
        #endif

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 18)
        "\tdelayacct_blkio_ticks: " << __stat.delayacct_blkio_ticks << "\n" <<
        #endif

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24)
        "\tguest_time: " << __stat.guest_time << "\n" << "\tcguest_time: " << __stat.cguest_time << "\n" <<
        #endif

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 3, 0)
        "\tstart_data: " << __stat.start_data << "\n" << "\tend_data: " << __stat.end_data << "\n" << "\tstart_brk: " << __stat.start_brk << "\n" <<
        #endif

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 5, 0)
        "\targ_start: " << __stat.arg_start << "\n" << "\targ_end: " << __stat.arg_end << "\n" << "\tenv_start: " << __stat.env_start << "\n" << "\tenv_end: " << __stat.env_end << "\n" << "\texit_code: " << __stat.exit_code <<
        #endif
        "\t\n";

        return os;
    }
};

/* Put information about the application.  */
bool stat(struct stat* __stat, const std::string &__path = "/proc/self/")
{
    std::ifstream ifstat;
    ifstat.open(std::string(__path + "stat").c_str());
    if (!ifstat.is_open())
    {
        std::cerr << "Error opening \"" + std::string(__path) + "\" file!\n\t" << strerror(errno) << std::endl;
        return true;
    }

    ifstat >> __stat->pid >> __stat->comm >> __stat->state >> __stat->ppid >> __stat->pgrp >> __stat->session >> __stat->tty_nr >> __stat->tpgid >> __stat->flags >> __stat->minflt >> __stat->cminflt >> __stat->majflt >> __stat->cmajflt >> __stat->utime >> __stat->stime >> __stat->cutime >> __stat->cstime >> __stat->priority >> __stat->nice >> __stat->num_threads >> __stat->itrealvalue

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
    >> __stat->starttime
    #endif

    >> __stat->vsize >> __stat->rss >> __stat->rsslim >> __stat->startcode >> __stat->endcode >> __stat->startstack >> __stat->kstkesp >> __stat->kstkeip >> __stat->signal >> __stat->blocked >> __stat->sigignore >> __stat->sigcatch >> __stat->wchan >> __stat->nswap >> __stat->cnswap

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 1, 22)
    >> __stat->exit_signal
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 2, 8)
    >> __stat->processor
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 5, 19)
    >> __stat->rt_priority >> __stat->policy
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 18)
    >> __stat->delayacct_blkio_ticks
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24)
    >> __stat->guest_time >> __stat->cguest_time
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 3, 0)
    >> __stat->start_data >> __stat->end_data >> __stat->start_brk
    #endif

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 5, 0)
    >> __stat->arg_start >> __stat->arg_end >> __stat->env_start >> __stat->env_end >> __stat->exit_code;
    #else
    ;
    #endif

    ifstat.close();

    return false;
}

}