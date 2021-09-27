#ifndef __PROTOBUF_PROCESSOR_H__
#define __PROTOBUF_PROCESSOR_H__

#include <google/protobuf/message.h>
#include <QSharedPointer>
#include <QString>

/** Protobuf消息的共享指针 */
typedef QSharedPointer<google::protobuf::Message> ProtoMsg_ptr;

/** Protobuf消息处理器的抽象父类 */
class ProtobufProcessor
{
public:
    virtual ~ProtobufProcessor()
    {
    }

    /**
     * @brief 处理Protobuf消息
     * @param msg [输入] Protobuf消息体
     * @param type [输入] 主题类型
     * @param para [输入] 主题参数
     * @param src_id [输入] 消息的发送方ID
     */
    virtual void process(const ProtoMsg_ptr &msg, const QString &type, const QString &para, const QString &src_id) = 0;
};

/** Protobuf消息和它的处理器组成的对结构体 */
struct PROTO_PROC_PAIR
{
    ProtoMsg_ptr msg;/**< Protobuf消息 */
    ProtobufProcessor* proc;/**< 处理器 */
    PROTO_PROC_PAIR()
    {
    }

    PROTO_PROC_PAIR(ProtoMsg_ptr m, ProtobufProcessor* p)
    : msg(m)
    , proc(p)
    {
    }
};

#endif // __PROTOBUF_PROCESSOR_H__
