#pragma once

#include "manager/models/ProjectModel.hpp"
#include "ui/platform/SaveAfterCloseDialog/SaveAfterCloseDialog.hpp"
#include "ui/ds_export.h"

#include <QJsonObject>
#include <QLineEdit>
#include <QMainWindow>
#include <QMessageBox>
#include <QToolButton>
#include <QTreeView>

namespace Ui
{
class Platform;
}

namespace QtNodes
{
class FlowScene;
class FlowView;
struct CutData;
namespace Item
{
enum class NodeType;
}
} // namespace QtNodes

namespace dsrepository
{
class Repository;
}

class QDesignerWorkbench;

namespace dscontrol
{
class ControllManager;
class CommunicationsDialog;
class MessagesDialog;
class MiniViewDialog;
class DockWidget;
class DatabaseModule;

namespace CustomEventFilters
{
class ProjectTreeEventFilter;
}

enum class Pages
{
    StartPage,
    ScenePage
};

/*!
\brief Application main window
 */
class DS_EXPORT Platform final : public QMainWindow
{
Q_OBJECT

    enum AlignType
    {
        Left,
        Right,
        Top,
        Bottom,
        CenterVertically,
        CenterHorizontally,
        Width,
        Height,
        Size,
        Grid
    };

    enum RepoTab
    {
        LocalDir,
        DataBase,
        TabCount
    };

    enum class ProjectTreeType
    {
        None,
        ProjectByLayers,
        ProjectByNesting
    };

public:
    /*!
    \param isPlugin set to true if Platform is plugin for some IDE
     */
    explicit Platform(bool isPlugin = false, QWidget* parent = nullptr);

    ~Platform() override;

    /*!
    \brief Set manager which connect to and handle all actions of the
     Platform
    \warning Set ownership of manager. Delete manager_ instance in
     destructor or if set other manager
     */
    void setControllManager(ControllManager* manager);

    ControllManager* controllManager();
    const ControllManager* controllManager() const;

protected:
    void closeEvent(QCloseEvent* event) override;
    void changeEvent(QEvent* event) override;

private:
    void updateProjectStatusBar();
    void prepareComponentsContextMenu();

    void prepareZoomSlider();
    void prepareObjectPropertiesWidget();
    void prepareProjectTrees();
    void prepareProjectTree(ProjectTreeType type);
    void prepareLayersProjectTree();
    void prepareNestingProjectTree();
    void prepareRepositoryWidget();

    void showProjectInfo();
    void showCommunications();
    void showMessages();
    SaveAfterCloseDialog::ExecResult showSavingDialog();

    void connectFlowView(QtNodes::FlowView* view);
    void deleteNode(const QUuid &id);
    void moveAssociatedItems(const QUuid &id, const QPointF &offset);
    void alignItems(AlignType type);
    void alignByGrid();

    QtNodes::Item::NodeType nodeTypeFrom(ProjectModel::MainProjectItems type) const;
    void nestedItemsOption(ProjectTreeType projectTreeType, QMenu* mainMenu, const QModelIndex &index);
    void mainItemsOption(QMenu* mainMenu, ProjectModel::MainProjectItems type);
    QMenu* createProjectTreeContextMenu(ProjectTreeType projectTreeType, const QPoint &point);
    void mimeButtonLambdaBody(QtNodes::Item::NodeType nodeType, const QAction* action, QObject* obj);

    void checkTabsForDeletedNodes();

signals:
    void fontChanged(const QFont &font, const QColor &color);
    void itemSelected(const QUuid &id);

private slots:
    void showRepository();
    void showMiniView();
    void showDesigner();
    void showDesigner(const QString &filename);
    void signIn();
    void signOut();
    void showComponents();
    void closeTab(int indexOfTab);

    void projectItemDoubleClicked(const QModelIndex &index);
    void projectItemClicked(const QUuid &id);

    void loadRecentProjectsToMenu();
    void handleRecentProjectMenuAction();

    void backToProjectPage();

    void projectOpened();
    void projectClosed();

    void projectTreeClicked(const QModelIndex &index);
    void projectTreeCustomContextMenuRequested(const QPoint &point);
    void projectTreeHeaderCustomContextMenuRequested(const QPoint &point);

private:
    Ui::Platform* ui_;
    ControllManager* manager_;
    MessagesDialog* msgDialog_;
    CommunicationsDialog* commDialog_;
    DockWidget* miniViewDock_;
    MiniViewDialog* miniView_;
    dsrepository::Repository* repository_;
    CustomEventFilters::ProjectTreeEventFilter* projectTreeFilter_;
    CustomEventFilters::ProjectTreeEventFilter* newProjectTreeFilter_;

    QDesignerWorkbench* designerWorkbench_;
    int userId_;

    // QtNodes::MiniView *view_;
};

} // namespace dscontrol
