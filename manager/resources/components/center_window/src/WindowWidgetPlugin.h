#ifndef WindowWidgetPlugin_H
#define WindowWidgetPlugin_H

#include <QWidget>
#include "IWidgetPlugin.h"

class WindowWidgetPlugin : public IWidgetPlugin
{
Q_OBJECT

public:
    WindowWidgetPlugin();
    virtual ~WindowWidgetPlugin();

    /* Plugin interfaces */
    QList<ICenterWindow*> getCenterWindow();
    QList<ISettingWindow*> getSettingWindow();
    QList<IPluginInfo*> getPluginInfoWindow();

private:
    QList<ICenterWindow*> m_centerWindowList;
    QList<ISettingWindow*> m_settingWindowList;
    QList<IPluginInfo*> m_pluginInfoList;

    void createWidget();
};

#endif // WindowWidgetPlugin_H
