#ifndef __{ { worker_class_name } }_H__
    #define __{{worker_class_name}}_H__

    #include <QThread>
    #include <QScopedPointer>
    #include "IWorker.h"

/** work thread related with the worker */
class
{
    {
        thread_class_name
    }
}
: public QThread
{
Q_OBJECT
private:
bool runFlag;/**< flag to control thread running */

public:
explicit {
{
thread_class_name}}(
QObject* parent = NULL
);
~{
{
thread_class_name}}();

void startWork();/**< start work */
void stopWork();/**< stop work */

protected:
virtual void run();
};

/** worker class */
class
{
    {
        worker_class_name
    }
}
: public IWorker
{
Q_OBJECT
private:
QScopedPointer<{{thread_class_name}}> thd;

public:
explicit {
{
worker_class_name}}(
QObject* parent = NULL
);

/**
  * @brief configure worker
  * @param cfg [IN] - configuration parameters
  */
virtual void config(const ParaHash &cfg);
/**
  * @brief get name of worker
  * @return name of worker
  */
virtual QString getName();
/**
  * @brief get description of worker
  * @return description of worker
  */
virtual QString getDescription();
/** @brief start working */
virtual void start();
/** @brief stop working */
virtual void stop();
/**
  * @brief get status of worker
  * @return status of worker
  */
virtual bool isWorking();
};

#endif // __{{worker_class_name}}_H__
