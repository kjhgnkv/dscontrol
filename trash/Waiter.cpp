#include "Waiter.hpp"
#include <QThread>

Waiter::Waiter()
: waitLimit_ {0}
, timeCondition_ {0}
, summary_ {0}
{
}

void Waiter::setWaitLimit(const unsigned long &msecs)
{
    waitLimit_ = msecs;
}

void Waiter::setTimeCondition(const unsigned long &msecs)
{
    timeCondition_ = msecs;
}

bool Waiter::waitFor(const unsigned long &msecs)
{
    if (summary_ >= waitLimit_)
    {
        return false;
    }

    auto sleepTime = msecs;

    auto tmp = summary_ + msecs;
    if (tmp >= waitLimit_)
    {
        sleepTime = waitLimit_ - summary_;
    }

    QThread::msleep(sleepTime);
    summary_ += sleepTime;

    return true;
}

void Waiter::reset()
{
    summary_ = 0;
}

bool Waiter::isInterRange() const
{
    return (summary_ % timeCondition_) == 0;
}
