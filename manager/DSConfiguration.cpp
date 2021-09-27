#include "manager/DSConfiguration.h"
#include "manager/WindowsSettings.h"
#include "manager/LinuxSettings.h"
#include "ui/platform/DSMessageBox/DSMessageBox.hpp"
#include "common/globals.h"

#include <QDir>
#include <QJsonDocument>
#include <QSettings>

using namespace dscontrol;

DSConfiguration &DSConfiguration::Instance()
{
    static DSConfiguration instance;
    return instance;
}

DSConfiguration::DSConfiguration()
: settings_ {nullptr}
{
#ifdef WIN32
    settings_ = new WindowsSettings();
#else
#ifdef __linux
    settings_ = new LinuxSettings();
#else
    settings_ = new BaseSettings();
#endif

#endif
}

DSConfiguration::~DSConfiguration()
{
    delete settings_;
}

void DSConfiguration::init()
{
    auto baseFolder = settings_->baseFolder_;
    qDebug() << "baseFolder" << baseFolder;
    auto componentsFolder = baseFolder + "/components";
    qDebug() << "componentsFolder" << componentsFolder;
    auto bundlesFolder = baseFolder + "/bndl";
    qDebug() << "bundlesFolder" << bundlesFolder;
    auto deployFolder = baseFolder + "/deploy";
    qDebug() << "bundlesFolder" << bundlesFolder;
    auto sampleFolder = baseFolder + "/sample";
    qDebug() << "bundlesFolder" << bundlesFolder;
    auto descriptionFolder = baseFolder + "/description";
    qDebug() << "bundlesFolder" << bundlesFolder;

    QDir base {baseFolder};
    if (!base.exists("components"))
    {
        //makes also base and components directories
        if (base.mkpath(componentsFolder))
        {
            //TODO: copy components? from where?
        }
    }
    if (!base.exists("bndl"))
    {
        //makes also base and bundles directories
        if (base.mkpath(bundlesFolder))
        {
            //TODO: copy components? from where?
        }
    }
    if (!base.exists("deploy"))
    {
        //makes also base and deploy directories
        if (base.mkpath(deployFolder))
        {
        }
    }
    if (!base.exists("sample"))
    {
        //makes also base and sample directories
        if (base.mkpath(sampleFolder))
        {
            //TODO: copy sample? from where?
        }
    }
    if (!base.exists("description"))
    {
        //makes also base and description directories
        if (base.mkpath(descriptionFolder))
        {
            //TODO: copy descriptions? from where?
        }
    }
    settings_->componentsFolder_ = componentsFolder;
    settings_->bundlesFolder_ = bundlesFolder;
    settings_->deployFolder_ = deployFolder;
    settings_->sampleFolder_ = sampleFolder;
    settings_->descriptionFolder_ = descriptionFolder;

    QSettings settings;

    if (!settings.value(DS_SETTINGS_EXISTS).isNull())
    {
        settings_->loadSettings();
        return;
    }

    // saving default settings
    settings_->saveSettings();
}

void DSConfiguration::save()
{
    settings_->saveSettings();
}

QString DSConfiguration::baseFolder()
{
    return settings_->baseFolder_;
}

QString DSConfiguration::componentsFolder()
{
    return settings_->componentsFolder_;
}

QString DSConfiguration::bundlesFolder()
{
    return settings_->bundlesFolder_;
}

QString DSConfiguration::deployFolder()
{
    return settings_->deployFolder_;
}

QString DSConfiguration::sampleFolder()
{
    return settings_->sampleFolder_;
}

QString DSConfiguration::descriptionFolder()
{
    return settings_->descriptionFolder_;
}

QString DSConfiguration::vsPath()
{
    return settings_->codeExtraction_.vsPath;
}

void DSConfiguration::setVSPath(const QString &path)
{
    settings_->codeExtraction_.vsPath = path;
}

QString DSConfiguration::qtPath()
{
    return settings_->codeExtraction_.qtPath;
}

void DSConfiguration::setQtPath(const QString &path)
{
    settings_->codeExtraction_.qtPath = path;
}

using QtNodes::Item::NodeType;

int DSConfiguration::itemBorderWidth(QtNodes::Item::NodeType type)
{
    switch (type)
    {
        case NodeType::SystemNode:
        {
            return settings_->items_.system.borderWidth;
        }
        case NodeType::SubSystemNode:
        {
            return settings_->items_.subsystem.borderWidth;
        }
        case NodeType::ComputerNode:
        {
            return settings_->items_.computer.borderWidth;
        }
        case NodeType::ApplicationNode:
        {
            return settings_->items_.application.borderWidth;
        }
        case NodeType::ComponentNode:
        {
            return settings_->items_.component.borderWidth;
        }
        default:
        {
            return -1;
        }
    }
}

