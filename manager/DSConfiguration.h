#pragma once

#include <QString>
#include "BaseSettings.h"
#include "nodeeditor/common/nodeeditor_globals.hpp"

namespace dscontrol
{

class DSConfiguration final
{
public:
    static DSConfiguration &Instance();
    ~DSConfiguration();

    DSConfiguration(DSConfiguration const &) = delete;
    DSConfiguration &operator=(DSConfiguration const &) = delete;

    DSConfiguration(DSConfiguration &&) = delete;
    DSConfiguration &operator=(DSConfiguration &&) = delete;

    void init();
    void save();
    QString baseFolder();
    QString componentsFolder();
    QString bundlesFolder();
    QString deployFolder();
    QString sampleFolder();
    QString descriptionFolder();

    void setVSPath(const QString &path);
    QString vsPath();
    void setQtPath(const QString &path);
    QString qtPath();

    int itemBorderWidth(QtNodes::Item::NodeType type);
    void setItemBorderWidth(QtNodes::Item::NodeType type, int);
    QColor itemColor(QtNodes::Item::NodeType type);
    void setItemColor(QtNodes::Item::NodeType type, const QColor &color);
    QColor itemBackgroundColor(QtNodes::Item::NodeType type);
    void setItemBackgroundColor(QtNodes::Item::NodeType type, const QColor &color);
    QColor itemBorderColor(QtNodes::Item::NodeType type);
    void setItemBorderColor(QtNodes::Item::NodeType type, const QColor &color);
    QFont itemFont(QtNodes::Item::NodeType type);
    void setItemFont(QtNodes::Item::NodeType type, const QFont &color);

    int arrowWidth(QtNodes::Arrows::ArrowType type);
    void setArrowWidth(QtNodes::Arrows::ArrowType type, int width);
    int arrowDirection(QtNodes::Arrows::ArrowType type);
    void setArrowDirection(QtNodes::Arrows::ArrowType type, int direction);
    QColor arrowColor(QtNodes::Arrows::ArrowType type);
    void setArrowColor(QtNodes::Arrows::ArrowType type, const QColor &color);

    QString repositoryLocalPath();
    void setRepositoryLocalPath(const QString &path);
    QString repositoryRemotePath();
    void setRepositoryRemotePath(const QString &path);
    QString repositoryFilter();
    void setRepositoryFilter(const QString &filter);

    int sceneMinZoom();
    void setSceneMinZoom(int zoom);
    int sceneMaxZoom();
    void setSceneMaxZoom(int zoom);
    int sceneZoomStep();
    void setSceneZoomStep(int step);
    int sceneTextHideZoom();
    void setSceneTextHideZoom(int zoom);
    QColor sceneBackColor();
    void setSceneBackColor(const QColor &color);

private:
    DSConfiguration();
    BaseSettings* settings_;
};
}
