#include "Thread.hpp"

using namespace monitor;

Thread::Thread(const QString &pid, const QString &cpu)
: pid_(pid)
, cpu_(cpu)
{
}

void Thread::update(const QString &pid, const QString &cpu) {
    pid_ = pid;
    cpu_ = cpu;
}

QString Thread::pid() const {
    return pid_;
}

QString Thread::cpu() const {
    return cpu_;
}