void DSConfiguration::setItemBorderWidth(QtNodes::Item::NodeType type, int width)
{
    switch (type)
    {
        case NodeType::SystemNode:
        {
            settings_->items_.system.borderWidth = width;
            break;
        }
        case NodeType::SubSystemNode:
        {
            settings_->items_.subsystem.borderWidth = width;
            break;
        }
        case NodeType::ComputerNode:
        {
            settings_->items_.computer.borderWidth = width;
            break;
        }
        case NodeType::ApplicationNode:
        {
            settings_->items_.application.borderWidth = width;
            break;
        }
        case NodeType::ComponentNode:
        {
            settings_->items_.component.borderWidth = width;
            break;
        }
        default:
        {
            return;
        }
    }
}

QColor DSConfiguration::itemColor(QtNodes::Item::NodeType type)
{
    switch (type)
    {
        case NodeType::SystemNode:
        {
            return settings_->items_.system.color;
        }
        case NodeType::SubSystemNode:
        {
            return settings_->items_.subsystem.color;
        }
        case NodeType::ComputerNode:
        {
            return settings_->items_.computer.color;
        }
        case NodeType::ApplicationNode:
        {
            return settings_->items_.application.color;
        }
        case NodeType::ComponentNode:
        {
            return settings_->items_.component.color;
        }
        default:
        {
            return {};
        }
    }
}

void DSConfiguration::setItemColor(QtNodes::Item::NodeType type, const QColor &color)
{
    switch (type)
    {
        case NodeType::SystemNode:
        {
            settings_->items_.system.color = color;
            break;
        }
        case NodeType::SubSystemNode:
        {
            settings_->items_.subsystem.color = color;
            break;
        }
        case NodeType::ComputerNode:
        {
            settings_->items_.computer.color = color;
            break;
        }
        case NodeType::ApplicationNode:
        {
            settings_->items_.application.color = color;
            break;
        }
        case NodeType::ComponentNode:
        {
            settings_->items_.component.color = color;
            break;
        }
        default:
        {
            return;
        }
    }

}

QColor DSConfiguration::itemBackgroundColor(QtNodes::Item::NodeType type)
{
    switch (type)
    {
        case NodeType::SystemNode:
        {
            return settings_->items_.system.backColor;
        }
        case NodeType::SubSystemNode:
        {
            return settings_->items_.subsystem.backColor;
        }
        case NodeType::ComputerNode:
        {
            return settings_->items_.computer.backColor;
        }
        case NodeType::ApplicationNode:
        {
            return settings_->items_.application.backColor;
        }
        case NodeType::ComponentNode:
        {
            return settings_->items_.component.backColor;
        }
        default:
        {
            return {};
        }
    }
}

void DSConfiguration::setItemBackgroundColor(QtNodes::Item::NodeType type, const QColor &color)
{
    switch (type)
    {
        case NodeType::SystemNode:
        {
            settings_->items_.system.backColor = color;
            break;
        }
        case NodeType::SubSystemNode:
        {
            settings_->items_.subsystem.backColor = color;
            break;
        }
        case NodeType::ComputerNode:
        {
            settings_->items_.computer.backColor = color;
            break;
        }
        case NodeType::ApplicationNode:
        {
            settings_->items_.application.backColor = color;
            break;
        }
        case NodeType::ComponentNode:
        {
            settings_->items_.component.backColor = color;
            break;
        }
        default:
        {
            return;
        }
    }
}

QColor DSConfiguration::itemBorderColor(QtNodes::Item::NodeType type)
{
    switch (type)
    {
        case NodeType::SystemNode:
        {
            return settings_->items_.system.borderColor;
        }
        case NodeType::SubSystemNode:
        {
            return settings_->items_.subsystem.borderColor;
        }
        case NodeType::ComputerNode:
        {
            return settings_->items_.computer.borderColor;
        }
        case NodeType::ApplicationNode:
        {
            return settings_->items_.application.borderColor;
        }
        case NodeType::ComponentNode:
        {
            return settings_->items_.component.borderColor;
        }
        default:
        {
            return {};
        }
    }
}

void DSConfiguration::setItemBorderColor(QtNodes::Item::NodeType type, const QColor &color)
{
    switch (type)
    {
        case NodeType::SystemNode:
        {
            settings_->items_.system.borderColor = color;
            break;
        }
        case NodeType::SubSystemNode:
        {
            settings_->items_.subsystem.borderColor = color;
            break;
        }
        case NodeType::ComputerNode:
        {
            settings_->items_.computer.borderColor = color;
            break;
        }
        case NodeType::ApplicationNode:
        {
            settings_->items_.application.borderColor = color;
            break;
        }
        case NodeType::ComponentNode:
        {
            settings_->items_.component.borderColor = color;
            break;
        }
        default:
        {
            return;
        }
    }
}

