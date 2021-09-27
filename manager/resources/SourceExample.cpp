#include "MyWidgetPlugin.h"

#include <QtPlugin>
#include <QtUiTools/QUiLoader>
#include <QFile>
#include <QWidget>

MyWidgetPlugin::MyWidgetPlugin(QObject* parent)
: QObject(parent)
{
    m_initialized = false;
    path = "/*path to ui file*/";
}

void MyWidgetPlugin::initialize(QDesignerFormEditorInterface* /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. heremywidgetplugin

    m_initialized = true;
}

bool MyWidgetPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget* MyWidgetPlugin::createWidget(QWidget* parent)
{
    QUiLoader builder;
    QFile file(path);
    file.open(QFile::ReadOnly);
    myWidget = builder.load(&file);
    file.close();
    myWidget->setParent(parent);
    return myWidget;
}

QString MyWidgetPlugin::name() const
{
    return QLatin1String("MyWidget");
}

QString MyWidgetPlugin::group() const
{
    return QLatin1String("");
}

QIcon MyWidgetPlugin::icon() const
{
    return QIcon();
}

QString MyWidgetPlugin::toolTip() const
{
    return QLatin1String("");
}

QString MyWidgetPlugin::whatsThis() const
{
    return QLatin1String("asdasdasdasdas");
}

bool MyWidgetPlugin::isContainer() const
{
    return true;
}

QString MyWidgetPlugin::domXml() const
{
    return QLatin1String("<widget class=\"MyWidget\" name=\"myWidget\">\n</widget>\n");
}

QString MyWidgetPlugin::includeFile() const
{
    return QLatin1String("mywidget.h");
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(mywidgetplugin, MyWidgetPlugin)
#endif // QT_VERSION < 0x050000
