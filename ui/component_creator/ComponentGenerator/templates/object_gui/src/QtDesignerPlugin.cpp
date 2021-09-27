#include "QtDesignerPlugin.h"
#include "QtWidget.h"

QString %name%QDesignerWidgetInterface::domXml() const
{
    // TODO: write appropriate XML description of the widget
    return "<ui language=\"c++\">\n"
        " <widget class=\"%name%Widget\" name=\"testWidget\">\n"
        "  <property name=\"geometry\">\n"
        "   <rect>\n"
        "    <x>0</x>\n"
        "    <y>0</y>\n"
        "    <width>100</width>\n"
        "    <height>100</height>\n"
        "   </rect>\n"
        "  </property>\n"
        "  <property name=\"toolTip\" >\n"
        "   <string>DSHost test widget</string>\n"
        "  </property>\n"
        "  <property name=\"whatsThis\" >\n"
        "   <string>The test widget helps to understand how to create graphical DSHost components.</string>\n"
        "  </property>\n"
        " </widget>\n"
        "</ui>\n";
}

QWidget* %name%QDesignerWidgetInterface::createWidget(QWidget *parent)
{
    return new %namespace-prefix%%name%Widget(parent);
}

