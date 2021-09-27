#include <ctkPluginContext.h>
#include <QtPlugin>
#include <QDir>
#include <QFileInfo>
#include <QTextCodec>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include "Activator.h"
#include "ICommObjectFactory.h"

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
, pWidget(0)
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

//prepare communication with inner bus
prepareInnerBusComm();

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
pComm.
reset();//release communication object
}

void {
{
short_name}}

Activator::prepareInnerBusComm()
{
    if (pComm.isNull() && pCxt)
    {
        QList<ctkServiceReference> refs = pCxt->getServiceReferences(QString("core::ICommObjectFactory"),
                                                                     QString());//get references of service
        foreach(const ctkServiceReference &ref, refs)
            {
                core::ICommObjectFactory* fac = dynamic_cast<core::ICommObjectFactory*>(pCxt->getService(
                ref));//try get factory
                if (fac && (fac->aim() == "InnerBus"))
                {//check factory
                    QStringList topics;
                    //TODO: setup listening topics
                    //topics<<"pb:send_datagram";
                    pComm.reset(fac->createCommObject("{{full_name}}", topics));//create comm object
                    break;
                }
            }
    }
    //setup handler
    if (pComm)
    {
        pComm->setMessageHandler(onInnerBusMessage);
    }
}

void {
{
short_name}}

Activator::handleInnerBusMessage(const QString &topic, const QString &src_id, const QByteArray &msg)
{
    //parse topic
    QString t_prefix, t_type, t_para;
    if (!parseTopic(topic, t_prefix, t_type, t_para))
    {
        return;
    }
    //process
    foreach (PROTO_PROC_PAIR pair, protoProcessors)
        {//iterate list
            if (pair.msg && pair.proc)
            {
                if (parseProtobuf(t_type, msg, *pair.msg))
                {//parse protobuf
                    pair.proc->process(pair.msg, t_type, t_para, src_id);
                }
            }
        }
}

bool {
{
short_name}}
Activator::publishMessage(QString
&topic,
QByteArray &msg
)
{
if(pComm) {
return pComm->
publishMessage(topic, msg
);
}
return false;
}

bool {
{
short_name}}
Activator::publishProto(google::protobuf::Message
*proto,
const QString &para
)
{
//get proto type
QString p_type = QString::fromStdString(proto->GetTypeName());//get protobuf object type
QRegularExpression re_proto("^(\\w*\\.)*(?<type>\\w+)$");//prepare regular expression
QRegularExpressionMatch match = re_proto.match(p_type);//match
if(match.
hasMatch()
) {
p_type = match.captured("type");//fetch type
}
//prepare topic
QString topic = QString("pb:%1").arg(p_type);
if(para.
size()
>0) {
topic += QString("?%1").
arg(para);
}
//publish
std::string str = proto->SerializeAsString();
return
publishMessage(topic, QByteArray::fromRawData(str.data(), (int)str.size())
);
}

void {
{
short_name}}
Activator::registerProtoProcessor(ProtoMsg_ptr
msg,
ProtobufProcessor* proc
)
{
protoProcessors.
append(PROTO_PROC_PAIR(msg, proc)
);
}

void {
{
short_name}}
Activator::unregisterProtoProcessor(ProtobufProcessor
*proc)
{
QList<PROTO_PROC_PAIR>::Iterator i = protoProcessors.begin();//iterate list
while(i!=protoProcessors.
end()
) {
PROTO_PROC_PAIR &pair = *i;//get pair
if(pair.proc==proc) {
i = protoProcessors.erase(i);
}
else {
i++;
}
}
}

bool {
{
short_name}}

Activator::parseTopic(const QString &topic, QString &prefix, QString &type, QString &para)
{
    QRegularExpression re("^((?<prefix>\\w+):)?(?<type>\\w+)(\\?(?<para>.*))?$");//prepare regular expression
    QRegularExpressionMatch match = re.match(topic);//match topic
    prefix = match.captured("prefix");//prefix
    type = match.captured("type");//type
    para = match.captured("para");//parameters
    return match.hasMatch();
}

bool {
{
short_name}}

Activator::parseProtobuf(const QString &type, const QByteArray &msg, google::protobuf::Message &proto)
{
    //check type
    QString p_type = QString::fromStdString(proto.GetTypeName());//get protobuf object type
    QRegularExpression re_proto("^(\\w*\\.)*(?<type>\\w+)$");//prepare regular expression
    QRegularExpressionMatch match = re_proto.match(p_type);//match
    if (match.hasMatch())
    {
        p_type = match.captured("type");//fetch type
    }
    if (type != p_type)
    {
        return false;
    }
    //try parse
    return proto.ParseFromArray(msg.data(), msg.size());
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

    //register widget as a service
    if ((pWidget == 0) && pCxt)
    {
        pWidget = new WindowWidgetPlugin();
        pCxt->registerService(QStringList("IWidgetPlugin"), pWidget, props);
    }
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

void onInnerBusMessage(QString topic, QString src_id, char comm_type, const QByteArray &msg)
{
    Q_UNUSED(comm_type);
    {
        {
            short_name
        }
    }
    Activator * pActivator = {{short_name}}
    Activator::getInstance();
    if (pActivator)
    {
        pActivator->handleInnerBusMessage(topic, src_id, msg);
    }
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN2({{full_name}}, {{short_name}}Activator)
#endif
