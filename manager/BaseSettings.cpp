#include "BaseSettings.h"

#include "../common/globals.h"

#include <QDir>
#include <QSettings>

using namespace dscontrol;

BaseSettings::BaseSettings()
: baseFolder_ {QDir::currentPath()}
, componentsFolder_ {}
, bundlesFolder_ {}
, descriptionFolder_{}
{
    // set up default settings
}

// TODO too much duplicate code( think about how to improve
void BaseSettings::saveSettings() const
{
    QSettings settings;

    settings.setValue(DS_SETTINGS_EXISTS, true);
    settings.beginGroup(DS_SETTINGS_SECTION_EXTRACT);
    settings.setValue(DS_SETTINGS_QT_PATH, codeExtraction_.qtPath);
    settings.setValue(DS_SETTINGS_VS_PATH, codeExtraction_.vsPath);
    settings.endGroup();

    //items
    settings.beginGroup(DS_SETTINGS_SECTION_ITEMS);
    //system
    settings.beginGroup(DS_SETTINGS_SECTION_SYSTEM);
    settings.setValue(DS_SETTINGS_BORDER_WIDTH, items_.system.borderWidth);
    settings.setValue(DS_SETTINGS_COLOR, items_.system.color.name());
    settings.setValue(DS_SETTINGS_BACK_COLOR, items_.system.backColor.name());
    settings.setValue(DS_SETTINGS_BORDER_COLOR, items_.system.borderColor.name());
    settings.setValue(DS_SETTINGS_FONT_FAMILY, items_.system.font.family());
    settings.setValue(DS_SETTINGS_FONT_SIZE, items_.system.font.pointSize());
    settings.setValue(DS_SETTINGS_FONT_WEIGHT, items_.system.font.weight());
    settings.setValue(DS_SETTINGS_FONT_ITALIC, items_.system.font.italic());
    settings.endGroup();
    //subsystem
    settings.beginGroup(DS_SETTINGS_SECTION_SUBSYSTEM);
    settings.setValue(DS_SETTINGS_BORDER_WIDTH, items_.subsystem.borderWidth);
    settings.setValue(DS_SETTINGS_COLOR, items_.subsystem.color.name());
    settings.setValue(DS_SETTINGS_BACK_COLOR, items_.subsystem.backColor.name());
    settings.setValue(DS_SETTINGS_BORDER_COLOR, items_.subsystem.borderColor.name());
    settings.setValue(DS_SETTINGS_FONT_FAMILY, items_.subsystem.font.family());
    settings.setValue(DS_SETTINGS_FONT_SIZE, items_.subsystem.font.pointSize());
    settings.setValue(DS_SETTINGS_FONT_WEIGHT, items_.subsystem.font.weight());
    settings.setValue(DS_SETTINGS_FONT_ITALIC, items_.subsystem.font.italic());
    settings.endGroup();
    //computer
    settings.beginGroup(DS_SETTINGS_SECTION_COMPUTER);
    settings.setValue(DS_SETTINGS_BORDER_WIDTH, items_.computer.borderWidth);
    settings.setValue(DS_SETTINGS_COLOR, items_.computer.color.name());
    settings.setValue(DS_SETTINGS_BACK_COLOR, items_.computer.backColor.name());
    settings.setValue(DS_SETTINGS_BORDER_COLOR, items_.computer.borderColor.name());
    settings.setValue(DS_SETTINGS_FONT_FAMILY, items_.computer.font.family());
    settings.setValue(DS_SETTINGS_FONT_SIZE, items_.computer.font.pointSize());
    settings.setValue(DS_SETTINGS_FONT_WEIGHT, items_.computer.font.weight());
    settings.setValue(DS_SETTINGS_FONT_ITALIC, items_.computer.font.italic());
    settings.endGroup();
    //application
    settings.beginGroup(DS_SETTINGS_SECTION_APPLICATION);
    settings.setValue(DS_SETTINGS_BORDER_WIDTH, items_.application.borderWidth);
    settings.setValue(DS_SETTINGS_COLOR, items_.application.color.name());
    settings.setValue(DS_SETTINGS_BACK_COLOR, items_.application.backColor.name());
    settings.setValue(DS_SETTINGS_BORDER_COLOR, items_.application.borderColor.name());
    settings.setValue(DS_SETTINGS_FONT_FAMILY, items_.application.font.family());
    settings.setValue(DS_SETTINGS_FONT_SIZE, items_.application.font.pointSize());
    settings.setValue(DS_SETTINGS_FONT_WEIGHT, items_.application.font.weight());
    settings.setValue(DS_SETTINGS_FONT_ITALIC, items_.application.font.italic());
    settings.endGroup();
    //component
    settings.beginGroup(DS_SETTINGS_SECTION_COMPONENT);
    settings.setValue(DS_SETTINGS_BORDER_WIDTH, items_.component.borderWidth);
    settings.setValue(DS_SETTINGS_COLOR, items_.component.color.name());
    settings.setValue(DS_SETTINGS_BACK_COLOR, items_.component.backColor.name());
    settings.setValue(DS_SETTINGS_BORDER_COLOR, items_.component.borderColor.name());
    settings.setValue(DS_SETTINGS_FONT_FAMILY, items_.component.font.family());
    settings.setValue(DS_SETTINGS_FONT_SIZE, items_.component.font.pointSize());
    settings.setValue(DS_SETTINGS_FONT_WEIGHT, items_.component.font.weight());
    settings.setValue(DS_SETTINGS_FONT_ITALIC, items_.component.font.italic());
    settings.endGroup();
    settings.endGroup();//items

    //arrows
    settings.beginGroup(DS_SETTINGS_SECTION_ARROWS);
    //line
    settings.beginGroup(DS_SETTINGS_SECTION_LINE);
    settings.setValue(DS_SETTINGS_WIDTH, arrows_.line.width);
    settings.setValue(DS_SETTINGS_COLOR, arrows_.line.color.name());
    settings.setValue(DS_SETTINGS_DIRECTION, arrows_.line.direction);
    settings.endGroup();
    //polyline
    settings.beginGroup(DS_SETTINGS_SECTION_POLYLINE);
    settings.setValue(DS_SETTINGS_WIDTH, arrows_.polyline.width);
    settings.setValue(DS_SETTINGS_COLOR, arrows_.polyline.color.name());
    settings.setValue(DS_SETTINGS_DIRECTION, arrows_.polyline.direction);
    settings.endGroup();
    //curved
    settings.beginGroup(DS_SETTINGS_SECTION_CURVED);
    settings.setValue(DS_SETTINGS_WIDTH, arrows_.curved.width);
    settings.setValue(DS_SETTINGS_COLOR, arrows_.curved.color.name());
    settings.setValue(DS_SETTINGS_DIRECTION, arrows_.curved.direction);
    settings.endGroup();
    //connection
    settings.beginGroup(DS_SETTINGS_SECTION_CONNECTION);
    settings.setValue(DS_SETTINGS_WIDTH, arrows_.connection.width);
    settings.setValue(DS_SETTINGS_COLOR, arrows_.connection.color.name());
    settings.setValue(DS_SETTINGS_DIRECTION, arrows_.connection.direction);
    settings.endGroup();
    //lateral horizontal
    settings.beginGroup(DS_SETTINGS_SECTION_LATERALHOR);
    settings.setValue(DS_SETTINGS_WIDTH, arrows_.lateralHorizontal.width);
    settings.setValue(DS_SETTINGS_COLOR, arrows_.lateralHorizontal.color.name());
    settings.setValue(DS_SETTINGS_DIRECTION, arrows_.lateralHorizontal.direction);
    settings.endGroup();
    //lateral vertical
    settings.beginGroup(DS_SETTINGS_SECTION_LATERALVERT);
    settings.setValue(DS_SETTINGS_WIDTH, arrows_.lateralVertical.width);
    settings.setValue(DS_SETTINGS_COLOR, arrows_.lateralVertical.color.name());
    settings.setValue(DS_SETTINGS_DIRECTION, arrows_.lateralVertical.direction);
    settings.endGroup();
    //tree horizontal
    settings.beginGroup(DS_SETTINGS_SECTION_TREEHOR);
    settings.setValue(DS_SETTINGS_WIDTH, arrows_.treeHorizontal.width);
    settings.setValue(DS_SETTINGS_COLOR, arrows_.treeHorizontal.color.name());
    settings.setValue(DS_SETTINGS_DIRECTION, arrows_.treeHorizontal.direction);
    settings.endGroup();
    //tree vertical
    settings.beginGroup(DS_SETTINGS_SECTION_TREEHOR);
    settings.setValue(DS_SETTINGS_WIDTH, arrows_.treeVertical.width);
    settings.setValue(DS_SETTINGS_COLOR, arrows_.treeVertical.color.name());
    settings.setValue(DS_SETTINGS_DIRECTION, arrows_.treeVertical.direction);
    settings.endGroup();
    settings.endGroup();//arrows
    //repository
    settings.beginGroup(DS_SETTINGS_SECTION_REPOSITORY);
    settings.setValue(DS_SETTINGS_LOCAL_PATH, repository_.localPath);
    settings.setValue(DS_SETTINGS_REMOTE_PATH, repository_.remotePath);
    settings.setValue(DS_SETTINGS_FILTER, repository_.filter);
    settings.endGroup();
    //scene
    settings.beginGroup(DS_SETTINGS_SECTION_SCENE);
    settings.setValue(DS_SETTINGS_MIN_ZOOM, scene_.minZoom);
    settings.setValue(DS_SETTINGS_MAX_ZOOM, scene_.maxZoom);
    settings.setValue(DS_SETTINGS_TEXT_HIDE_ZOOM, scene_.textHideZoom);
    settings.setValue(DS_SETTINGS_ZOOM_STEP, scene_.zoomStep);
    settings.setValue(DS_SETTINGS_BACK_COLOR, scene_.backColor.name());
    settings.endGroup();//scene

}

