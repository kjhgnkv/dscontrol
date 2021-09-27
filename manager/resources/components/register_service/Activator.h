#ifndef __{ { short_name } }Activator_H__
    #define __{{short_name}}Activator_H__

    #include <ctkPluginActivator.h>
    #include "{{full_name}}_Export.h"

/** 插件 {{plugin_name}} 的激活器 */
class
{
    {
        full_name
    }
} _Export {{short_name}}
Activator : public QObject, public ctkPluginActivator
{
Q_OBJECT Q_INTERFACES(ctkPluginActivator)

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

void getServices();/**< 从框架获取服务 */
void regServices();/**< 向框架注册服务 */

private:
ctkPluginContext* pCxt;/**< 框架上下文 */
};

/** 生成目录 */
bool generateDir(QString parent, QString child);

#endif // __{{short_name}}Activator_H__
