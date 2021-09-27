#include "ControllManager.hpp"

#include "platform/AboutDialog/AboutDialog.hpp"
#include "platform/AfterSuccessfullySavedDialog/AfterSuccessfullySavedDialog.hpp"
#include "platform/ConnectionCreatorDialog/ConnectionCreatorDialog.hpp"
#include "platform/DSMessageBox/DSMessageBox.hpp"
#include "platform/NewProjectDialog/NewProjectDialog.hpp"
#include "platform/ObjectAttributesDialog/ObjectAttributesDialog.hpp"
#include "platform/DeployProgressBarDialog/DeployProgressBarDialog.hpp"

#include "nodeeditor/nodes/ComponentItemImpl.h"
#include "nodeeditor/models/NodesScene.hpp"
#include "nodeeditor/graphicsComposites/universalGraphicsObject/UniversalGraphicsItem.h"
#include "nodeeditor/singletones/Facade.hpp"
#include "nodeeditor/singletones/NameController.hpp"
#include "nodeeditor/views/FlowView.hpp"

#include "component_creator/IDEProjectProperties.hpp"
#include "component_creator/ComponentCreateConfig.h"
#include "component_creator/ComponentGenerator/ComponentGenerator.h"

#include "IDEProjectCreator.hpp"
#include "MessagesDeserializer.hpp"
//#include "NetworkManager.hpp"
#include "OutXmlGenerator.hpp"
#include "Project.hpp"
#include "ProjectDocumentCreator.hpp"
#include "DSConfiguration.h"

#include "SettingsManager.hpp"

#include "repository/manager/ConfigLoader.hpp"
#include "repository/manager/RepositoryManager.hpp"
#include "repository/ui/Repository.hpp"

#include "models/CommunicationModel.hpp"
#include "models/MessageModel.hpp"
#include "models/MimeStoreModel.hpp"
#include "models/ProjectModel.hpp"
#include "models/ProjectTreeModel.hpp"
#include "models/PropertiesTreeModel.hpp"
#include "models/RecentProjectsModel.hpp"

#include "monitor/MonitorWidget.hpp"
#include "deploy/DeployWidget.hpp"
#include "update/UpdateWidget.hpp"
#include "sniffer/NetworkWidget.hpp"

#include "common/globals.h"
#include "ComponentCreator.h"

#include <QAbstractItemModel>
#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QDesktopServices>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>


#include <Poco/Zip/Compress.h>
#include <Poco/Path.h>

#include <memory>
#include <fstream>

#define MODEL_TAG "model"
#define COMPONENT_SCENE_TAG "components"
#define DEVICE_SCENE_TAG "devices"
#define RECENT_PROJECTS_FILE "recent.json"
#define MAX_PROJECT_COUNT 8

QString validateProjectFileName(const QString &fileName)
{
    if (QFileInfo info {fileName}; info.completeSuffix() != FILE_PROJECT_EXTENSION)
    {
        return {fileName + '.' + FILE_PROJECT_EXTENSION};
    }
    return fileName;
}

using namespace dscontrol;
//using namespace network;
// using namespace widgets;

ControllManager::ControllManager(QObject* parent)
: QObject {parent}
, currentProject_ {nullptr}
, projectModel_ {nullptr}
, projectTreeModel_ {nullptr}
, recentProjects_ {nullptr}
, mimeStore_ {nullptr}
, communicationModel_ {nullptr}
, messageModel_ {nullptr}
, monitorWidget_ {nullptr}
, networkWidget_ {nullptr}
//, networkManager_ {nullptr}
, propertiesTreeModel_ {nullptr}
, repositoryManager_ {nullptr}
, userManualFilename_ {"UserManual.pdf"}
{
    qDebug() << "ControllManager";

    recentProjects_ = new RecentProjectsModel;
    recentProjectWatcher_ = new QFileSystemWatcher(this);

    loadRecentProjects();

    projectModel_ = new ProjectModel;
    projectTreeModel_ = new ProjectTreeModel;
    mimeStore_ = new MimeStoreModel {this};

    communicationModel_ = new CommunicationModel {QtNodes::Facade::Instance().communicationList(), this};
    messageModel_ = new MessageModel {QtNodes::Facade::Instance().messageList(), this};

    monitorWidget_ = MonitorWidget::init();
    // TODO Monitor
    // connect(monitorWidget_, &MonitorWidget::selStoped, this, &ControllManager::stopApplications);

    networkWidget_ = NetworkWidget::init();
    updateWidget_ = UpdateWidget::init();
    deployWidget_ = DeployWidget::init();

    // TODO Deploy
    //networkManager_ = NetworkManager::init();
    // connect(networkManager_, &NetworkManager::deployError, this, &ControllManager::deployError, Qt::UniqueConnection);
    // connect(networkManager_, &NetworkManager::uploadComplete, this, &ControllManager::uploadFinished,
    //         Qt::UniqueConnection);
    // connect(networkManager_, &NetworkManager::uploadError, this, &ControllManager::uploadError, Qt::UniqueConnection);

    // connect(networkManager_, &NetworkManager::progressValueChanged, this, &ControllManager::updateProgressBar,
    //         Qt::UniqueConnection);
    // TODO Deploy
    // иницилиазируем Network
    //networkManager_->exec();

    //connect(networkManager_, &NetworkManager::monitorDataUpdated, this, &ControllManager::monitorDataUpdated);
    _serverComm=new DSServerComm();
    connect(_serverComm, &DSServerComm::UpdatedMonitorData, this, &ControllManager::UpdatedMonitorData, Qt::UniqueConnection);

    connect(&QtNodes::Facade::Instance(), &QtNodes::Facade::itemDoubleClicked, this,
            &ControllManager::openConnectionDialog);
    QtNodes::NameController::Instance()->Init();

    connect(recentProjectWatcher_, &QFileSystemWatcher::fileChanged, this, &ControllManager::fileChangedSlot);

    setRepository(new RepositoryManager {this});
}

