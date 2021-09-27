#include "WindowWidgetPlugin.h"
#include "{{wnd_class_name}}.h"

WindowWidgetPlugin::WindowWidgetPlugin()
{
    createWidget();
}

WindowWidgetPlugin::~WindowWidgetPlugin()
{
}

void WindowWidgetPlugin::createWidget()
{
    ICenterWindow * centerWin = new {{wnd_class_name}}
    ();
    m_centerWindowList.append(centerWin);
}

QList<ICenterWindow*> WindowWidgetPlugin::getCenterWindow()
{
    return m_centerWindowList;
}

QList<ISettingWindow*> WindowWidgetPlugin::getSettingWindow()
{
    return m_settingWindowList;
}

QList<IPluginInfo*> WindowWidgetPlugin::getPluginInfoWindow()
{
    return m_pluginInfoList;
}
