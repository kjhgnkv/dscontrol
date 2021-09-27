#include "{{wnd_class_name}}.h"
#include "ui_{{wnd_class_name}}.h"

{
{
wnd_class_name}}::{
{
wnd_class_name}}(
QWidget* parent
) :
ui(new Ui::{
{
wnd_class_name}})
{
setParent(parent);
ui->setupUi(this);
}

{
{
wnd_class_name}}
::~{{wnd_class_name}}
()
{
delete
ui;
}

void {
{
wnd_class_name}}::setTitle(
const QString &aTitle
)
{
Q_UNUSED(aTitle);
}

QString {
{
wnd_class_name}}::getTitle() const
{
return "{{wnd_name}}";
}

QString {
{
wnd_class_name}}::getChineseTitle() const
{
return tr("{{wnd_display_name}}");
}

QString {
{
wnd_class_name}}::getIconFilename()
{
return "{{wnd_icon}}";
}

void {
{
wnd_class_name}}::setIconFilename(
const QString &aIconFilename
)
{
Q_UNUSED(aIconFilename);
}

bool {
{
wnd_class_name}}::isActive()
{
return true;
}

PluginMenuView *{
{
wnd_class_name}}::getPluginMenuView() const
{
return
NULL;
}