ControllManager::~ControllManager()
{
    if (currentProject_)
    {
        delete currentProject_;
    }

    delete monitorWidget_;
    delete networkWidget_;
    delete updateWidget_;
    delete deployWidget_;

    delete projectModel_;
    delete projectTreeModel_;
    delete recentProjects_;
    delete messageModel_;
    delete communicationModel_;

    if (QFile::exists(userManualFilename_))
    {
        QFile::remove(userManualFilename_);
    }

    //delete networkManager_;
}

void ControllManager::setRepository(RepositoryManager* repositoryManager)
{
    if (!repositoryManager)
    {
        return;
    }

    repositoryManager_ = repositoryManager;

    auto components = repositoryManager_->loader()->components();
    for (const auto &[path, component] : components)
    {
        mimeStore_->addComponentConfig(component);
    }

    emit repositoryManagerChanged();

    connect(repositoryManager_, &RepositoryManager::contentHasChanged, [this]()
    {
        auto components = repositoryManager_->loader()->components();
        auto componentsToRemove = repositoryManager_->loader()->componentsToRemove();
        for (const auto &[path, component] : components)
        {
            mimeStore_->addComponentConfig(component);
        }
        for (const auto &[path, component] : componentsToRemove)
        {
            mimeStore_->removeComponentConfig(component);
        }
    });
}

QString ControllManager::makeTabTitle(const QString &name, int maxSize)
{
    QString tabTitle = name;
    if (int size = tabTitle.size(); size > maxSize)
    {
        if (int index = tabTitle.lastIndexOf("_"); index != -1)
        {
            auto ending = tabTitle.right(size - index - 1);
            ending.insert(0, "...");
            tabTitle = tabTitle.left(maxSize - ending.size()) + ending;
        }
        else
        {
            tabTitle = tabTitle.leftJustified(maxSize, '.');
        }
    }
    return tabTitle;
}

Project* ControllManager::currentProject()
{
    return currentProject_;
}

const Project* ControllManager::currentProject() const
{
    return currentProject_;
}

QAbstractListModel* ControllManager::recentProjects() const
{
    return recentProjects_;
}

QDir ControllManager::currentPath()
{
    return QDir {QDir::currentPath()};
}

QDir ControllManager::currentPathXML()
{
    // TODO xml path
    QString pathBin {DSConfiguration::Instance().baseFolder()};
    return QDir {pathBin + "/xml/"};
}

void ControllManager::newProject()
{
    qDebug() << "Set new project";
    NewProjectDialog dialog;
    if (dialog.exec() == QDialog::DialogCode::Accepted)
    {
        if (currentProject_)
            closeProject();

        // here we not check about extension, because we get not file, but
        // project name
        newProject(
        dialog.getProjectPath() + QDir::separator() + dialog.getProjectName() + '.' + FILE_PROJECT_EXTENSION);
    }
}

void ControllManager::newProject(const QString &projectFile)
{
    auto project = new Project {projectFile};
    if (currentProject_)
    {
        closeProject();
    }

    setNewProject(project);
    saveProject(*currentProject_, true);
    MessagesDeserializer::getMessages1(currentPathXML());
}

void ControllManager::openProject()
{
    qDebug() << "Open project";
    auto fileName = QFileDialog::getOpenFileName(nullptr, tr("Open Project"), QDir::currentPath(),
                                                 QString {"(*.%1)"}.arg(FILE_PROJECT_EXTENSION));

    if (!fileName.isEmpty())
    {
        openProject(fileName);
    }
}

void ControllManager::openProject(const QString &projectFile)
{
    qDebug() << "try open project: " << projectFile;
    auto valid = validateProjectFileName(projectFile);
    if (valid != projectFile || !QFileInfo {projectFile}.exists())
    {
        setNewProject(nullptr);

        qWarning() << "project can not be opened because file not exists";
        DSMessageBox {DSMessageBox::Icon::Warning, tr("Warning"), tr("Project can not be opened")}.exec();
    }
    else
    {
        QJsonObject flowData;
        if (QFile file {projectFile}; file.open(QIODevice::ReadOnly))
        {
            flowData = QJsonDocument::fromJson(file.readAll()).object();
            file.close();
        }
        else
        {
            qWarning() << "project file not opened";
            DSMessageBox {DSMessageBox::Icon::Warning, tr("Warn"), tr("Project file can not be opened")}.exec();
            return;
        }

        auto project = new Project {projectFile};

        if (currentProject_)
        {
            closeProject();
        }

        saveToRecentProjects(projectFile);
        project->load(flowData);
        setNewProject(project);

        //TODO: think about it is necessary
        //MessagesDeserializer::getMessages1(currentPathXML());

        // after load information to model
        //    communicationModel_->restore(flowData["communicationModel"].toObject());
        //    messageModel_->restore(flowData["messageModel"].toObject());

        qDebug() << "project opened";
    }
}

void ControllManager::saveProject()
{
    if (currentProject_)
    {
        saveProject(*currentProject_, false);
    }
}

