#pragma once

#include <QFutureWatcher>
#include <QObject>
#include <QThreadPool>

class GlobalInstance : public QObject
{
public:
    explicit GlobalInstance();
    ~GlobalInstance();

    static GlobalInstance &Instance();

    GlobalInstance(GlobalInstance const &) = delete;
    GlobalInstance &operator=(GlobalInstance const &) = delete;

    GlobalInstance(GlobalInstance &&) = delete;
    GlobalInstance &operator=(GlobalInstance &&) = delete;

    QThreadPool* pool() const;

private:
    QThreadPool* pool_;
};
