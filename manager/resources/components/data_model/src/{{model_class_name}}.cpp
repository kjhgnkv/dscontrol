#include <QDebug>
#include "Activator.h"
#include "{{model_class_name}}.h"

{
{
model_class_name}}::{
{
model_class_name}}(
QObject* parent
):
dm::SimpleModel(QString("{{model_id}}"), parent
)
{
//register protobuf
{
{
short_name}}
Activator* pActivator = {{short_name}}
Activator::getInstance();
if(pActivator) {
//TODO
//pActivator->registerProtoProcessor(ProtoMsg_ptr(new proto_object()),this);
}
}

{
{
model_class_name}}
::~{{model_class_name}}
()
{
{
{
short_name}}
Activator* pActivator = {{short_name}}
Activator::getInstance();
if(pActivator) {
pActivator->unregisterProtoProcessor(this);
}
}

void {
{
model_class_name}}::process(
const ProtoMsg_ptr &msg,
const QString &type,
const QString &para,
const QString &src_id
)
{
Q_UNUSED(msg)
Q_UNUSED(type)
Q_UNUSED(para)
Q_UNUSED(src_id);
//TODO
}
