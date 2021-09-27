#pragma once

#include "OutXmlGenerator.hpp"

#include <QFileSystemWatcher>
#include <QMap>
#include <QMessageBox>
#include <QObject>
#include <QTimer>
#include <QtCore>
#include <set>

#include <vector>
#include "network/DSServerComm.h"

struct DeployInfo
{
    QString ip;
    QString appName;
    QString filePath;
    qstring file;
};

class DeployProgressBarDialog;

namespace QtNodes
{
class FlowScene;
class FlowView;
struct CutData;
} // namespace QtNodes

class QAbstractItemModel;
class QAbstractListModel;
class QDomDocument;

namespace dsrepository
{
class RepositoryManager;
class Repository;
} // namespace dsrepository

class MonitorWidget;
class NetworkWidget;
class UpdateWidget;
class DeployWidget;

// namespace network
// {
// class NetworkManager;
// } // namespace network

namespace dscontrol
{
class Project;
class ProjectModel;
class RecentProjectsModel;
class CommunicationModel;
class MessageModel;
class ProjectTreeModel;
class PropertiesTreeModel;
class MimeStoreModel;

using namespace dsrepository;

class ControllManager final : public QObject
{
Q_OBJECT

public:
    explicit ControllManager(QObject* parent = nullptr);
    ~ControllManager() override;

    void setRepository(RepositoryManager* repositoryManager);

    static QString makeTabTitle(const QString &name, int maxSize);

    Project* currentProject();
    const Project* currentProject() const;

    QAbstractListModel* recentProjects() const;

    QAbstractItemModel* projectModel() const;

    ProjectTreeModel* projectTreeModel() const;

    CommunicationModel* communicationModel() const;

    QAbstractItemModel* messageModel() const;

    MimeStoreModel* getMimeStoreModel();

    RepositoryManager* repositoryManager() const;

    void setPlatformLanguage(const QLocale &language);

    QLocale platformLanguage() const;

    void setPropertiesTreeModel(PropertiesTreeModel* model);

    QString copyFiles(const QString &source, const QString &dest, const QStringList &filter = {});

    void viewToClipboard(const QRect &area = QRect {});
    static QPixmap exportToImage(const QRect &area = QRect {});
    const Project* getProjectById(const QUuid &id) const;

    std::set<std::string> getServersIP();
    void createDeployFiles();
    void removeDeployFiles();
    void copyAndReplaceFolderContents(const QString& fromDir, const QString& toDir, bool copyAndRemove = false);
    QString compressFolderToFile(const QString& folder, const QString& file);
    std::string matchIP(std::string ip);


public slots:
    /*!
    \brief start deploy of current project
     */
    void openDeployProject();

    void showRepository();
    void showMonitoring();
    void showNetwork();

    void newProject();
    void openProject();
    void openProject(const QString &projectFile);
    void saveProject();
    QMessageBox::StandardButton saveProjectAs();
    void closeProject();
    void extractProject();

    void aboutApplication();
    void aboutCompany();
    void showHelpCenter();

    void generateCodeComponent();
    void extractComponentCode(const QString &name);
    void generateProjectDocumentation();

    void setSelectedItemsToBackground();
    void setSelectedItemsToForeground();

    static void openConnectionDialog(const QUuid &id);
    void UpdatedMonitorData(quint32 ip, QString* info);

    void upload();
    void deploy();
    
protected:
    static QDir currentPath();
    static QDir currentPathXML();

    void newProject(const QString &projectFile);
    void saveProject(const Project &savingProject, bool isNewProject);
    void loadRecentProjects();
    void saveToRecentProjects(const QString &projectFile);
    void setNewProject(Project* project);

private:
    void removeFromRecentProjects(const QString &fileName);

private slots:
    void fileChangedSlot(const QString &path);
    // void deployError(const QString &ipAddress, const QString &error);
    // void uploadFinished(const QString &ipAddress);
    // void uploadError(const QString &componentName, const QString &ipAddress, const QString &error);

    // void updateProgressBar(const QString &componentName, const QString &ipAddress, qreal value);
    void monitorDataUpdated(QString ip, const QByteArray &json);
    // void projectNameHasChanged(const QString &previousProjectName);
    // void stopApplications(const std::map<QString, QStringList> &applicationsToStop);

signals:
    void projectOpened();
    void projectClosed();
    void fontChanged(const QFont &font, const QColor &color);
    void languageChanged(const QLocale &oldLocale);
    void openComponentInIde(const QString &file);
    void repositoryManagerChanged();

private:
    Project* currentProject_;
    QLocale language_;
    QTranslator translator_;
    ProjectModel* projectModel_;
    ProjectTreeModel* projectTreeModel_;
    RecentProjectsModel* recentProjects_;
    MimeStoreModel* mimeStore_;

    CommunicationModel* communicationModel_;
    MessageModel* messageModel_;

    MonitorWidget* monitorWidget_;
    NetworkWidget* networkWidget_;
    UpdateWidget* updateWidget_;
    DeployWidget* deployWidget_;

    //network::NetworkManager* networkManager_;

    std::map<QString, std::set<DeployObject>> deployDocuments_;
    std::map<QString, std::set<UploadObject>> uploadDocuments_;

    DeployProgressBarDialog* progressBar_;

    PropertiesTreeModel* propertiesTreeModel_;

    RepositoryManager* repositoryManager_;

    QFileSystemWatcher* recentProjectWatcher_;

    dsrepository::Repository* repository_;

    QString userManualFilename_;

    DSServerComm* _serverComm;
    typedef std::tuple<std::string, std::string, std::string> DeployItem;
    typedef std::vector<DeployItem> DeployList;
};
} // namespace dscontrol
