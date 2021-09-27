#ifndef __{ { model_class_name } }_H__
    #define __{{model_class_name}}_H__

    #include "dm_SimpleModel.h"
    #include "ProtobufProcessor.h"

/** 数据模型类 */
class
{
    {
        model_class_name
    }
}
: public dm::SimpleModel, public ProtobufProcessor
{
Q_OBJECT
public:
explicit {
{
model_class_name}}(
QObject* parent = NULL
);
~{
{
model_class_name}}();

/**
 * @brief 处理Protobuf消息
 * @param msg [输入] Protobuf消息体
 * @param type [输入] 主题类型
 * @param para [输入] 主题参数
 * @param src_id [输入] 消息的发送方ID
 */
virtual void process(const ProtoMsg_ptr &msg, const QString &type, const QString &para, const QString &src_id);
};

#endif // __{{model_class_name}}_H__