QFont DSConfiguration::itemFont(QtNodes::Item::NodeType type)
{
    switch (type)
    {
        case NodeType::SystemNode:
        {
            return settings_->items_.system.font;
        }
        case NodeType::SubSystemNode:
        {
            return settings_->items_.subsystem.font;
        }
        case NodeType::ComputerNode:
        {
            return settings_->items_.computer.font;
        }
        case NodeType::ApplicationNode:
        {
            return settings_->items_.application.font;
        }
        case NodeType::ComponentNode:
        {
            return settings_->items_.component.font;
        }
        default:
        {
            return {};
        }
    }
}

void DSConfiguration::setItemFont(QtNodes::Item::NodeType type, const QFont &font)
{
    switch (type)
    {
        case NodeType::SystemNode:
        {
            settings_->items_.system.font = font;
            break;
        }
        case NodeType::SubSystemNode:
        {
            settings_->items_.subsystem.font = font;
            break;
        }
        case NodeType::ComputerNode:
        {
            settings_->items_.computer.font = font;
            break;
        }
        case NodeType::ApplicationNode:
        {
            settings_->items_.application.font = font;
            break;
        }
        case NodeType::ComponentNode:
        {
            settings_->items_.component.font = font;
            break;
        }
        default:
        {
            return;
        }
    }
}

using QtNodes::Arrows::ArrowType;

int DSConfiguration::arrowWidth(QtNodes::Arrows::ArrowType type)
{
    switch (type)
    {
        case ArrowType::Line:
        {
            return settings_->arrows_.line.width;
        }
        case ArrowType::PolyLine:
        {
            return settings_->arrows_.polyline.width;
        }
        case ArrowType::Curved:
        {
            return settings_->arrows_.curved.width;
        }
        case ArrowType::Connection:
        {
            return settings_->arrows_.connection.width;
        }
        case ArrowType::LateralHorizontal:
        {
            return settings_->arrows_.lateralHorizontal.width;
        }
        case ArrowType::LateralVertical:
        {
            return settings_->arrows_.lateralVertical.width;
        }
        case ArrowType::TreeStyleHorizontal:
        {
            return settings_->arrows_.treeHorizontal.width;
        }
        case ArrowType::TreeStyleVertical:
        {
            return settings_->arrows_.treeVertical.width;
        }
        default:
        {
            return -1;
        }
    }
}

void DSConfiguration::setArrowWidth(QtNodes::Arrows::ArrowType type, int width)
{
    switch (type)
    {
        case ArrowType::Line:
        {
            settings_->arrows_.line.width = width;
            break;
        }
        case ArrowType::PolyLine:
        {
            settings_->arrows_.polyline.width = width;
            break;
        }
        case ArrowType::Curved:
        {
            settings_->arrows_.curved.width = width;
            break;
        }
        case ArrowType::Connection:
        {
            settings_->arrows_.connection.width = width;
            break;
        }
        case ArrowType::LateralHorizontal:
        {
            settings_->arrows_.lateralHorizontal.width = width;
            break;
        }
        case ArrowType::LateralVertical:
        {
            settings_->arrows_.lateralVertical.width = width;
            break;
        }
        case ArrowType::TreeStyleHorizontal:
        {
            settings_->arrows_.treeHorizontal.width = width;
            break;
        }
        case ArrowType::TreeStyleVertical:
        {
            settings_->arrows_.treeVertical.width = width;
            break;
        }
        default:
        {
            return;
        }
    }
}

int DSConfiguration::arrowDirection(QtNodes::Arrows::ArrowType type)
{
    switch (type)
    {
        case ArrowType::Line:
        {
            return settings_->arrows_.line.direction;
        }
        case ArrowType::PolyLine:
        {
            return settings_->arrows_.polyline.direction;
        }
        case ArrowType::Curved:
        {
            return settings_->arrows_.curved.direction;
        }
        case ArrowType::Connection:
        {
            return settings_->arrows_.connection.direction;
        }
        case ArrowType::LateralHorizontal:
        {
            return settings_->arrows_.lateralHorizontal.direction;
        }
        case ArrowType::LateralVertical:
        {
            return settings_->arrows_.lateralVertical.direction;
        }
        case ArrowType::TreeStyleHorizontal:
        {
            return settings_->arrows_.treeHorizontal.direction;
        }
        case ArrowType::TreeStyleVertical:
        {
            return settings_->arrows_.treeVertical.direction;
        }
        default:
        {
            return -1;
        }
    }
}