void ControllManager::saveProject(const Project &savingProject, bool isNewProject)
{
    const QString &projectFileName = validateProjectFileName(savingProject.fileName());
    const bool isProjectFileNull = projectFileName.isNull();
    if (isProjectFileNull && !isNewProject)
    {
        DSMessageBox {DSMessageBox::Icon::Warning, tr("Warning"), tr("Project can not be saved")}.exec();
        return;
    }

    if (isProjectFileNull)
    {
        return;
    }

    QFile file {projectFileName};
    if (!file.open(QIODevice::WriteOnly))
    {
        return;
    }

    QJsonObject saveData = savingProject.save();
    const qint64 byteCapacity {file.write(QJsonDocument {saveData}.toJson())};
    file.close();

    if (byteCapacity == 0)
    {
        return;
    }

    saveToRecentProjects(QFileInfo {file}.absoluteFilePath());
    if (isNewProject)
    {
        return;
    }

    AfterSuccessfullySavedDialog save;
    save.exec();
}

QMessageBox::StandardButton ControllManager::saveProjectAs()
{
    if (nullptr == currentProject_)
    {
        return QMessageBox::Cancel;
    }

    auto fileName = QFileDialog::getSaveFileName(nullptr, tr("Save As"), QDir::currentPath(),
                                                 QString {"*.%1"}.arg(FILE_PROJECT_EXTENSION));
    if (fileName.isEmpty())
    {
        return QMessageBox::Cancel;
    }
    fileName = validateProjectFileName(fileName);

    Project savingProject {fileName};
    savingProject.copyFrom(*currentProject_);
    saveProject(savingProject, false);
    return QMessageBox::Save;
}

void ControllManager::closeProject()
{
    qDebug() << "close project";
    if (currentProject_)
    {
        QtNodes::Facade::Instance().clear();
        delete currentProject_;
        currentProject_ = nullptr;
        emit projectClosed();
    }
}

void ControllManager::aboutApplication()
{
    AboutDialog dlg(AboutDialog::Type::VIDE);
    dlg.exec();
}

void ControllManager::aboutCompany()
{
    AboutDialog dlg(AboutDialog::Type::CETC);
    dlg.exec();
}