void BaseSettings::loadSettings()
{
    QSettings settings;

    settings.beginGroup(DS_SETTINGS_SECTION_EXTRACT);
    codeExtraction_.qtPath = settings.value(DS_SETTINGS_QT_PATH).toString();
    codeExtraction_.vsPath = settings.value(DS_SETTINGS_VS_PATH).toString();
    settings.endGroup();

    //items
    settings.beginGroup(DS_SETTINGS_SECTION_ITEMS);
    //system
    settings.beginGroup(DS_SETTINGS_SECTION_SYSTEM);
    items_.system.borderWidth = settings.value(DS_SETTINGS_BORDER_WIDTH).toInt();
    items_.system.color.setNamedColor(settings.value(DS_SETTINGS_COLOR).toString());
    items_.system.backColor.setNamedColor(settings.value(DS_SETTINGS_BACK_COLOR).toString());
    items_.system.borderColor.setNamedColor(settings.value(DS_SETTINGS_BORDER_COLOR).toString());
    items_.system.font.setFamily(settings.value(DS_SETTINGS_FONT_FAMILY).toString());
    items_.system.font.setPointSize(settings.value(DS_SETTINGS_FONT_SIZE).toInt());
    items_.system.font.setWeight(settings.value(DS_SETTINGS_FONT_WEIGHT).toInt());
    items_.system.font.setItalic(settings.value(DS_SETTINGS_FONT_ITALIC).toBool());
    settings.endGroup();
    //subsystem
    settings.beginGroup(DS_SETTINGS_SECTION_SUBSYSTEM);
    items_.subsystem.borderWidth = settings.value(DS_SETTINGS_BORDER_WIDTH).toInt();
    items_.subsystem.color.setNamedColor(settings.value(DS_SETTINGS_COLOR).toString());
    items_.subsystem.backColor.setNamedColor(settings.value(DS_SETTINGS_BACK_COLOR).toString());
    items_.subsystem.borderColor.setNamedColor(settings.value(DS_SETTINGS_BORDER_COLOR).toString());
    items_.subsystem.font.setFamily(settings.value(DS_SETTINGS_FONT_FAMILY).toString());
    items_.subsystem.font.setPointSize(settings.value(DS_SETTINGS_FONT_SIZE).toInt());
    items_.subsystem.font.setWeight(settings.value(DS_SETTINGS_FONT_WEIGHT).toInt());
    items_.subsystem.font.setItalic(settings.value(DS_SETTINGS_FONT_ITALIC).toBool());
    settings.endGroup();
    //computer
    settings.beginGroup(DS_SETTINGS_SECTION_COMPUTER);
    items_.computer.borderWidth = settings.value(DS_SETTINGS_BORDER_WIDTH).toInt();
    items_.computer.color.setNamedColor(settings.value(DS_SETTINGS_COLOR).toString());
    items_.computer.backColor.setNamedColor(settings.value(DS_SETTINGS_BACK_COLOR).toString());
    items_.computer.borderColor.setNamedColor(settings.value(DS_SETTINGS_BORDER_COLOR).toString());
    items_.computer.font.setFamily(settings.value(DS_SETTINGS_FONT_FAMILY).toString());
    items_.computer.font.setPointSize(settings.value(DS_SETTINGS_FONT_SIZE).toInt());
    items_.computer.font.setWeight(settings.value(DS_SETTINGS_FONT_WEIGHT).toInt());
    items_.computer.font.setItalic(settings.value(DS_SETTINGS_FONT_ITALIC).toBool());
    settings.endGroup();
    //application
    settings.beginGroup(DS_SETTINGS_SECTION_APPLICATION);
    items_.application.borderWidth = settings.value(DS_SETTINGS_BORDER_WIDTH).toInt();
    items_.application.color.setNamedColor(settings.value(DS_SETTINGS_COLOR).toString());
    items_.application.backColor.setNamedColor(settings.value(DS_SETTINGS_BACK_COLOR).toString());
    items_.application.borderColor.setNamedColor(settings.value(DS_SETTINGS_BORDER_COLOR).toString());
    items_.application.font.setFamily(settings.value(DS_SETTINGS_FONT_FAMILY).toString());
    items_.application.font.setPointSize(settings.value(DS_SETTINGS_FONT_SIZE).toInt());
    items_.application.font.setWeight(settings.value(DS_SETTINGS_FONT_WEIGHT).toInt());
    items_.application.font.setItalic(settings.value(DS_SETTINGS_FONT_ITALIC).toBool());
    settings.endGroup();
    //component
    settings.beginGroup(DS_SETTINGS_SECTION_COMPONENT);
    items_.component.borderWidth = settings.value(DS_SETTINGS_BORDER_WIDTH).toInt();
    items_.component.color.setNamedColor(settings.value(DS_SETTINGS_COLOR).toString());
    items_.component.backColor.setNamedColor(settings.value(DS_SETTINGS_BACK_COLOR).toString());
    items_.component.borderColor.setNamedColor(settings.value(DS_SETTINGS_BORDER_COLOR).toString());
    items_.component.font.setFamily(settings.value(DS_SETTINGS_FONT_FAMILY).toString());
    items_.component.font.setPointSize(settings.value(DS_SETTINGS_FONT_SIZE).toInt());
    items_.component.font.setWeight(settings.value(DS_SETTINGS_FONT_WEIGHT).toInt());
    items_.component.font.setItalic(settings.value(DS_SETTINGS_FONT_ITALIC).toBool());
    settings.endGroup();
    settings.endGroup();//items

    //arrows
    settings.beginGroup(DS_SETTINGS_SECTION_ARROWS);
    //line
    settings.beginGroup(DS_SETTINGS_SECTION_LINE);
    arrows_.line.width = settings.value(DS_SETTINGS_WIDTH).toInt();
    arrows_.line.color.setNamedColor(settings.value(DS_SETTINGS_COLOR).toString());
    arrows_.line.direction = settings.value(DS_SETTINGS_DIRECTION).toInt();
    settings.endGroup();
    //polyline
    settings.beginGroup(DS_SETTINGS_SECTION_POLYLINE);
    arrows_.polyline.width = settings.value(DS_SETTINGS_WIDTH).toInt();
    arrows_.polyline.color.setNamedColor(settings.value(DS_SETTINGS_COLOR).toString());
    arrows_.polyline.direction = settings.value(DS_SETTINGS_DIRECTION).toInt();
    settings.endGroup();
    //curved
    settings.beginGroup(DS_SETTINGS_SECTION_CURVED);
    arrows_.curved.width = settings.value(DS_SETTINGS_WIDTH).toInt();
    arrows_.curved.color.setNamedColor(settings.value(DS_SETTINGS_COLOR).toString());
    arrows_.curved.direction = settings.value(DS_SETTINGS_DIRECTION).toInt();
    settings.endGroup();
    //connection
    settings.beginGroup(DS_SETTINGS_SECTION_CONNECTION);
    arrows_.connection.width = settings.value(DS_SETTINGS_WIDTH).toInt();
    arrows_.connection.color.setNamedColor(settings.value(DS_SETTINGS_COLOR).toString());
    arrows_.connection.direction = settings.value(DS_SETTINGS_DIRECTION).toInt();
    settings.endGroup();
    //lateral horizontal
    settings.beginGroup(DS_SETTINGS_SECTION_LATERALHOR);
    arrows_.lateralHorizontal.width = settings.value(DS_SETTINGS_WIDTH).toInt();
    arrows_.lateralHorizontal.color.setNamedColor(settings.value(DS_SETTINGS_COLOR).toString());
    arrows_.lateralHorizontal.direction = settings.value(DS_SETTINGS_DIRECTION).toInt();
    settings.endGroup();
    //lateral vertical
    settings.beginGroup(DS_SETTINGS_SECTION_LATERALVERT);
    arrows_.lateralVertical.width = settings.value(DS_SETTINGS_WIDTH).toInt();
    arrows_.lateralVertical.color.setNamedColor(settings.value(DS_SETTINGS_COLOR).toString());
    arrows_.lateralVertical.direction = settings.value(DS_SETTINGS_DIRECTION).toInt();
    settings.endGroup();
    //tree horizontal
    settings.beginGroup(DS_SETTINGS_SECTION_TREEHOR);
    arrows_.treeHorizontal.width = settings.value(DS_SETTINGS_WIDTH).toInt();
    arrows_.treeHorizontal.color.setNamedColor(settings.value(DS_SETTINGS_COLOR).toString());
    arrows_.treeHorizontal.direction = settings.value(DS_SETTINGS_DIRECTION).toInt();
    settings.endGroup();
    //tree vertical
    settings.beginGroup(DS_SETTINGS_SECTION_TREEVERT);
    arrows_.treeVertical.width = settings.value(DS_SETTINGS_WIDTH).toInt();
    arrows_.treeVertical.color.setNamedColor(settings.value(DS_SETTINGS_COLOR).toString());
    arrows_.treeVertical.direction = settings.value(DS_SETTINGS_DIRECTION).toInt();
    settings.endGroup();
    settings.endGroup();//arrows
    //repository
    settings.beginGroup(DS_SETTINGS_SECTION_REPOSITORY);
    repository_.localPath = settings.value(DS_SETTINGS_LOCAL_PATH).toString();
    repository_.remotePath = settings.value(DS_SETTINGS_REMOTE_PATH).toString();
    repository_.filter = settings.value(DS_SETTINGS_FILTER).toString();
    settings.endGroup();//repository
    //scene
    settings.beginGroup(DS_SETTINGS_SECTION_SCENE);
    scene_.minZoom = settings.value(DS_SETTINGS_MIN_ZOOM).toInt();
    scene_.maxZoom = settings.value(DS_SETTINGS_MAX_ZOOM).toInt();
    scene_.zoomStep = settings.value(DS_SETTINGS_ZOOM_STEP).toInt();
    scene_.textHideZoom = settings.value(DS_SETTINGS_TEXT_HIDE_ZOOM).toInt();
    scene_.backColor.setNamedColor(settings.value(DS_SETTINGS_BACK_COLOR).toString());
    settings.endGroup();//scene
}

QJsonObject BaseSettings::save() const
{
    QJsonObject settings;
    QJsonObject option;
    option["vsPath"] = codeExtraction_.vsPath;
    option["qtPath"] = codeExtraction_.qtPath;
    settings["codeGen"] = option;
    return settings;
}

void BaseSettings::restore(const QJsonObject &settings)
{
    codeExtraction_.vsPath = settings["vsPath"].toString();
    codeExtraction_.qtPath = settings["qtPath"].toString();
}
