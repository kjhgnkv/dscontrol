QT += designer uitools

CONFIG      += plugin debug_and_release
TARGET      = $$qtLibraryTarget(MyWidgetPlugin)
TEMPLATE    = lib

HEADERS     = MyWidgetPlugin.h
SOURCES     = MyWidgetPlugin.cpp
RESOURCES   = icons.qrc
LIBS        += -L. 

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += designer
} else {
    CONFIG += designer
}

target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS    += target
