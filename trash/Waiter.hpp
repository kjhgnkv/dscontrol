#pragma once

class Waiter
{
public:
    explicit Waiter();

    void setWaitLimit(const unsigned long &msecs);
    void setTimeCondition(const unsigned long &msecs);
    bool waitFor(const unsigned long &msecs);
    void reset();
    bool isInterRange() const;

private:
    unsigned long waitLimit_;
    unsigned long timeCondition_;
    unsigned long summary_;
};
