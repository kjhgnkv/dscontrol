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

void {
{
short_name}}

Activator::getServices()
{
    //The following is an example to get a service with the class type "SeviceClass"
    //TODO: get necessary services according to this example
    #if 0
    ServiceClass *service = 0;
    if(pCxt) {
        QList<ctkServiceReference> refs = pCxt->getServiceReferences(QString("SeviceClass"),QString());//get references of service
        foreach(const ctkServiceReference &ref,refs) {
            service = dynamic_cast<SeviceClass *>(pCxt->getService(ref));//try get service
            if(service) {
                break;
            }
        }
    }
    #endif
}

void {
{
short_name}}

Activator::regServices()
{
    ctkDictionary props;
    props.insert(ctkPluginConstants::SERVICE_RANKING, 0);//prepare dictionary

    //The following is an example to register a service with the class type "MySevice"
    //TODO: register necessary services according to this example
    #if 0
    MyService *my_service = new Myservice;
    if(pCxt) {
        pCxt->registerService(QStringList("Myservice"),my_service, props);//register service
    }
    #endif
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