void ControllManager::showHelpCenter()
{
    QFile file(":/docs/resources/docs/" + userManualFilename_);
    QString path = qApp->applicationDirPath().append("/" + userManualFilename_);
    file.copy(path);

    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void ControllManager::setNewProject(Project* project)
{
    if (project == currentProject_)
    {
        return;
    }
    currentProject_ = project;
    projectModel_->setProject(currentProject_);
    if (nullptr != currentProject_)
    {
        projectTreeModel_->setProject(currentProject_->name());
        emit projectOpened();
    }
    else
    {
        projectTreeModel_->setProject("");
    }
    // connect(currentProject_, &Project::nameHasChanged, this, &ControllManager::projectNameHasChanged);
}

QAbstractItemModel* ControllManager::projectModel() const
{
    return projectModel_;
}

ProjectTreeModel* ControllManager::projectTreeModel() const
{
    return projectTreeModel_;
}

void ControllManager::loadRecentProjects()
{
    qDebug() << "load recent projects";
    if (QFile recentProjectFile {RECENT_PROJECTS_FILE}; recentProjectFile.exists() && recentProjectFile.open(
    QIODevice::ReadOnly))
    {
        QStringList recentProjectsList;
        for (const auto i : QJsonDocument::fromJson(recentProjectFile.readAll()).array())
        {
            // here we add to the list files only if they exists
            if (QFileInfo {i.toString()}.exists())
            {
                recentProjectsList.append(i.toString());
                recentProjectWatcher_->addPath(i.toString());
            }
        }
        recentProjectFile.close();

        recentProjects_->setStringList(recentProjectsList);
    }
}

void ControllManager::saveToRecentProjects(const QString &fileName)
{
    qDebug() << "save to recent projects";

    // get list of all saved projects and save current at first
    auto projectsList = recentProjects_->stringList();

    if (projectsList.contains(fileName))
    {
        return;
    }
    if (QFile recentProjectFile {RECENT_PROJECTS_FILE}; recentProjectFile.open(QIODevice::WriteOnly))
    {
        if (projectsList.size() == MAX_PROJECT_COUNT)
        {
            recentProjectWatcher_->removePath(projectsList.last());
            projectsList.removeLast();
        }
        projectsList.push_front(fileName);
        recentProjectWatcher_->addPath(fileName);

        // write at first current fileName
        QJsonArray recentProjectsArray {};
        for (const auto &i : projectsList)
        {
            recentProjectsArray.push_back(i);
        }

        // save in file
        recentProjectFile.write(QJsonDocument {recentProjectsArray}.toJson());

        recentProjectFile.close();

        // set new list to model
        recentProjects_->setStringList(projectsList);
    }
}

void ControllManager::removeFromRecentProjects(const QString &fileName)
{
    if (QFile recentProjectFile {RECENT_PROJECTS_FILE}; recentProjectFile.open(QIODevice::WriteOnly))
    {
        // get list of all saved projects and save current at first
        auto projectsList = recentProjects_->stringList();
        projectsList.removeOne(fileName);

        // write at first current fileName
        QJsonArray recentProjectsArray;
        for (const auto &i : projectsList)
        {
            recentProjectsArray.push_back(i);
        }

        // save in file
        recentProjectFile.write(QJsonDocument {recentProjectsArray}.toJson());

        recentProjectFile.close();

        // set new list to model
        recentProjects_->setStringList(projectsList);
    }
}

void ControllManager::fileChangedSlot(const QString &path)
{
    if (nullptr == recentProjectWatcher_ || sender() != recentProjectWatcher_ || recentProjectWatcher_->files().contains(
    path))
    {
        return;
    }
    recentProjectWatcher_->addPath(path);
}

// void ControllManager::deployError(const QString &, const QString &error)
// {
//     QMessageBox::critical(nullptr, "Critical", error);
// }
//
// void ControllManager::uploadFinished(const QString &ipAddress)
// {
//     auto found = uploadDocuments_.find(ipAddress);
//     if (found != uploadDocuments_.end())
//     {
//         auto &apps = found->second;
//         if (!apps.empty())
//         {
//             auto app = *apps.begin();
//             apps.erase(apps.begin());
//
//             // TODO Upload
//             // тут надо доработать upload
//             networkManager_->publish(app.filePath, ipAddress, app.componentName, app.xml,
//                                      DS::dsserver::CommandType::ReceiveComponent);
//         }
//     }
//
//     deploy();
// }

// void ControllManager::uploadError(const QString &componentName, const QString &ipAddress, const QString &error)
// {
//     if (progressBar_)
//     {
//         progressBar_->setProgressError(componentName, ipAddress, error);
//     }
// }

// void ControllManager::updateProgressBar(const QString &componentName, const QString &ipAddress, qreal value)
// {
//     if (progressBar_)
//     {
//         progressBar_->updateShowingData(componentName, ipAddress, value);
//     }
// }

void ControllManager::monitorDataUpdated(QString itIp,const QByteArray &json)
{
    //qDebug() << "ControllManager::monitorDataUpdated";
    // TODO: deploy
    // if (!monitorWidget_->isHidden())
    // {
    monitorWidget_->updateData(itIp, json);

    // }

    // if (!networkWidget_->isHidden())
    // {
    // networkWidget_->updateData(json);
    // }

    // if (auto ret = networkManager_->isServerUpload(networkManager_->ipToString(key)); ret)
    // {
    //     // TODO: remove fix path
    //     // TODO Monitor
    //     // виджет монитор принимает данные
    //     QString path;
    //     // делаем поиск, если нету, возвращает ""
    //     auto sys = monitorWidget_->getSystemName(key);
    //     if (sys.isEmpty())
    //     {
    //         monitorWidget_->updateData({key, json});
    //         sys = monitorWidget_->getSystemName(key);
    //     }
    //
    //     // TODO ?
    //     // тут походу отправляется Samples.zip для нового подключенного пользователя (DSHost)
    //
    //     path = QString("%1Sample_%2.zip").arg(DEFAULT_SAMPLE_PATH).arg(sys);
    //     auto data = OutXmlGenerator::generateApplicationTemplateXmls(path);
    //     QFile file {"1.xml"};
    //     file.open(QIODevice::ReadWrite);
    //     file.write(data);
    //     file.close();
    //     networkManager_->initNewServer(path, networkManager_->ipToString(key), data);
    // }
}

// void ControllManager::projectNameHasChanged(const QString &)
// {
//     if (sender() != currentProject_ || currentProject_ == nullptr)
//     {
//         return;
//     }
//     if (nullptr != projectModel_)
//     {
//         projectModel_->updateProjectName();
//     }
//     if (nullptr != projectTreeModel_)
//     {
//         projectTreeModel_->setProjectName(currentProject_->name());
//     }
// }

// void ControllManager::stopApplications(const std::map<QString, QStringList> &applicationsToStop)
// {
//     TODO: deploy
//     сюда отправляем xml на Сервер
//     на котором надо остановить
//
//     std::map<quint32, std::set<QByteArray>> stoped;
//     for (const auto &[ip, apps] : applicationsToStop)
//     {
//         stoped.insert({ip, OutXmlGenerator::generateStopXmls(apps)});
//     }
//
//     networkManager_->stopApplications(stoped);
// }

void ControllManager::viewToClipboard(const QRect &area)
{
    auto model = QtNodes::Facade::Instance().sceneModel();
    if (!model)
    {
        return;
    }

    auto scene = model->getTopScene();
    auto view = model->getTopView();

    if (!scene || !view)
    {
        return;
    }

    auto viewport = view->viewport();
    if (!viewport)
    {
        return;
    }

    QClipboard* clipboard = QApplication::clipboard();

    scene->clearSelection();

    QRect curRect;
    QPoint offset;
    if (area.isEmpty())
    {
        curRect = viewport->rect();
        offset = {0, 0};
    }
    else
    {
        curRect = area;
        offset = area.topLeft();
    }
    QImage image(curRect.size(), QImage::Format_ARGB32);

    image.fill(view->getBackColor());
    QPainter painter(&image);
    viewport->render(&painter, -offset);

    clipboard->setImage(image);

    DSMessageBox::information(nullptr, tr("Information"), tr("Image was successfully copied to clipboard!"));
}

QAbstractItemModel* ControllManager::messageModel() const
{
    qDebug() << "messageModel";
    return messageModel_;
}

MimeStoreModel* ControllManager::getMimeStoreModel()
{
    return mimeStore_;
}

RepositoryManager* ControllManager::repositoryManager() const
{
    return repositoryManager_;
}

void ControllManager::setPlatformLanguage(const QLocale &language)
{
    if (language_ == language)
        return;

    auto old = language_;
    language_ = language;
    switch (language_.language())
    {
        case QLocale::Language::English:
        {
            qApp->removeTranslator(&translator_);
            if (translator_.load(SettingsManager::localization + "en"))
            {
                qApp->installTranslator(&translator_);
                if (projectModel_)
                    projectModel_->translateCaptions();
                if (projectTreeModel_)
                    projectTreeModel_->translateCaptions();
                if (propertiesTreeModel_)
                    propertiesTreeModel_->translateCaptions();
            }
            break;
        }
        case QLocale::Language::Chinese:
        {
            qApp->removeTranslator(&translator_);
            if (translator_.load(SettingsManager::localization + "cn"))
            {
                qApp->installTranslator(&translator_);
                if (projectModel_)
                    projectModel_->translateCaptions();
                if (projectTreeModel_)
                    projectTreeModel_->translateCaptions();
                if (propertiesTreeModel_)
                    propertiesTreeModel_->translateCaptions();
            }
            break;
        }
        default:
        {
            qWarning() << "ControllManager::setPlatformLanguage: Uknown language!";
            break;
        }
    }

    emit languageChanged(old);
}

QLocale ControllManager::platformLanguage() const
{
    return language_;
}

// QAbstractItemModel *ControllManager::propertyModel() const
//{
//    return propertyModel_;
//}

CommunicationModel* ControllManager::communicationModel() const
{
    return communicationModel_;
}

void ControllManager::generateCodeComponent()
{
    //ComponentCreateConfig conf;
    ComponentGenerator dlg;
    dlg.exec();
}

void ControllManager::setSelectedItemsToBackground()
{
    auto sceneModel = QtNodes::Facade::Instance().sceneModel();
    if (nullptr != sceneModel)
    {
        auto topScene = sceneModel->getTopScene();
        if (nullptr != topScene)
        {
            QList<QGraphicsItem*> selectedItems;
            QtNodes::UniversalSelectableItem* selectedSelectableItem;
            selectedItems = topScene->selectedItems();
            for (const auto &selectedItem : selectedItems)
            {
                selectedSelectableItem = dynamic_cast<QtNodes::UniversalSelectableItem*>(selectedItem);
                if (nullptr != selectedSelectableItem)
                {
                    selectedSelectableItem->setToBottomMostVisible();
                }
            }
            if (!selectedItems.isEmpty())
            {
                topScene->update();
            }
        }
    }
}

void ControllManager::setSelectedItemsToForeground()
{
    auto sceneModel = QtNodes::Facade::Instance().sceneModel();
    if (nullptr != sceneModel)
    {
        auto topScene = sceneModel->getTopScene();
        if (nullptr != topScene)
        {
            QList<QGraphicsItem*> selectedItems;
            QtNodes::UniversalSelectableItem* selectedSelectableItem;
            selectedItems = topScene->selectedItems();
            for (const auto &selectedItem : selectedItems)
            {
                selectedSelectableItem = dynamic_cast<QtNodes::UniversalSelectableItem*>(selectedItem);
                if (nullptr != selectedSelectableItem)
                {
                    selectedSelectableItem->setToTopMostVisible();
                }
            }
            if (!selectedItems.isEmpty())
            {
                topScene->update();
            }
        }
    }
}

void ControllManager::openConnectionDialog(const QUuid &id)
{
    const auto* dataFlowModel = QtNodes::Facade::Instance().model();
    if (nullptr == dataFlowModel)
    {
        return;
    }
    using QtNodes::Item::NodeType;
    const NodeType nodeType = dataFlowModel->nodeType(dataFlowModel->nodeIndex(id));
    if (nodeType == NodeType::ComponentNode || nodeType == NodeType::ExternalNode)
    {
        dscontrol::ConnectionCreatorDialog dialog {id};
        dialog.exec();
    }
    else
    {
        dscontrol::ObjectAttributesDialog dialog {id};
        dialog.exec();
    }
}

void ControllManager::upload()
{
    // TODO Deploy
//     auto json = QtNodes::Facade::Instance().save();
//     auto outComputers = OutXmlGenerator::generateComputersMap(json);
//
//     if (progressBar_ != nullptr)
//     {
//         // networkManager_->uploadCancel();
//         progressBar_ = nullptr;
//     }
//
//     progressBar_ = new DeployProgressBarDialog;
//
//     connect(progressBar_, &DeployProgressBarDialog::destroyed, [=]() mutable
//     {
//         // networkManager_->uploadCancel();
//         progressBar_ = nullptr;
//     });
//
//     // connect(progressBar, &DeployProgressBarDialog::onCancelBtnClicked, networkManager_, &NetworkManager::uploadCancel, Qt::UniqueConnection);
//     connect(progressBar_, &DeployProgressBarDialog::onOkBtnClicked, this, &ControllManager::deploy,
//             Qt::UniqueConnection);
//     connect(progressBar_, &DeployProgressBarDialog::onCancelBtnClicked, progressBar_, &DeployProgressBarDialog::close,
//             Qt::UniqueConnection);
//
//     if (!uploadDocuments_.empty())
//     {
//         for (auto &[ip, value] : uploadDocuments_)
//         {
//             auto object = *value.begin();
//             value.erase(value.begin());
//
//             qDebug() << "ip: " << ip;
//             qDebug() << "object.filePath: " << object.filePath;
//             qDebug() << "object.componentName: " << object.componentName;
//             qDebug() << "object.xml.toByteArray: " << object.xml;
//             // TODO Upload
//
//             networkManager_->publish(object.filePath, ip, object.componentName, object.xml,
//                                      DS::dsserver::CommandType::ReceiveComponent);
//         }
//     }
//
//     // progressBar_->setShowingData(outComputers);
//     // progressBar_->show();
//
//     deploy();
}

void ControllManager::deploy()
{
    // TODO Deploy
//     for (const auto&[ip, setValue] : deployDocuments_)
//     {
//         for (const auto &value  : setValue)
//         {
//             qDebug() << "value.xml " << value.xml.toString();
//             networkManager_->publish(ip, value.xml.toString(), DS::dsserver::CommandType::Deploy);
//         }
//     }
}

void ControllManager::openDeployProject()
{
    // TODO Deploy
    auto hierarchy = OutXmlGenerator::generateProjectHierarchy(currentProject_->unsafeSave());
    auto generated = OutXmlGenerator::generateApplicationXmls(QtNodes::Facade::Instance().save());
    auto uploads = OutXmlGenerator::generateUploadXmls(QtNodes::Facade::Instance().save());

    qDebug() << "--------hierarchy---------";
//     for (const auto &[level, name, ip] : hierarchy)
//     {
//         qDebug() << "level: " << level;
//         qDebug() << "name: " << name;
//         qDebug() << "ip:" << ip;
//     }
    qDebug() << "--------generated---------";
    for (const auto &item : generated)
    {
        qDebug() << "appName: " << item.appName;
        qDebug() << "xml: " << item.xml.toString() << "\n\n";
    }
    qDebug() << "--------uploads---------";
    for (const auto &item : uploads)
    {
        qDebug() << "appName: " << item.appName;
        qDebug() << "componentName: " << item.componentName;
        qDebug() << "filePath: " << item.filePath;
        qDebug() << "xml: " << item.xml;
    }

    deployDocuments_.clear();
    uploadDocuments_.clear();

    auto deployLambda = [&](bool, const std::map<QString, QString> &applications)
    {
        // формирует xml
        for (const auto &[app, ip] : applications)
        {
            auto found = std::find_if(generated.begin(), generated.end(), [app = &app](const DeployObject &value)
            {
                if (value.appName == app)
                {
                    return true;
                }
                return false;
            });

            auto foundUpload = std::find_if(uploads.begin(), uploads.end(), [app = &app](const UploadObject &value)
            {
                if (value.appName == app)
                {
                    return true;
                }
                return false;
            });

            if (found != generated.end())
            {
                auto[it, ret] = deployDocuments_.insert({ip, {*found}});
                if (!ret)
                {
                    it->second.insert(*found);
                }
            }

            if (foundUpload != uploads.end())
            {
                auto[it, ret] = uploadDocuments_.insert({ip, {*foundUpload}});
                if (!ret)
                {
                    it->second.insert(*foundUpload);
                }
            }
        }

        deployWidget_->close();
        //upload();
        createDeployFiles();

    };

    // TODO Deploy
    connect(deployWidget_, &DeployWidget::deploySelectedBtn, deployLambda);

    // передаем в виджет deployWidget (xml, list active IP)
    deployWidget_->setupData(hierarchy, monitorWidget_->targetDeploy());
    deployWidget_->exec();

    DeployList list =createDeployFiles();
    for(DeployList::iterator it=list.begin(); it!=list.end(); it++)
    {
            _serverComm->AssignDeployTask(std::get<0>(*it),std::get<2>(*it) , std::get<1>(*it));
    }
    _serverComm->StartDeployTask();
}


void ControllManager::removeDeployFiles()
{
    QDir deploy{DSConfiguration::Instance().deployFolder()};
    if (deploy.exists())
    {
        deploy.removeRecursively();
    }
}

ControllManager::DeployList ControllManager::createDeployFiles()
{
    QDir base{DSConfiguration::Instance().baseFolder()};
    if (!base.exists("deploy"))
    {
        base.mkpath(DSConfiguration::Instance().deployFolder());
    }
    std::vector<DeployInfo> deploys;
    for (const auto&[ip, obj] : deployDocuments_)
    {
        for (const auto &app : obj)
        {
            DeployInfo deploy;
            deploy.ip = ip;
            auto appName = app.appName;
            deploy.appName = appName;
            auto toDir = DSConfiguration::Instance().deployFolder()
                + QDir::separator() + appName;
            deploy.filePath = DSConfiguration::Instance().deployFolder()
                    + QDir::separator();
            //TODO check sample folder subfolders?
            copyAndReplaceFolderContents(DSConfiguration::Instance().sampleFolder()
                , toDir);

            auto bundles = uploadDocuments_[ip];

            QString bundlesDir = toDir + QDir::separator()
                + "components";// +QDir::separator() + "business";
            for (const auto& bundle : bundles)
            {
                if (bundle.appName == appName)
                {
                    QFileInfo file(bundle.filePath);
                    if(file.exists())
                    {
                        QFile::copy(file.absoluteFilePath(), bundlesDir
                            + QDir::separator() + file.fileName());
                    }
                }
            }
            QFile out(toDir + QDir::separator() + "config/components.xml");
            if (out.open(QIODevice::WriteOnly | QIODevice::Truncate))
            {
                out.write(app.xml.toByteArray());
                out.close();
            }
            //copy server bundles
            auto xml = app.xml;
            QStringList serverBundles;
            if (auto root = xml.documentElement(); !root.isNull() && root.nodeName() == "root")
            {
                for (auto host = root.firstChildElement(); !host.isNull(); host = host.nextSiblingElement())
                {
                    if (host.nodeName() == "DSHost")
                    {
                        for (auto element = host.firstChildElement(); !element.isNull(); element = element.nextSiblingElement())
                        {
                            if (element.nodeName() == "Protocols")
                            {
                                for (auto protocol = element.firstChildElement(); !protocol.isNull();
                                    protocol = protocol.nextSiblingElement())
                                {
                                    auto component = protocol.attributeNode("component").value();
                                    if (!component.isEmpty())
                                    {
                                        serverBundles.push_back(component);
                                    }
                                }
                            }
                            if (element.nodeName() == "Communication")
                            {
                                for (auto communication = element.firstChildElement(); !communication.isNull();
                                    communication = communication.nextSiblingElement())
                                {
                                    auto component = communication.attributeNode("component").value();
                                    if (!component.isEmpty())
                                    {
                                        serverBundles.push_back(component);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            for (const auto& bundle : serverBundles)
            {
                QFileInfo file;
                for (const auto &component : repositoryManager_->loader()->components())
                {
                    auto name = component.first;
                    QFileInfo fi{name};
                    QString temp = name;
                    auto baseName = temp.remove(fi.absolutePath());
                    baseName.remove("/");
                    if (baseName == bundle + ".bndl")
                    {
                        file.setFile(name);
                        break;
                    }
                    else
                    {
                        continue;
                    }
                }
                if (file.exists())
                {
                    QFile::copy(file.absoluteFilePath(), bundlesDir
                        + QDir::separator() + file.fileName());
                }
            }

            QString zipFile = DSConfiguration::Instance().deployFolder() + QDir::separator()
                + appName + ".zip";
            deploy.file=appName + ".zip";
            //deploy.filePath = compressFolderToFile(toDir, zipFile);

            deploys.push_back(deploy);
        }
    }
    std::string deployPath;
    ControllManager::DeployList fileList;
    for (const auto& dep : deploys)
    {
        fileList.push_back(std::make_tuple((dep.ip).toStdString(),(dep.appName).toStdString(),(dep.file).toStdString()));
        deployPath=(dep.filePath).toStdString();
        qDebug() << "deploys:";
        qDebug() << "ip:" << dep.ip;
        qDebug() << "app:" << dep.appName;
        qDebug() << "file:" << dep.filePath;
    }
    _serverComm->SetFtpHome(deployPath);
    return fileList;
    //deploy(deploys)
    //removeDeployFiles();
}

QString ControllManager::compressFolderToFile(const QString& folder, const QString& file)
{
    bool success = true;
    std::ofstream out(file.toStdString(), std::ios::binary);
    Poco::Zip::Compress c(out, true);

    Poco::Path data(folder.toStdString());
    data.makeDirectory();
    c.addRecursive(data);
    c.close();

    return success ? file : QString{};
}

void ControllManager::copyAndReplaceFolderContents(const QString& fromDir, const QString& toDir, bool copyAndRemove)
{
    QDirIterator it(fromDir, QDirIterator::Subdirectories);
    QDir dir(fromDir);
    const int absSourcePathLength = dir.absoluteFilePath(fromDir).length();

    while (it.hasNext())
    {
        it.next();
        const auto fileInfo = it.fileInfo();
        if (!fileInfo.isHidden())
        { //filters dot and dotdot
            const QString subPathStructure = fileInfo.absoluteFilePath().mid(absSourcePathLength);
            const QString constructedAbsolutePath = toDir + subPathStructure;

            if (fileInfo.isDir())
            {
                //Create directory in target folder
                dir.mkpath(constructedAbsolutePath);
            }
            else if (fileInfo.isFile())
            {
                //Copy File to target directory

                //Remove file at target location, if it exists, or QFile::copy will fail
                QFile::remove(constructedAbsolutePath);
                QFile::copy(fileInfo.absoluteFilePath(), constructedAbsolutePath);
            }
        }
    }

    if (copyAndRemove)
        dir.removeRecursively();
}

std::set<std::string> ControllManager::getServersIP()
{
    auto hierarchy = OutXmlGenerator::generateProjectHierarchy(currentProject_->unsafeSave());

    std::set<std::string> serversIP;

    for (const auto& [level, name, ip] : hierarchy)
    {
        auto type = static_cast<QtNodes::DSLevels>(level);

        switch (type)
        {
            case QtNodes::DSLevels::Component:
                serversIP.insert(ip.toStdString());
                break;
            default:
                break;
        }
    }
    return serversIP;
}
void ControllManager::setPropertiesTreeModel(PropertiesTreeModel* model)
{
    propertiesTreeModel_ = model;
}

QPixmap ControllManager::exportToImage(const QRect &area)
{
    auto model = QtNodes::Facade::Instance().sceneModel();
    if (!model)
    {
        return {};
    }

    auto topView = model->getTopView();
    if (!topView)
    {
        return {};
    }

    auto topScene = topView->flowScene();
    if (!topScene)
    {
        return {};
    }

    QGraphicsView newView;
    newView.setScene(topScene);

    QSize size {topView->sceneRect().size().toSize() + QSize {2, 2}};
    QSize curSize;
    QPoint offset;

    if (area.isEmpty())
    {
        curSize = size;
        offset = {0, 0};
    }
    else
    {
        curSize = area.size();
        offset = area.topLeft();
    }
    QRect rect {offset, curSize};

    newView.setFixedSize(size);
    newView.setBackgroundBrush(topView->getBackColor());

    model->clearSelection();

    QPixmap resultPixmap {curSize};
    QPixmap contentPixmap = newView.grab(rect);
    QRect resultRect {{0, 0}, curSize};

    QPainter painter {&resultPixmap};
    painter.drawPixmap(resultRect, contentPixmap, resultRect);

    return resultPixmap;
}

const Project* ControllManager::getProjectById(const QUuid &) const
{
    return currentProject();
}

void ControllManager::showRepository()
{
    if (repository_->isVisible())
    {
        repository_->close();
        return;
    }
    repository_->show();
}

void ControllManager::showMonitoring()
{
    std::set<std::string> serversIP=getServersIP();
    std::vector<std::string> serverList;
    std::copy(serversIP.begin(), serversIP.end(), std::back_inserter(serverList));
    std::vector<std::string>::iterator it;
    for(it=serverList.begin();it!=serverList.end();it++) *it=matchIP(*it);
    sort(serverList.begin(),serverList.end());
    serverList.erase(unique(serverList.begin(),serverList.end()),serverList.end());
    _serverComm->Stop();
    _serverComm->Init(serverList);
    monitorWidget_->assignServerComm(_serverComm);
    _serverComm->SetMonitor(true);
    if (monitorWidget_->isVisible())
    {
        monitorWidget_->close();
        return;
    }
    monitorWidget_->show();
}

void ControllManager::showNetwork()
{
    if (networkWidget_->isVisible())
    {
        networkWidget_->close();
        return;
    }
    networkWidget_->show();
}

QString ControllManager::copyFiles(const QString &source, const QString &dest, const QStringList &filter)
{
    QString output = source;
    if (source != dest)
    {
        QDirIterator it(source, filter, QDir::Files, QDirIterator::Subdirectories);
        QStringList include;
        QDir destDir {dest};
        while (it.hasNext())
        {
            auto file = it.next();
            auto srcFile = file;
            auto newFile = file.replace(source, dest);

            auto newFileInfo = QFileInfo {newFile};
            auto newPath = newFileInfo.absolutePath();
            QDir newDir {newPath};
            if (!newDir.exists())
            {
                destDir.mkpath(newPath);
            }
            QFile::copy(srcFile, newFile);
        }

        output = dest;
    }
    return output;
}

void ControllManager::extractComponentCode(const QString &name)
{
    qDebug() << "extractComponentCode";

    auto source = QDir::currentPath() + "/components/" + name;

    QDir src {source};
    if (!src.exists())
    {
        DSMessageBox::critical(nullptr, tr("Error"), tr("Source folder does not exists!"));
        return;
    }

    IDEProjectProperties prop;
    prop.exec();

    if (prop.result() == QDialog::Rejected)
    {
        return;
    }

    auto dest = prop.Path();
    auto newname = prop.Name();
    int type = prop.IDEType();
    source = copyFiles(source, dest, QStringList() << "*.h" << "*.hpp" << "*.cpp");

    QString toLaunch;
    switch (type)
    {
        case IDEProjectCreator::VisualStudio:
        {
            std::map<QUuid, QString> proj;
            proj.insert(IDEProjectCreator::createProjectFromExistingCode(newname, source, type));
            toLaunch = IDEProjectCreator::createVSSolution(proj, source, newname);
            break;
        }
        case IDEProjectCreator::QtCreator:
            toLaunch = IDEProjectCreator::createProjectFromExistingCode(newname, source, type).second;
            break;
        default:
            break;
    }
    if (!toLaunch.isEmpty())
        dscontrol::IDEProjectCreator::launchProject(QStringList() << toLaunch, type);
}

void ControllManager::extractProject()
{
    IDEProjectProperties prop;
    prop.exec();

    if (prop.result() == QDialog::Rejected)
    {
        return;
    }

    auto alldest = prop.Path();
    auto newname = prop.Name();
    int type = prop.IDEType();

    std::map<QUuid, QString> projects;

    auto model = QtNodes::Facade::Instance().model();

    std::set<QString> extracted;

    for (const auto &index : model->nodes(QtNodes::Item::NodeType::ComponentNode))
    {
        auto impl = model->nodeImplementation(index);

        auto cimpl = reinterpret_cast<QtNodes::ComponentItemImpl*>(impl);

        auto name = cimpl->componentType();

        if (extracted.find(name) != extracted.end())
            continue;

        auto source = QDir::currentPath() + "/components/" + name;

        QDir src {source};
        if (!src.exists())
        {
            continue;
        }

        auto dest = alldest + ("/" + name);

        source = copyFiles(source, dest, QStringList() << "*.h" << "*.hpp" << "*.cpp");

        projects.insert(IDEProjectCreator::createProjectFromExistingCode(name, source, type));

        extracted.insert(name);
    }

    QString toLaunch;
    switch (type)
    {
        case IDEProjectCreator::VisualStudio:
        {
            toLaunch = IDEProjectCreator::createVSSolution(projects, alldest, newname);
            if (!toLaunch.isEmpty())
                dscontrol::IDEProjectCreator::launchProject(QStringList() << toLaunch, type);
            break;
        }
        case IDEProjectCreator::QtCreator:
        {
            QStringList list;
            for (auto &project : projects)
            {
                list.append(project.second);
            }
            IDEProjectCreator::launchProject(list, type);
        }
            break;
        default:
            break;
    }
}

void ControllManager::generateProjectDocumentation()
{
    ProjectDocumentCreator doc {this};
    DSMessageBox msg {
    DSMessageBox::Icon::Question, tr("Saving project documentation..."), tr("What format do you want to save?")
    , DSMessageBox::StandardButtons::Pdf | DSMessageBox::StandardButtons::Odf | DSMessageBox::StandardButtons::Cancel
    };
    auto res = static_cast<DSMessageBox::StandardButtons>(msg.exec());
    switch (res)
    {
        case DSMessageBox::StandardButtons::Pdf:
        {
            doc.createPdfDocumentation();
            break;
        }
        case DSMessageBox::StandardButtons::Odf:
        {
            doc.createOdfDocumentation();
            break;
        }
        default:
        {
            break;
        }
    }
    return;
}
void ControllManager::UpdatedMonitorData(quint32 ip, QString* info)
{
    QVariant var = QVariant{*info};
    QByteArray json=var.toByteArray();
    QString ipstring=DSServerComm::converseIPStringFromInt32(ip).c_str();
    monitorDataUpdated(ipstring,json);
}
std::string ControllManager::matchIP(std::string ip)
{
    return ip.substr(0,ip.find(":"));
}
