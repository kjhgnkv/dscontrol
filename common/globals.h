// globals.h

#pragma once

#include <QString>

// extension of project file without first point!
#define FILE_PROJECT_EXTENSION "dspl"

/// program info
#define PROGRAMM_NAME "Visual Development Deployment Enviroment"
#define PROGRAMM_VERSION "v0.1-alpha"
#define ORGANIZATION_NAME "CETC"
#define ORGANIZATION_DOMAIN "cetc.by"

#define DS_SETTINGS_EXISTS "Exists"
#define DS_SETTINGS_SECTION_EXTRACT "CodeExtraction"
#define DS_SETTINGS_QT_PATH "QtPath"
#define DS_SETTINGS_VS_PATH "VSPath"
#define DS_SETTINGS_SECTION_ITEMS "Items"
#define DS_SETTINGS_SECTION_SYSTEM "System"
#define DS_SETTINGS_SECTION_SUBSYSTEM "SubSystem"
#define DS_SETTINGS_SECTION_SYSTEM "System"
#define DS_SETTINGS_SECTION_COMPUTER "Computer"
#define DS_SETTINGS_SECTION_APPLICATION "Application"
#define DS_SETTINGS_SECTION_COMPONENT "Component"
#define DS_SETTINGS_SECTION_ARROWS "Arrows"
#define DS_SETTINGS_SECTION_LINE "Line"
#define DS_SETTINGS_SECTION_POLYLINE "Polyline"
#define DS_SETTINGS_SECTION_CURVED "Curved"
#define DS_SETTINGS_SECTION_CONNECTION "Connection"
#define DS_SETTINGS_SECTION_LATERALHOR "LateralHorizontal"
#define DS_SETTINGS_SECTION_LATERALVERT "LateralVertical"
#define DS_SETTINGS_SECTION_TREEHOR "TreeHorizontal"
#define DS_SETTINGS_SECTION_TREEVERT "TreeVertical"
#define DS_SETTINGS_WIDTH "Width"
#define DS_SETTINGS_BORDER_WIDTH "BorderWidth"
#define DS_SETTINGS_COLOR "Color"
#define DS_SETTINGS_BACK_COLOR "BackColor"
#define DS_SETTINGS_BORDER_COLOR "BorderColor"
#define DS_SETTINGS_FONT_FAMILY "FontFamily"
#define DS_SETTINGS_FONT_SIZE "FontSize"
#define DS_SETTINGS_FONT_WEIGHT "FontWeight"
#define DS_SETTINGS_FONT_ITALIC "FontItalic"
#define DS_SETTINGS_DIRECTION "Direction"
#define DS_SETTINGS_SECTION_REPOSITORY "Repository"
#define DS_SETTINGS_LOCAL_PATH "LocalPath"
#define DS_SETTINGS_REMOTE_PATH "RemotePath"
#define DS_SETTINGS_FILTER "Filter"
#define DS_SETTINGS_SECTION_SCENE "Scene"
#define DS_SETTINGS_MIN_ZOOM "MinZoom"
#define DS_SETTINGS_MAX_ZOOM "MaxZoom"
#define DS_SETTINGS_TEXT_HIDE_ZOOM "TextHideZoom"
#define DS_SETTINGS_ZOOM_STEP "ZoomStep"


/// resources
#define PLATFORM_ICON ":/icons/window/resources/icons/vide/48x48.ico"

#define DEFAULT_SAMPLE_PATH "./"

#define MAX_SCALE 500
#define MIN_SCALE 20
#define STEP 10

static const QString _DEF_STYLESHEET = {
R"(QDialog{
      background: palette(window);
    }
    QLabel{
       color: black;
    }
    QCheckBox{
       color: black;
    }
	QGroupBox{
       color: black;
	}
    QLineEdit{
       background: white;
       color: black;
    }
    QScrollBar:horizontal {
    background: palette(window);
}
QScrollBar::handle:horizontal {
    background: gray;
}
QScrollBar::add-line:horizontal {
    background: palette(window);
}

QScrollBar::sub-line:horizontal {
    background: palette(window);
}

QScrollBar::sub-page:horizontal, QScrollBar::add-page:horizontal  {
    background: palette(window);
}

QScrollBar::left-arrow:horizontal {
    image: url(:/toolbar/mode/left);
}

QScrollBar::right-arrow:horizontal {
    image: url(:/toolbar/mode/right);
}

QScrollBar:vertical {
    background: palette(window);
}
QScrollBar::handle:vertical {
    background: #AAAAC5;
}
QScrollBar::add-line:vertical {
    background: palette(window);
}

QScrollBar::sub-line:vertical {
    background: palette(window);
}

QScrollBar::sub-page:vertical, QScrollBar::add-page:vertical  {
    background: palette(window);
}

QComboBox
{
    color: black;
    background-color: palette(window);
}

QComboBox:on
{
    color: black;
    background-color: palette(window);
}

QComboBox:!on
{
    color: black;
    background-color: palette(window);
}

QComboBox QAbstractItemView
{
    color: black;
    background-color: white;
}

QComboBox::drop-down
{
    color: black;
}

)"
};
