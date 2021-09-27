#include <QDebug>
#include "{{worker_class_name}}.h"

{
{
thread_class_name}}::{
{
thread_class_name}}(
QObject* parent
):

QThread (parent), runFlag(false)
{
}

{
{
thread_class_name}}
::~{{thread_class_name}}
()
{
//stop work
if(
isRunning()
) {
stopWork();
while(
isRunning()
);
}
}

void {
{
thread_class_name}}::startWork()
{
if(!
isRunning()
) {
runFlag = true;
start();
}
}

void {
{
thread_class_name}}::stopWork()
{
runFlag = false;
}

void {
{
thread_class_name}}::run()
{
//check flag
while(runFlag) {
//TODO thread work
//sleep
msleep(10);
}
}

{
{
worker_class_name}}::{
{
worker_class_name}}(
QObject* parent
)
{
//set parent
setParent(parent);
//prepare thread
thd.reset(new {
{
thread_class_name}}());
connect(thd
.
data(), SIGNAL(started()),
this,SIGNAL(started()));
connect(thd
.
data(), SIGNAL(finished()),
this,SIGNAL(stopped()));
}

void {
{
worker_class_name}}::config(
const IWorker::ParaHash &cfg
)
{
Q_UNUSED(cfg);
}

QString {
{
worker_class_name}}::getName()
{
return tr("{{worker_name}}");
}

QString {
{
worker_class_name}}::getDescription()
{
return tr("{{worker_description}}");
}

void {
{
worker_class_name}}::start()
{
//start thread
thd->
startWork();
}

void {
{
worker_class_name}}::stop()
{
//stop thread
thd->
stopWork();
}

bool {
{
worker_class_name}}::isWorking()
{
//return state of thread
return thd->
isRunning();
}
