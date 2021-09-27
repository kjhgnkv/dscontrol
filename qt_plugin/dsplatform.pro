
#message($$QTC_BUILD)
DEFINES += DSPLATFORM_LIBRARY

QT += xml

# DSPlatform files
SOURCES += \
        dsplatformplugin.cpp

HEADERS += \
        include/dsplatformplugin.h \
include/dsplatform_global.h \
        include/dsplatformconstants.h

RESOURCES += resource/resources.qrc

# Qt Creator linking

## Either set the IDE_SOURCE_TREE when running qmake,
## or set the QTC_SOURCE environment variable, to override the default setting
isEmpty(IDE_SOURCE_TREE): IDE_SOURCE_TREE = $$QTC_SOURCE
#isEmpty(IDE_SOURCE_TREE): IDE_SOURCE_TREE = /home/soft/Desktop/4_12/
## Either set the IDE_BUILD_TREE when running qmake,
## or set the QTC_BUILD environment variable, to override the default setting
isEmpty(IDE_BUILD_TREE): IDE_BUILD_TREE = $$QTC_BUILD
#isEmpty(IDE_BUILD_TREE): IDE_BUILD_TREE = /home/soft/Desktop/4_12/

## uncomment to build plugin into user config directory
## <localappdata>/plugins/<ideversion>
##    where <localappdata> is e.g.
##    "%LOCALAPPDATA%\QtProject\qtcreator" on Windows Vista and later
##    "$XDG_DATA_HOME/data/QtProject/qtcreator" or "~/.local/share/data/QtProject/qtcreator" on Linux
##    "~/Library/Application Support/QtProject/Qt Creator" on OS X
USE_USER_DESTDIR = yes

###### If the plugin can be depended upon by other plugins, this code needs to be outsourced to
###### <dirname>_dependencies.pri, where <dirname> is the name of the directory containing the
###### plugin's sources.

QTC_PLUGIN_NAME = DSPlatform
QTC_LIB_DEPENDS += \

QTC_PLUGIN_DEPENDS += \
    coreplugin

QTC_PLUGIN_RECOMMENDS += \
    # optional plugin dependencies. nothing here at this time

###### End _dependencies.pri contents ######

include($$IDE_SOURCE_TREE/src/qtcreatorplugin.pri)

# TODO set here variable from cmake
LIBS += $$DS_LIB

INCLUDEPATH += $$PWD/../
DEPENDPATH += $$PWD/../

# path for ds_export
INCLUDEPATH += $$PWD/../build/

# TODO here code not portable fix this
PRE_TARGETDEPS += $$DS_LIB