void DSConfiguration::setArrowDirection(QtNodes::Arrows::ArrowType type, int direction)
{
    switch (type)
    {
        case ArrowType::Line:
        {
            settings_->arrows_.line.direction = direction;
            break;
        }
        case ArrowType::PolyLine:
        {
            settings_->arrows_.polyline.direction = direction;
            break;
        }
        case ArrowType::Curved:
        {
            settings_->arrows_.curved.direction = direction;
            break;
        }
        case ArrowType::Connection:
        {
            settings_->arrows_.connection.direction = direction;
            break;
        }
        case ArrowType::LateralHorizontal:
        {
            settings_->arrows_.lateralHorizontal.direction = direction;
            break;
        }
        case ArrowType::LateralVertical:
        {
            settings_->arrows_.lateralVertical.direction = direction;
            break;
        }
        case ArrowType::TreeStyleHorizontal:
        {
            settings_->arrows_.treeHorizontal.direction = direction;
            break;
        }
        case ArrowType::TreeStyleVertical:
        {
            settings_->arrows_.treeVertical.direction = direction;
            break;
        }
        default:
        {
            return;
        }
    }
}

QColor DSConfiguration::arrowColor(QtNodes::Arrows::ArrowType type)
{
    switch (type)
    {
        case ArrowType::Line:
        {
            return settings_->arrows_.line.color;
        }
        case ArrowType::PolyLine:
        {
            return settings_->arrows_.polyline.color;
        }
        case ArrowType::Curved:
        {
            return settings_->arrows_.curved.color;
        }
        case ArrowType::Connection:
        {
            return settings_->arrows_.connection.color;
        }
        case ArrowType::LateralHorizontal:
        {
            return settings_->arrows_.lateralHorizontal.color;
        }
        case ArrowType::LateralVertical:
        {
            return settings_->arrows_.lateralVertical.color;
        }
        case ArrowType::TreeStyleHorizontal:
        {
            return settings_->arrows_.treeHorizontal.color;
        }
        case ArrowType::TreeStyleVertical:
        {
            return settings_->arrows_.treeVertical.color;
        }
        default:
        {
            return {};
        }
    }
}

void DSConfiguration::setArrowColor(QtNodes::Arrows::ArrowType type, const QColor &color)
{
    switch (type)
    {
        case ArrowType::Line:
        {
            settings_->arrows_.line.color = color;
            break;
        }
        case ArrowType::PolyLine:
        {
            settings_->arrows_.polyline.color = color;
            break;
        }
        case ArrowType::Curved:
        {
            settings_->arrows_.curved.color = color;
            break;
        }
        case ArrowType::Connection:
        {
            settings_->arrows_.connection.color = color;
            break;
        }
        case ArrowType::LateralHorizontal:
        {
            settings_->arrows_.lateralHorizontal.color = color;
            break;
        }
        case ArrowType::LateralVertical:
        {
            settings_->arrows_.lateralVertical.color = color;
            break;
        }
        case ArrowType::TreeStyleHorizontal:
        {
            settings_->arrows_.treeHorizontal.color = color;
            break;
        }
        case ArrowType::TreeStyleVertical:
        {
            settings_->arrows_.treeVertical.color = color;
            break;
        }
        default:
        {
            return;
        }
    }
}

QString DSConfiguration::repositoryLocalPath()
{
    return settings_->repository_.localPath;
}

void DSConfiguration::setRepositoryLocalPath(const QString &path)
{
    settings_->repository_.localPath = path;
}

QString DSConfiguration::repositoryRemotePath()
{
    return settings_->repository_.remotePath;
}

void DSConfiguration::setRepositoryRemotePath(const QString &path)
{
    settings_->repository_.remotePath = path;
}

QString DSConfiguration::repositoryFilter()
{
    return settings_->repository_.filter;
}

void DSConfiguration::setRepositoryFilter(const QString &filter)
{
    settings_->repository_.filter = filter;
}

//scene
int DSConfiguration::sceneMinZoom()
{
    return settings_->scene_.minZoom;
}

void DSConfiguration::setSceneMinZoom(int zoom)
{
    settings_->scene_.minZoom = zoom;
}

int DSConfiguration::sceneMaxZoom()
{
    return settings_->scene_.maxZoom;
}

void DSConfiguration::setSceneMaxZoom(int zoom)
{
    settings_->scene_.maxZoom = zoom;
}

int DSConfiguration::sceneZoomStep()
{
    return settings_->scene_.zoomStep;
}

void DSConfiguration::setSceneZoomStep(int step)
{
    settings_->scene_.zoomStep = step;
}

int DSConfiguration::sceneTextHideZoom()
{
    return settings_->scene_.textHideZoom;
}

void DSConfiguration::setSceneTextHideZoom(int zoom)
{
    settings_->scene_.textHideZoom = zoom;
}

QColor DSConfiguration::sceneBackColor()
{
    return settings_->scene_.backColor;
}

void DSConfiguration::setSceneBackColor(const QColor &color)
{
    settings_->scene_.backColor = color;
}