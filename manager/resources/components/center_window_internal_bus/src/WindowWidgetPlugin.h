#ifndef WindowWidgetPlugin_H
#define WindowWidgetPlugin_H

#include <QWidget>
#include "IWidgetPlugin.h"

/** widget plugin service */
class WindowWidgetPlugin : public IWidgetPlugin
{
Q_OBJECT

public:
    WindowWidgetPlugin();
    virtual ~WindowWidgetPlugin();

    // Plugin interfaces
    QList<ICenterWindow*> getCenterWindow();/**< get center window list */
    QList<ISettingWindow*> getSettingWindow();/**< get setting window list, obsolete */
    QList<IPluginInfo*> getPluginInfoWindow();/**< get plugin info window list, obsolete */

private:
    QList<ICenterWindow*> m_centerWindowList;/**< center window list */
    QList<ISettingWindow*> m_settingWindowList;
    QList<IPluginInfo*> m_pluginInfoList;

    void createWidget();/**< create center windows */
};

#endif // WindowWidgetPlugin_H
