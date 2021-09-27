#include <ctkPluginContext.h>
#include <QtPlugin>
#include <QDir>
#include <QFileInfo>
#include <QTextCodec>
#include "Activator.h"

{
{
short_name}}Activator *{
{
short_name}}
Activator::instance = 0;

{
{
short_name}}Activator::{
{
short_name}}

Activator()
: pCxt(0)
{
    //set instance
    instance = this;
}

void {
{
short_name}}
Activator::start(ctkPluginContext
*context)
{
//record context
pCxt = context;

//set default codec to UTF-8
QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8")
);
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#endif

//build record folder
generateDir("","record");
generateDir("record","{{plugin_name}}");

//get necessary services
getServices();

//register necessary services
regServices();
}

void {
{
short_name}}
Activator::stop(ctkPluginContext
*context)
{
Q_UNUSED(context);
pCxt = 0;//clear context
}

bool generateDir(QString parent, QString child)
{
    QDir pDir(parent);//parent dir
    if (!pDir.exists())
    {//check existence of parent directory
        return false;
    }
    QFileInfoList entrys = pDir.entryInfoList();//get entry list
    //Check whether the child directory exists.
    foreach(QFileInfo entry, entrys)
        {
            if ((entry.fileName() == child) && (entry.isDir()))
            {
                return true;
            }
        }
    //If it doesn't exist, then create it
    return pDir.mkdir(child);
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN2({{full_name}}, {{short_name}}Activator)
#endif
