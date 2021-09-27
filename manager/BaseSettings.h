#pragma once

#include "nodeeditor/nodes/Serializable.hpp"

#include <QString>
#include <QJsonObject>
#include <QFont>
#include <QColor>

namespace dscontrol
{
class DSConfiguration;

struct ItemSettings
{
    int borderWidth;
    QColor color;
    QColor backColor;
    QColor borderColor;
    QFont font;
};

struct ArrowSettings
{
    int width;
    int direction;
    QColor color;
};

struct OptionsCodeExtraction
{
    QString vsPath;
    QString qtPath;
};

struct OptionsItems
{
    ItemSettings system;
    ItemSettings subsystem;
    ItemSettings computer;
    ItemSettings application;
    ItemSettings component;
};

struct OptionsArrows
{
    ArrowSettings line;
    ArrowSettings polyline;
    ArrowSettings curved;
    ArrowSettings connection;
    ArrowSettings lateralHorizontal;
    ArrowSettings lateralVertical;
    ArrowSettings treeHorizontal;
    ArrowSettings treeVertical;
};

struct OptionsRepository
{
    QString localPath;
    QString remotePath;
    QString filter;
};

struct OptionsScene
{
    int minZoom;
    int maxZoom;
    int zoomStep;
    int textHideZoom;
    QColor backColor;
};

class BaseSettings : public QtNodes::Serializable
{
    friend class DSConfiguration;

public:
    BaseSettings();
    ~BaseSettings() override = default;

    void saveSettings() const;
    void loadSettings();

    QJsonObject save() const override;
    void restore(const QJsonObject &settings) override;

protected:
    QString baseFolder_;
    QString componentsFolder_;
    QString bundlesFolder_;
    QString deployFolder_;
    QString sampleFolder_;
    QString descriptionFolder_;
    OptionsCodeExtraction codeExtraction_;
    OptionsItems items_;
    OptionsArrows arrows_;
    OptionsRepository repository_;
    OptionsScene scene_;

};
}