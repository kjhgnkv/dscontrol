#ifndef __{ { short_name } }Activator_H__
    #define __{{short_name}}Activator_H__

    #include <ctkPluginActivator.h>
    #include <QScopedPointer>
    #include <QList>
    #include "{{full_name}}_Export.h"
    #include "ICommObject.h"
    #include "ProtobufProcessor.h"
    #include "src/WindowWidgetPlugin.h"

/** 插件 {{plugin_name}} 的激活器 */
class
{
    {
        full_name
    }
} _Export {{short_name}}
Activator : public QObject, public ctkPluginActivator
{
Q_OBJECT
Q_INTERFACES(ctkPluginActivator)

    #ifdef HAVE_QT5
Q_PLUGIN_METADATA(IID "{{full_name}}")
    #endif

private:
static {
{
short_name}}
Activator* instance;

public:
explicit {
{
short_name}}
Activator();
static {
{
short_name}}

Activator* getInstance()
{
    return instance;
}/**< 获取激活器单例 */

void start(ctkPluginContext* context);/**< 启动插件 */
void stop(ctkPluginContext* context);/**< 停止插件 */

/** 准备内部总线通信 */
void prepareInnerBusComm();
/**
 * @brief 处理从内部总线接收的消息
 * @param topic [输入] 消息主题
 * @param src_id [输入] 消息源ID
 * @param msg [输入] 消息体
 */
void handleInnerBusMessage(const QString &topic, const QString &src_id, const QByteArray &msg);
/**
 * @brief 通过内部总线发送消息
 * @param topic [输入] 消息主题
 * @param msg [输入] 消息体
 * @return 消息发布是否成功
 */
bool publishMessage(QString &topic, QByteArray &msg);
/**
 * @brief 通过内部总线发送一个Protobuf消息
 * @param proto [输入] Protobuf实例
 * @param para [输入] 参数
 * @return 消息发布是否成功
 */
bool publishProto(google::protobuf::Message* proto, const QString &para = QString());

/**
 * @brief 注册一个Protobuf的处理器
 * @param msg [输入] 关联的Protobuf消息
 * @param proc [输入] 处理器
 */
void registerProtoProcessor(ProtoMsg_ptr msg, ProtobufProcessor* proc);
/**
 * @brief 注销一个Protobuf的处理器
 * @param proc [输入] 处理器
 */
void unregisterProtoProcessor(ProtobufProcessor* proc);

void getServices();/**< 从框架获取服务 */
void regServices();/**< 向框架注册服务 */

private:
ctkPluginContext* pCxt;/**< 框架上下文 */
QScopedPointer<core::ICommObject> pComm;/**< 内部总线通信实例 */
QList<PROTO_PROC_PAIR> protoProcessors;/**< Protobuf处理器列表 */
WindowWidgetPlugin* pWidget;/**< 插件窗口实例 */

private:
/**
 * @brief 以"prefix:type?para"的模式解析消息主题
 * @param topic [输入] 消息主题
 * @param prefix [输出] 主题前缀
 * @param type [输出] 主题类型
 * @param para [输出] 主题参数
 * @return 解析是否成功
 */
bool parseTopic(const QString &topic, QString &prefix, QString &type, QString &para);
/**
 * @brief 尝试将一个内部总线消息解析为Protobuf消息
 * @param type [输入] 主题类型
 * @param msg [输入] 消息体
 * @param proto [输出] Protobuf消息
 * @return 解析是否成功
 */
bool parseProtobuf(const QString &type, const QByteArray &msg, google::protobuf::Message &proto);
};

/** 生成目录 */
bool generateDir(QString parent, QString child);
/** 处理内部总线消息 */
void onInnerBusMessage(QString topic, QString src_id, char comm_type, const QByteArray &msg);

#endif // __{{short_name}}Activator_H__
