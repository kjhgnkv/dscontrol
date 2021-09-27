#include "Platform.hpp"

#include "CommunicationsDialog/CommunicationsDialog.hpp"
#include "manager/ControllManager.hpp"
#include "MessagesDialog/MessagesDialog.hpp"
#include "MiniViewDialog/MiniViewDialog.hpp"
#include "manager/Project.hpp"
#include "ProjectInfo/ProjectInfo.hpp"
#include "PropertiesDialog/PropertiesDialog.hpp"
#include "custom/MimeButton.hpp"
#include "custom/MimeMenu.hpp"
#include "delegates/RecentLabelDelegate.hpp"

#include "models/MimeGenerator.hpp"
#include "lists/CommunicationList.hpp"
#include "manager/models/CommunicationModel.hpp"
#include "manager/models/DSProxyModel.hpp"

#include "manager/models/MessageModel.hpp"
#include "manager/models/MimeStoreModel.hpp"
#include "manager/models/ProjectModelItem.hpp"
#include "manager/models/ProjectTreeItem.hpp"
#include "manager/models/ProjectTreeModel.hpp"
#include "manager/models/PropertiesItemDelegate.hpp"
#include "manager/models/PropertiesProxyModel.hpp"
#include "manager/models/PropertiesTreeModel.hpp"
#include "manager/models/RecentProjectsModel.hpp"
#include "scenes/FlowScene.hpp"
#include "singletones/ClipboardController.hpp"
#include "singletones/Facade.hpp"
#include "singletones/UndoController.hpp"
#include "singletones/MenuBuilder.h"
#include "views/FlowView.hpp"
#include "DSMessageBox/DSMessageBox.hpp"
#include "DSSettings/DSSettings.hpp"
#include "DockWidget.h"

// TODO if not needed, remove IDEPath from project
//#include "IDEPath.hIDEPath.hpp"
#include "manager/IDEProjectCreator.hpp"

#include "graphicsComposites/universalGraphicsObject/UniversalGraphicsItem.h"
#include "nodes/UniversalGraphicsItemImpl.h"
#include "models/NodesScene.hpp"
#include "event_filters/ProjectTreeEventFilter.h"
#include "event_filters/RepoTabEventFilter.h"
#include "common/globals.h"
#include "repository/ui/Repository.hpp"
#include "widgets/RenameDialog.h"
#include "ui_Platform.h"

#include "repository/manager/RepositoryManager.hpp"
#include "repository/manager/models/DataBaseModel.hpp"
#include "repository/manager/models/LocalFileSystemModel.hpp"

#include "DSDesigner/qdesigner_workbench.h"
#include "DSDesigner/qdesigner_actions.h"

#include "manager/DSConfiguration.h"

#include <QFileDialog>
#include <QClipboard>
#include <QCloseEvent>
#include <QFontComboBox>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QToolButton>
#include <QDebug>
#include "repository/manager/ConfigLoader.hpp"
#include "DatabaseModule/DatabaseModule.h"
#include <Authorization/SignInDialog/SignInDialog.h>
#include <ComponentsDialog/ComponentsDialog.h>

/**
 * This has to be ouside of all namespaces.
 * We set it here (not in main),
 * because when we will compile plugin for qt we will not have main.cpp in sources
**/
void init_resources()
{
    // TODO
    // WAT resources ???
    Q_INIT_RESOURCE(resources);
    Q_INIT_RESOURCE(res_test);
}

using namespace QtNodes;
using namespace dscontrol;
using namespace dsrepository;
using namespace dsrepository::models;
using namespace dsrepository::repository;

using namespace CustomEventFilters;

Platform::Platform(bool isPlugin, QWidget* parent)
    : QMainWindow(parent)
    , ui_ {new Ui::Platform}
    , manager_ {nullptr}
    , msgDialog_ {nullptr}
    , commDialog_ {nullptr}
    , miniViewDock_ {nullptr}
    , miniView_ {nullptr}
    , repository_ {new Repository {this}}
    , projectTreeFilter_ {nullptr}
    , newProjectTreeFilter_ {nullptr}
    , designerWorkbench_ {nullptr}
    , userId_{-1}
{
    init_resources();
    ui_->setupUi(this);

    // TODO
    // иницилиазируем конфигурацию
    DSConfiguration::Instance().init();

    // uncommebnt this
    ui_->dockWidget->hide();
    //TODO remove this widget after testing
    ui_->moduleWidget->hide();

    ui_->menuDesign->installEventFilter(new MimeMenu {ui_->modeToolBar, this});
    ui_->menuDesign->installEventFilter(new MimeMenu {ui_->textToolBar, this});
    ui_->menuDesign->installEventFilter(new MimeMenu {ui_->extToolBar, this});

    auto addSysBtn = ui_->modeToolBar->widgetForAction(ui_->actionSystem);
    auto addSubsysBtn = ui_->modeToolBar->widgetForAction(ui_->actionSubsystem);
    auto addCompBtn = ui_->modeToolBar->widgetForAction(ui_->actionAddComputer);
    auto addAppBtn = ui_->modeToolBar->widgetForAction(ui_->actionAddApplication);
    auto addFrameBtn = ui_->modeToolBar->widgetForAction(ui_->actionInsertFrame);
    auto addTextBtn = ui_->textToolBar->widgetForAction(ui_->actionAddText);
    auto addExternalBtn = ui_->extToolBar->widgetForAction(ui_->actionExternal);

    const auto &systemMimeButtonLambda = [this, action = ui_->actionSystem](QObject* obj)
    {
        mimeButtonLambdaBody(Item::NodeType::SystemNode, action, obj);
    };
    const auto &subSystemMimeButtonLambda = [this, action = ui_->actionSubsystem](QObject* obj)
    {
        mimeButtonLambdaBody(Item::NodeType::SubSystemNode, action, obj);
    };
    const auto &computerMimeButtonLambda = [this, action = ui_->actionAddComputer](QObject* obj)
    {
        mimeButtonLambdaBody(Item::NodeType::ComputerNode, action, obj);
    };
    const auto &applicationMimeButtonLambda = [this, action = ui_->actionAddApplication](QObject* obj)
    {
        mimeButtonLambdaBody(Item::NodeType::ApplicationNode, action, obj);
    };
    const auto &frameMimeButtonLambda = [this, action = ui_->actionInsertFrame](QObject* obj)
    {
        mimeButtonLambdaBody(Item::NodeType::Frame, action, obj);
    };
    const auto &textMimeButtonLambda = [this, action = ui_->actionAddText](QObject* obj)
    {
        mimeButtonLambdaBody(Item::NodeType::Text, action, obj);
    };
    const auto &externalMimeButtonLambda = [this, action = ui_->actionExternal](QObject* obj)
    {
        mimeButtonLambdaBody(Item::NodeType::ExternalNode, action, obj);
    };

    addSysBtn->installEventFilter(new MimeButton {systemMimeButtonLambda, this});
    addSubsysBtn->installEventFilter(new MimeButton {subSystemMimeButtonLambda, this});
    addCompBtn->installEventFilter(new MimeButton {computerMimeButtonLambda, this});
    addAppBtn->installEventFilter(new MimeButton {applicationMimeButtonLambda, this});
    addFrameBtn->installEventFilter(new MimeButton {frameMimeButtonLambda, this});
    addTextBtn->installEventFilter(new MimeButton {textMimeButtonLambda, this});
    addExternalBtn->installEventFilter(new MimeButton {externalMimeButtonLambda, this});

    ui_->tabWidget->setTabsClosable(true);
    connect(ui_->tabWidget, &QTabWidget::tabCloseRequested, this, &Platform::closeTab);

    if (isPlugin)
    {
        ui_->menubar->hide();
    }

    connect(ui_->tabWidget, &QTabWidget::currentChanged, [this](int index)
    {
        if (auto view = dynamic_cast<FlowView*>(this->ui_->tabWidget->widget(index)); view)
        {
            if (miniView_)
            {
                miniView_->setNewContent(view, view->flowScene());
            }
            Facade::Instance().sceneModel()->setTopView(view);

            view->changeScaleForResizableItems(view->getCurrentScale());
            emit view->scaleChanged(view->getCurrentScale());
        }
    });

    // Add custom fonts
    QFontDatabase::addApplicationFont(":/fonts/resources/fonts/SourceSansPro-Black.ttf");
    QFontDatabase::addApplicationFont(":/fonts/resources/fonts/SourceSansPro-BlackItalic.ttf");
    QFontDatabase::addApplicationFont(":/fonts/resources/fonts/SourceSansPro-Bold.ttf");
    QFontDatabase::addApplicationFont(":/fonts/resources/fonts/SourceSansPro-BoldItalic.ttf");
    QFontDatabase::addApplicationFont(":/fonts/resources/fonts/SourceSansPro-ExtraLight.ttf");
    QFontDatabase::addApplicationFont(":/fonts/resources/fonts/SourceSansPro-ExtraLightItalic.ttf");
    QFontDatabase::addApplicationFont(":/fonts/resources/fonts/SourceSansPro-Italic.ttf");
    QFontDatabase::addApplicationFont(":/fonts/resources/fonts/SourceSansPro-Light.ttf");
    QFontDatabase::addApplicationFont(":/fonts/resources/fonts/SourceSansPro-LightItalic.ttf");
    QFontDatabase::addApplicationFont(":/fonts/resources/fonts/SourceSansPro-Regular.ttf");
    QFontDatabase::addApplicationFont(":/fonts/resources/fonts/SourceSansPro-SemiBold.ttf");
    QFontDatabase::addApplicationFont(":/fonts/resources/fonts/SourceSansPro-SemiBoldItalic.ttf");

    connect(&Facade::Instance(), &Facade::itemInfoChanged, this, [this](QRectF rect, QString name)
    {
        const auto &lblProjectInfoText = tr(QString("Object name: %1").arg(name).toStdString().c_str());
        ui_->lblProjectInfo->setText(lblProjectInfoText);

        QPointF rectTopLeft = rect.topLeft();
        const auto &lblItemInfoText = tr(
        QString("Left: %1 x Top: %2 - Width: %3 x Height: %4").arg(rectTopLeft.rx()).arg(rectTopLeft.ry()).arg(
        rect.width()).arg(rect.height()).toStdString().c_str());

        ui_->lblItemInfo->setText(lblItemInfoText);
    });

    connect(&Facade::Instance(), &Facade::nodeRenamed,
            [&tabWidget = ui_->tabWidget, &manager_ = manager_](const NodeIndex &index)
            {
                auto sceneOfItem = Facade::Instance().sceneModel()->getScene(index.id());
                auto nodeName = index.nodeCaption();
                switch (sceneOfItem->deploymentType())
                {
                    case Item::DeploymentType::Internal:
                    {
                        nodeName += tr("(int)");
                        break;
                    }
                    case Item::DeploymentType::External:
                    {
                        nodeName += tr("(ext)");
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }

                const int tabWidgetCount {tabWidget->count()};
                for (int i {}; i < tabWidgetCount; ++i)
                {
                    auto curView = qobject_cast<FlowView*>(tabWidget->widget(i));
                    if (curView && curView->flowScene() == sceneOfItem.get())
                    {
                        tabWidget->setTabText(i, manager_->makeTabTitle(nodeName, 15));
                        tabWidget->setTabToolTip(i, nodeName);
                        return;
                    }
                }
            });

    connect(ui_->actionEnglish, &QAction::triggered, [&manager_ = manager_, this]()
    {
        if (manager_)
        {
            manager_->setPlatformLanguage(QLocale::English);
            auto actions = ui_->menuLanguage->actions();
            for (const auto &action : actions)
            {
                if (action->isChecked())
                {
                    action->setChecked(false);
                }
            }
            ui_->actionEnglish->setChecked(true);
        }
    });
    connect(ui_->actionChinese, &QAction::triggered, [&manager_ = manager_, this]()
    {
        if (manager_)
        {
            manager_->setPlatformLanguage(QLocale::Chinese);
            auto actions = ui_->menuLanguage->actions();
            for (const auto &action : actions)
            {
                if (action->isChecked())
                {
                    action->setChecked(false);
                }
            }
            ui_->actionChinese->setChecked(true);
        }
    });

    connect(ui_->actionOptions, &QAction::triggered, []()
    {
        DSSettings settings;
        settings.exec();
    });

    connect(manager_, &ControllManager::languageChanged, [this](const QLocale &oldLocale)
    {
        switch (oldLocale.language())
        {
            case QLocale::Language::English:
            {
                ui_->actionEnglish->setChecked(false);
                break;
            }
            case QLocale::Language::Chinese:
            {
                ui_->actionChinese->setChecked(false);
                break;
            }
            default:
            {
                return;
            }
        }

        auto newLocale = manager_->platformLanguage();

        switch (newLocale.language())
        {
            case QLocale::Language::English:
            {
                ui_->actionEnglish->setChecked(true);
                break;
            }
            case QLocale::Language::Chinese:
            {
                ui_->actionChinese->setChecked(true);
                break;
            }
            default:
            {
                return;
            }
        }
    });

    prepareZoomSlider();

    ui_->actionUndo->setShortcut(QKeySequence::Undo);
    ui_->actionRedo->setShortcut(QKeySequence::Redo);

    connect(ui_->actionUndo, &QAction::triggered, UndoController::Instance().getUndoAction(this), &QAction::triggered);
    connect(ui_->actionRedo, &QAction::triggered, UndoController::Instance().getRedoAction(this), &QAction::triggered);

    connect(ui_->actionCopy, &QAction::triggered, ClipboardController::Instance().copyAction(), &QAction::triggered);
    connect(ui_->actionCut, &QAction::triggered, ClipboardController::Instance().cutAction(), &QAction::triggered);
    connect(ui_->actionPaste, &QAction::triggered, ClipboardController::Instance().pasteAction(), &QAction::triggered);

    connect(ui_->actionAlignLeft, &QAction::triggered, this, [this]()
    {
        alignItems(AlignType::Left);
    });

    connect(ui_->actionAlignRight, &QAction::triggered, this, [this]()
    {
        alignItems(AlignType::Right);
    });

    connect(ui_->actionAlignTop, &QAction::triggered, this, [this]()
    {
        alignItems(AlignType::Top);
    });

    connect(ui_->actionAlignBottom, &QAction::triggered, this, [this]()
    {
        alignItems(AlignType::Bottom);
    });

    connect(ui_->actionAlignCenterHor, &QAction::triggered, this, [this]()
    {
        alignItems(AlignType::CenterHorizontally);
    });

    connect(ui_->actionAlignCenterVert, &QAction::triggered, this, [this]()
    {
        alignItems(AlignType::CenterVertically);
    });

    connect(ui_->actionAlignWidth, &QAction::triggered, this, [this]()
    {
        alignItems(AlignType::Width);
    });

    connect(ui_->actionAlignHeight, &QAction::triggered, this, [this]()
    {
        alignItems(AlignType::Height);
    });

    connect(ui_->actionAlignSize, &QAction::triggered, this, [this]()
    {
        alignItems(AlignType::Size);
    });

    connect(ui_->actionAlignGrid, &QAction::triggered, this, &Platform::alignByGrid);

    ui_->actionCutterMode->setCheckable(true);

    connect(ui_->actionCutterMode, &QAction::toggled, [](bool checked)
    {
        if (auto view = Facade::Instance().sceneModel()->getTopView())
        {
            view->setCutterMode(checked);
        }
    });
}

Platform::~Platform()
{
    if (projectTreeFilter_)
        delete projectTreeFilter_;

    if (newProjectTreeFilter_)
        delete newProjectTreeFilter_;
    // needed for ds_plugin, because it is static object, and after unload
    // shared library it can not be deleted

    if (repository_)
        delete repository_;

    if (miniViewDock_)
        miniViewDock_->close();

    if (designerWorkbench_ != nullptr)
    {
        designerWorkbench_->deleteLater();
    }

    delete ui_;
}

void Platform::prepareZoomSlider()
{
    ui_->zoomSlider_->setMinimum(MIN_SCALE / 10);
    ui_->zoomSlider_->setMaximum(MAX_SCALE / 10);
    ui_->zoomSlider_->setSingleStep(1);
    ui_->zoomSlider_->setPageStep(1);
    ui_->zoomSlider_->setTickPosition(QSlider::TicksBothSides);

    ui_->btnZoomIn_2->setCursor(Qt::PointingHandCursor);

    ui_->btnZoomOut_2->setCursor(Qt::PointingHandCursor);

    connect(ui_->zoomSlider_, &QSlider::valueChanged, [](const int &value)
    {
        if (auto view = Facade::Instance().sceneModel()->getTopView())
        {
            view->setScale(value * 10);
        }
    });

    connect(ui_->btnZoomIn_2, &QPushButton::clicked, []()
    {
        if (auto view = Facade::Instance().sceneModel()->getTopView())
        {
            view->scaleUp();
        }
    });
    connect(ui_->btnZoomOut_2, &QPushButton::clicked, []()
    {
        if (auto view = Facade::Instance().sceneModel()->getTopView())
        {
            view->scaleDown();
        }
    });
}

void Platform::updateProjectStatusBar()
{
    if (nullptr == manager_)
    {
        return;
    }
    const auto* currentProject = manager_->currentProject();
    if (nullptr == currentProject)
    {
        return;
    }
    const auto &lblProjectInfoText = tr(
    QString("Project: %1 Created: %2 Last modified: %3").arg(currentProject->name()).arg(
    currentProject->creationTimeAsString()).arg(currentProject->lastModifiedAsString()).toStdString().c_str());

    ui_->lblItemInfo->setText(lblProjectInfoText);
}

void Platform::closeEvent(QCloseEvent* event)
{
    if (nullptr != event && nullptr != manager_)
    {
        auto showDialogResult = showSavingDialog();
        switch (showDialogResult)
        {
            case SaveAfterCloseDialog::ExecResult::Save:
            {
                manager_->saveProject();
                manager_->closeProject();
                break;
            }
            case SaveAfterCloseDialog::ExecResult::Cancel:
            {
                event->ignore();
                break;
            }
            case SaveAfterCloseDialog::ExecResult::SaveAs:
            {
                auto saveAsResult = manager_->saveProjectAs();
                switch (saveAsResult)
                {
                    case QMessageBox::StandardButton::Cancel:
                    {
                        event->ignore();
                        break;
                    }
                    case QMessageBox::StandardButton::Save:
                    {
                        manager_->closeProject();
                        break;
                    }
                    default:
                        break;
                }
                break;
            }
            case SaveAfterCloseDialog::ExecResult::DoNotSave:
            {
                manager_->closeProject();
                break;
            }
            default:
                break;
        }

        if (event->isAccepted())
        {
            QMainWindow::closeEvent(event);
        }
        else
        {
            updateProjectStatusBar();
        }
    }
}

void Platform::changeEvent(QEvent* event)
{
    if (event)
    {
        switch (event->type())
        {
            case QEvent::LanguageChange:
            {
                ui_->retranslateUi(this);
                break;
            }
            default:
            {
                break;
            }
        }
        QMainWindow::changeEvent(event);
    }
}

void Platform::setControllManager(ControllManager* manager)
{
    qDebug() << "set ControllManager for Platform";
    if (manager_)
    {
        qDebug() << "remove previous manager";
        // delete previous manager if it was be set
        manager_->deleteLater();
        ui_->projectTree->setModel(nullptr);
        ui_->newProjectTree->setModel(nullptr);
        ui_->tabWidget->clear();
    }
    manager_ = manager;

    prepareObjectPropertiesWidget();

    if (manager_ == nullptr)
    {
        return;
    }

    qDebug() << "connect all signal-slots between ControllManager and "
                "Platform";
    manager_->setParent(this);

    repository_->setRepositoryManger(manager_->repositoryManager());
    prepareRepositoryWidget();

    ui_->projectTree->setModel(manager_->projectModel());
    ui_->newProjectTree->setModel(manager_->projectTreeModel());
    connect(&Facade::Instance(), &Facade::projectChanged, this, [&manager_ = manager_, tree = ui_->newProjectTree]()
    {
        manager_->projectTreeModel()->update();
        tree->expandAll();
    });

    ui_->newProjectTree->setExpandsOnDoubleClick(false);
    // ui_->moduleWidget->setVisible(false);
    ui_->mimeStoreView->setHeaderHidden(true);
    connect(manager_->getMimeStoreModel(), &QAbstractItemModel::rowsInserted,
            [&](const QModelIndex &parent, int first, int last)
            {
                for (; first <= last; ++first)
                {
                    ui_->mimeStoreView->expand(manager_->getMimeStoreModel()->index(first, 0, parent));
                }
            });

    projectTreeFilter_ = new CustomEventFilters::ProjectTreeEventFilter(ui_->projectTree);

    newProjectTreeFilter_ = new CustomEventFilters::ProjectTreeEventFilter(ui_->newProjectTree);

    connect(newProjectTreeFilter_, &CustomEventFilters::ProjectTreeEventFilter::delPressed, this,
            &Platform::deleteNode);

    connect(projectTreeFilter_, &CustomEventFilters::ProjectTreeEventFilter::delPressed, this, &Platform::deleteNode);

    connect(newProjectTreeFilter_, &CustomEventFilters::ProjectTreeEventFilter::selectedChanged,
            [this](const QModelIndex &index)
            {
                auto id = index.data(ProjectModel::Roles::Uid).toUuid();
                projectItemClicked(id);
            });

    connect(projectTreeFilter_, &CustomEventFilters::ProjectTreeEventFilter::selectedChanged, this,
            &Platform::projectTreeClicked);

    ui_->projectTree->installEventFilter(projectTreeFilter_);
    ui_->newProjectTree->installEventFilter(newProjectTreeFilter_);

    // connect action with manager
    connect(ui_->actionNewProject, &QAction::triggered, manager_, [this]()
    {
        qDebug() << "actionNewProject";
        if (manager_ == nullptr)
        {
            return;
        }

        auto showSavingDialogResult = showSavingDialog();
        switch (showSavingDialogResult)
        {
            case SaveAfterCloseDialog::ExecResult::Save:
            {
                manager_->saveProject();
                break;
            }
            case SaveAfterCloseDialog::ExecResult::SaveAs:
            {
                manager_->saveProjectAs();
                break;
            }
            case SaveAfterCloseDialog::ExecResult::Cancel:
            {
                return;
            }
            default:
            {
                break;
            }
        }
        // TODO test and remove
        // manager_->projectTreeModel()->setProject("");
        manager_->newProject();

        updateProjectStatusBar();
    });

    connect(ui_->actionOpenProject, &QAction::triggered, this, [this]()
    {
        if (manager_)
        {
            manager_->openProject();

            if (manager_->currentProject())
            {
                updateProjectStatusBar();
            }
        }
    });

    connect(ui_->actionSaveProject, &QAction::triggered, this, [this]()
    {
        if (manager_)
        {
            manager_->saveProject();
            if (manager_->currentProject())
            {
                updateProjectStatusBar();
            }
        }
    });

    connect(ui_->actionSaveProjectAs, &QAction::triggered, this, [this]()
    {
        if (manager_)
        {
            manager_->saveProjectAs();
            if (manager_->currentProject())
            {
                updateProjectStatusBar();
            }
        }
    });

    connect(&Facade::Instance(), &Facade::extractComponentCode, manager_, &ControllManager::extractComponentCode);

    connect(ui_->actionExportToImage, &QAction::triggered, this, [manager = manager_]()
    {
        if (manager)
        {
            QString fileName = QFileDialog::getSaveFileName(nullptr, tr("Save File"), "", tr("Images (*.png *.jpg)"));

            if (fileName.isEmpty())
                return;

            auto pixmap = manager->exportToImage();

            QFileInfo fileInfo(fileName);
            if (fileInfo.suffix().isEmpty())
                fileName += ".png";

            pixmap.save(fileName);
        }
    });

    connect(ui_->actionExtractProject, &QAction::triggered, manager_, &ControllManager::extractProject);
    connect(ui_->actionProjectDocumentation, &QAction::triggered, manager_,
            &ControllManager::generateProjectDocumentation);

    connect(ui_->actionCopyClipboard, &QAction::triggered, this, [manager = manager_]()
    {
        if (manager)
            manager->viewToClipboard();
    });

    // TODO Deploy
    connect(ui_->actionDeployProject, &QAction::triggered, manager_, &ControllManager::openDeployProject);
    connect(ui_->actionAboutApplication, &QAction::triggered, manager_, &ControllManager::aboutApplication);
    connect(ui_->aboutApplication_, &QPushButton::clicked, manager_, &ControllManager::aboutApplication);
    connect(ui_->aboutApplication_, &QPushButton::pressed, this, [&label = ui_->labelAbout]()
    {
        label->setPixmap(QPixmap {":/images/resources/images/question-pressed.png"});
    });
    connect(ui_->aboutApplication_, &QPushButton::released, this, [&label = ui_->labelAbout]()
    {
        label->setPixmap(QPixmap {":/images/resources/images/question.png"});
    });

    connect(ui_->actionAboutCompany, &QAction::triggered, manager_, &ControllManager::aboutCompany);

    connect(ui_->actionHelpCenter, &QAction::triggered, manager_, &ControllManager::showHelpCenter);

    connect(ui_->actionCommunications, &QAction::triggered, this, &Platform::showCommunications);

    connect(ui_->actionMessages, &QAction::triggered, this, &Platform::showMessages);

    connect(ui_->actionExit, &QAction::triggered, [this]
    {
        close();
    });

    connect(ui_->actionCloseProject, &QAction::triggered, this, [this]()
    {
        if (manager_ == nullptr)
        {
            return;
        }

        auto showSavingDialogResult = showSavingDialog();
        switch (showSavingDialogResult)
        {
            case SaveAfterCloseDialog::ExecResult::Save:
            {
                manager_->projectTreeModel()->setProject("");
                manager_->saveProject();
                manager_->closeProject();
                break;
            }
            case SaveAfterCloseDialog::ExecResult::SaveAs:
            {
                auto saveAsResult = manager_->saveProjectAs();
                switch (saveAsResult)
                {
                    case QMessageBox::StandardButton::Save:
                    {
                        manager_->projectTreeModel()->setProject("");
                        manager_->closeProject();
                        break;
                    }
                    default:
                    {
                        updateProjectStatusBar();
                        break;
                    }
                }
                break;
            }
            case SaveAfterCloseDialog::ExecResult::DoNotSave:
            {
                manager_->projectTreeModel()->setProject("");
                manager_->closeProject();
                break;
            }
            default:
            {
                updateProjectStatusBar();
                break;
            }
        }
    });

    connect(ui_->actionMiniView, &QAction::triggered, this, &Platform::showMiniView);
    connect(ui_->actionRepository, &QAction::triggered, this, &Platform::showRepository);
    connect(ui_->actionStartMonitoring, &QAction::triggered, manager_, &ControllManager::showMonitoring);
    connect(ui_->actionSignIn, &QAction::triggered, this, &Platform::signIn);
    connect(ui_->actionSignOut, &QAction::triggered, this, &Platform::signOut);
    connect(ui_->actionStartNetworkWidget, &QAction::triggered, manager_, &ControllManager::showNetwork);
    connect(ui_->actionStartDesignerWidget, &QAction::triggered, this, QOverload<>::of(&Platform::showDesigner));
    connect(&Facade::Instance(), &Facade::runDesigner, this, QOverload<const QString &>::of(&Platform::showDesigner));
    connect(ui_->actionComponents, &QAction::triggered, this, &Platform::showComponents);

    // connect manager signals
    connect(manager_, &ControllManager::projectOpened, this, &Platform::projectOpened);
    connect(manager_, &ControllManager::projectClosed, this, &Platform::projectClosed);

    ui_->recentProjectsView->setItemDelegate(new RecentLabelDelegate {ui_->recentProjectsView});
    DSProxyModel* recentProxy = new DSProxyModel {ui_->recentProjectsView};
    recentProxy->setSourceModel(manager_->recentProjects());
    auto filter = [](const QModelIndex &index)
    {
        if (!index.isValid() || index.row() >= 4)
        {
            return false;
        }
        return true;
    };
    recentProxy->setFilter(filter, 0);
    ui_->recentProjectsView->setModel(recentProxy);

    connect(ui_->recentProjectsView, &QAbstractItemView::doubleClicked, this,
            [&manager = manager_](const QModelIndex &index)
            {
                if (manager)
                {
                    manager->openProject(index.data(RecentProjectsModel::Role::FullFileName).toString());
                }
            });

    // update menu
    loadRecentProjectsToMenu();

    QPixmap pix {":/images/resources/images/question.png"};
    ui_->labelAbout->setPixmap(pix);

    // TODO here we update menu, when project was be closed. Maybe here
    // is a better way?
    connect(manager_, &ControllManager::projectClosed, this, &Platform::loadRecentProjectsToMenu);

    // Needed for plugin
    // TODO fix that later
    connect(ui_->actionGenerateClass, &QAction::triggered, manager_, &ControllManager::generateCodeComponent);

    connect(ui_->actionBackground, &QAction::triggered, manager_, &ControllManager::setSelectedItemsToBackground);
    connect(ui_->actionForeground, &QAction::triggered, manager_, &ControllManager::setSelectedItemsToForeground);

    prepareComponentsContextMenu();
    prepareProjectTrees();

    ui_->actionSignOut->setVisible(false);
}

ControllManager* Platform::controllManager()
{
    return manager_;
}

const ControllManager* Platform::controllManager() const
{
    return manager_;
}

void Platform::loadRecentProjectsToMenu()
{
    // first clear previous actions
    ui_->menuRecent->clear();

    // read model and generate action from this
    if (manager_ == nullptr)
    {
        return;
    }

    auto recentProjectsModel = manager_->recentProjects();
    if (recentProjectsModel == nullptr)
    {
        return;
    }

    for (int i {}; i < recentProjectsModel->rowCount(); ++i)
    {
        auto index = recentProjectsModel->index(i);
        auto curAction = ui_->menuRecent->addAction(index.data().toString());

        // and connect this
        connect(curAction, &QAction::triggered, this, &Platform::handleRecentProjectMenuAction);
    }
}

void Platform::handleRecentProjectMenuAction()
{
    auto senderAction = qobject_cast<QAction*>(QObject::sender());
    if (senderAction)
    {
        // get project file name from text and open this
        manager_->openProject(senderAction->text());
    }
}

void Platform::projectOpened()
{
    // enable project actions
    ui_->actionCloseProject->setEnabled(true);
    ui_->actionSaveProject->setEnabled(true);
    ui_->actionSaveProjectAs->setEnabled(true);
    ui_->actionDeployProject->setEnabled(true);
    ui_->actionExportToImage->setEnabled(true);
    ui_->actionCopyClipboard->setEnabled(true);
    ui_->actionExtractProject->setEnabled(true);
    ui_->actionProjectDocumentation->setEnabled(true);

    // enable mode actions
    // ui_->actionBrowseMode->setEnabled(true);

    // open project page
    ui_->stackedWidget->setCurrentWidget(ui_->projectPage);
    ui_->projectTree->expandAll();

    auto sceneModel = Facade::Instance().sceneModel();
    auto view = sceneModel->getTopView();
    if (!view)
    {
        view = new FlowView {};
        sceneModel->setTopView(view);
    }

    connectFlowView(view);

    ui_->lblCurrentScale->setText(tr("Zoom: 100%"));
    updateProjectStatusBar();

    view->setFlowScene(Facade::Instance().sceneMain().get());
    ui_->tabWidget->addTab(view, tr("Main Scene"));
    ui_->actionSystem->setEnabled(true);
    ui_->actionSubsystem->setEnabled(true);
    ui_->actionAddComputer->setEnabled(true);
    ui_->actionAddApplication->setEnabled(true);
    ui_->actionExternal->setEnabled(true);
    ui_->actionInsertFrame->setEnabled(true);
    ui_->actionAddText->setEnabled(true);
    ui_->actionCommunications->setEnabled(true);
    ui_->actionMessages->setEnabled(true);
    ui_->actionBackground->setEnabled(true);
    ui_->actionForeground->setEnabled(true);
    ui_->actionAlignLeft->setEnabled(true);
    ui_->actionAlignRight->setEnabled(true);
    ui_->actionAlignTop->setEnabled(true);
    ui_->actionAlignBottom->setEnabled(true);
    ui_->actionAlignCenterVert->setEnabled(true);
    ui_->actionAlignCenterHor->setEnabled(true);
    ui_->actionAlignWidth->setEnabled(true);
    ui_->actionAlignHeight->setEnabled(true);
    ui_->actionAlignSize->setEnabled(true);
    ui_->actionAlignGrid->setEnabled(true);
    ui_->actionCutterMode->setEnabled(true);

    ui_->actionMiniView->setEnabled(true);

    if (manager_)
    {
        manager_->projectTreeModel()->update();
        ui_->newProjectTree->expandAll();

        if (auto projectModel = dynamic_cast<ProjectModel*>(manager_->projectModel()))
        {
            ui_->projectTree->setRootIndex(projectModel->getRootIndex());
        }
        //Component toolbox (deprecated); Uncomment to use component toolbox
        //ui_->mimeStoreView->setModel(manager_->getMimeStoreModel());
    }
}

void Platform::projectClosed()
{
    //Component toolbox (deprecated); Uncomment to use component toolbox
    ui_->mimeStoreView->setModel(nullptr);
    // disable project actions
    ui_->actionCloseProject->setEnabled(false);
    ui_->actionSaveProject->setEnabled(false);
    ui_->actionSaveProjectAs->setEnabled(false);
    ui_->actionExportToImage->setEnabled(false);
    ui_->actionCopyClipboard->setEnabled(false);
    ui_->actionExtractProject->setEnabled(false);
    ui_->actionDeployProject->setEnabled(false);
    ui_->actionProjectDocumentation->setEnabled(false);

    // disable mode actions
    // ui_->actionBrowseMode->setEnabled(false);

    // open main page
    ui_->stackedWidget->setCurrentWidget(ui_->startPage);

    if (miniView_)
    {
        miniView_->close();
    }

    ui_->tabWidget->blockSignals(true);
    ui_->tabWidget->clear();
    ui_->tabWidget->blockSignals(false);

    ui_->actionAddComputer->setEnabled(false);
    ui_->actionAddApplication->setEnabled(false);
    ui_->actionExternal->setEnabled(false);
    ui_->actionInsertFrame->setEnabled(false);
    ui_->actionAddText->setEnabled(false);
    ui_->actionBackground->setEnabled(false);
    ui_->actionForeground->setEnabled(false);
    ui_->actionAlignLeft->setEnabled(false);
    ui_->actionAlignRight->setEnabled(false);
    ui_->actionAlignTop->setEnabled(false);
    ui_->actionAlignBottom->setEnabled(false);
    ui_->actionAlignCenterVert->setEnabled(false);
    ui_->actionAlignCenterHor->setEnabled(false);
    ui_->actionAlignWidth->setEnabled(false);
    ui_->actionAlignHeight->setEnabled(false);
    ui_->actionAlignSize->setEnabled(false);
    ui_->actionAlignGrid->setEnabled(false);
    ui_->actionCutterMode->setEnabled(false);

    ui_->actionMiniView->setEnabled(false);

    if (miniViewDock_)
        miniViewDock_->close();
}

SaveAfterCloseDialog::ExecResult Platform::showSavingDialog()
{
    auto ret = SaveAfterCloseDialog::ExecResult::Unknown;
    if (nullptr != manager_ && nullptr != manager_->currentProject())
    {
        SaveAfterCloseDialog saveDialog(this);
        ret = static_cast<SaveAfterCloseDialog::ExecResult>(saveDialog.exec());
    }
    return ret;
}

void Platform::closeTab(int indexOfTab)
{
    if (indexOfTab < ui_->tabWidget->count() && indexOfTab != 0)
    {
        ui_->tabWidget->removeTab(indexOfTab);
    }
}

void Platform::showCommunications()
{
    if (!commDialog_)
    {
        commDialog_ = new CommunicationsDialog {manager_->communicationModel(), this};
        connect(commDialog_, &CommunicationsDialog::closed, [&commDialog_ = commDialog_]
        {
            commDialog_ = nullptr;
        });
    }
    commDialog_->show();
}

void Platform::showMessages()
{
    qDebug() << "showMessages";

    if (!msgDialog_)
    {
        msgDialog_ = new MessagesDialog {manager_->messageModel(), this};
        connect(msgDialog_, &MessagesDialog::closed, [&msgDialog_ = msgDialog_]
        {
            msgDialog_ = nullptr;
        });
    }
    msgDialog_->show();
}

void Platform::projectItemClicked(const QUuid &id)
{
    emit itemSelected(id);
    auto sceneModel = Facade::Instance().sceneModel();
    if (sceneModel == nullptr)
    {
        return;
    }

    for (const auto &scene : sceneModel->allScenes())
    {
        if (const auto &item = scene->graphicsItem(id); item && item->flags().testFlag(QGraphicsItem::ItemIsSelectable))
        {
            auto itemScene = item->scene();
            if (itemScene)
            {
                itemScene->clearSelection();
            }
            item->setSelected(true);
            break;
        }
    }
}

void Platform::projectItemDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid())
    {
        return;
    }

    switch (index.data(ProjectModel::Roles::ProjectItem).value<int>())
    {
        case ProjectModel::MainProjectItems::NestedItem:
        {
            if (FlowScene* sceneOfItem = index.data(ProjectModel::Roles::FlowScene).value<FlowScene*>();
            sceneOfItem->type() != Item::NodeType::ComponentNode && sceneOfItem->type() != Item::NodeType::ExternalNode)
            {
                // when we try open new view for nested scene we have to
                // check: maybe it already opened
                for (int i {}; i < ui_->tabWidget->count(); ++i)
                {
                    auto curView = qobject_cast<FlowView*>(ui_->tabWidget->widget(i));
                    if (curView && curView->flowScene() == sceneOfItem)
                    {
                        // if scene already have view, then just open it
                        ui_->tabWidget->setCurrentIndex(i);
                        ui_->stackedWidget->setCurrentWidget(ui_->projectPage);
                        return;
                    }
                }
                // otherwise create new view
                FlowView* newView = new FlowView;

                connectFlowView(newView);

                QString name = index.data().toString();

                switch (sceneOfItem->deploymentType())
                {
                    case Item::DeploymentType::Internal:
                    {
                        name += tr("(int)");
                        break;
                    }
                    case Item::DeploymentType::External:
                    {
                        name += tr("(ext)");
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }

                QString tabTitle = manager_->makeTabTitle(name, 15);

                newView->setFlowScene(sceneOfItem);
                int tabIdx = ui_->tabWidget->addTab(newView, tabTitle);
                ui_->tabWidget->setTabToolTip(tabIdx, name);

                ui_->tabWidget->setCurrentWidget(newView);
                ui_->stackedWidget->setCurrentWidget(ui_->projectPage);

                // when we delete some item, then view have to be deleted
                // also
                connect(sceneOfItem, &QObject::destroyed, newView, &QWidget::deleteLater, Qt::UniqueConnection);
            }

            break;
        }
            // no need delete, because have flag wa_delete_on_close
        case ProjectModel::MainProjectItems::Communications:
        {
            showCommunications();
            break;
        }
        case ProjectModel::MainProjectItems::Messages:
        {
            showMessages();
            break;
        }
        default:
            break;
    }
}

void Platform::backToProjectPage()
{
    ui_->stackedWidget->setCurrentWidget(ui_->projectPage);
}

void Platform::showRepository()
{
    if (repository_->isVisible())
    {
        repository_->close();
        return;
    }
    repository_->show();
}

void Platform::showMiniView()
{
    if (miniViewDock_ != nullptr)
    {
        miniViewDock_->close();
    }

    auto view = dynamic_cast<FlowView*>(ui_->tabWidget->currentWidget());
    if (view == nullptr)
    {
        return;
    }

    auto scene = view->flowScene();
    if (scene == nullptr)
    {
        return;
    }

    miniView_ = new MiniViewDialog(view, scene, this);
    miniView_->setNewContent(view, scene);

    // Make mini view as dock widget
    miniViewDock_ = new DockWidget(tr("Pan&zoom"), this);
    miniViewDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    miniViewDock_->setWidget(miniView_);
    addDockWidget(Qt::RightDockWidgetArea, miniViewDock_);

    ui_->actionMiniView->setChecked(true);

    connect(miniViewDock_, &DockWidget::closed, [&]()
    {
        miniView_->deleteLater();
        miniView_ = nullptr;
        miniViewDock_->deleteLater();
        miniViewDock_ = nullptr;
        ui_->actionMiniView->setChecked(false);
    });
}

void Platform::showDesigner()
{
    if (designerWorkbench_ != nullptr)
    {
        designerWorkbench_->deleteLater();
    }

    designerWorkbench_ = new QDesignerWorkbench();
}

void Platform::showDesigner(const QString &filename)
{
    if (designerWorkbench_ != nullptr)
    {
        designerWorkbench_->deleteLater();
    }

    designerWorkbench_ = new QDesignerWorkbench();
    QString* errorMsg = new QString {};
    designerWorkbench_->openForm(filename, errorMsg);
    delete errorMsg;
}

void Platform::signIn()
{
    SignInDialog dialog;

    if (dialog.exec() == QDialog::DialogCode::Accepted)
    {
        userId_ = dialog.userId();
        auto userName = dialog.userName();

        ui_->actionSignOut->setText("Sign Out [" + userName + "]");

        ui_->actionSignIn->setVisible(false);
        ui_->actionSignOut->setVisible(true);
    }
}

void Platform::signOut()
{
    userId_ = -1;

    ui_->actionSignIn->setVisible(true);
    ui_->actionSignOut->setVisible(false);
}

void Platform::showComponents()
{
    ComponentsDialog().exec();
}

void Platform::prepareComponentsContextMenu()
{
    connect(ui_->mimeStoreView, &QTreeView::customContextMenuRequested, this,
            [&manager = manager_, mimeStoreView = ui_->mimeStoreView, this](const QPoint &point)
            {
                if (manager == nullptr)
                {
                    return;
                }

                QModelIndex index = mimeStoreView->indexAt(point);
                if (!index.isValid())
                {
                    return;
                }

                MimeStoreModel* model = manager->getMimeStoreModel();
                if (!model || model->indexType(index) == 0)
                {
                    return;
                }

                QMenu menu;
                menu.addAction(QIcon(":/componentMenu/add.png"), QAction::tr("Add"));
                menu.addAction(QIcon(":/componentMenu/update.png"), QAction::tr("Update"));
                menu.addAction(QAction::tr("Remove"));

                auto propsAction = menu.addAction(QAction::tr("Properties"));

                // Properties event
                connect(propsAction, &QAction::triggered, [model, index]()
                {
                    if (model == nullptr)
                    {
                        return;
                    }

                    const QJsonObject data = model->storedItemData(index);
                    PropertiesDialog dialog(data);
                    dialog.exec();
                });

                auto extractAction = menu.addAction(QAction::tr("Extract code"));

                connect(extractAction, &QAction::triggered, this, [model, index, this]
                {
                    if (model == nullptr || manager_ == nullptr)
                    {
                        return;
                    }

                    const QJsonObject data = model->storedItemData(index);
                    auto name = data["caption"].toString();
                    manager_->extractComponentCode(name);
                });

                menu.exec(mimeStoreView->viewport()->mapToGlobal(point));
            });
}

void Platform::showProjectInfo()
{
    if (nullptr == manager_)
    {
        return;
    }
    const auto* currentProject = manager_->currentProject();
    if (nullptr == currentProject)
    {
        return;
    }
    std::vector<std::pair<QString, QString>> info;
    info.push_back(std::make_pair(tr("Project name"), currentProject->name()));
    info.push_back(std::make_pair(tr("Project ID"), currentProject->uuid().toString()));
    info.push_back(std::make_pair(tr("Author"), currentProject->creator()));
    info.push_back(std::make_pair(tr("Date created"), currentProject->creationTimeAsString()));
    info.push_back(std::make_pair(tr("Last modified"), currentProject->lastModifiedAsString()));
    info.push_back(std::make_pair(tr("Path"), currentProject->fileName()));
    ProjectInfo showInfo(info);
    showInfo.exec();
}

void Platform::prepareObjectPropertiesWidget()
{
    auto model = new PropertiesTreeModel {this};

    if (manager_)
    {
        manager_->setPropertiesTreeModel(model);
    }

    PropertiesProxyModel* proxy = new PropertiesProxyModel {this};
    connect(ui_->propFilterEdit, &QLineEdit::textChanged,
            [proxy, tree = ui_->objPropertiesTree, edit = ui_->propFilterEdit](const QString &text)
            {
                if (text != tr("Filter by name"))
                {
                    proxy->setFilterStr(edit->text());
                    tree->expandAll();
                }
                else
                {
                    proxy->setFilterStr("");
                    tree->expandAll();
                }
            });

    model->removeColumn(2);

    proxy->setFilterStr("");
    proxy->setSourceModel(model);
    ui_->objPropertiesTree->setModel(proxy);
    ui_->objPropertiesTree->setItemDelegate(new PropertiesDelegate {this});
    ui_->objPropertiesTree->setRootIsDecorated(false);
    ui_->objPropertiesTree->expandAll();
    ui_->objPropertiesTree->setContentsMargins(20, 20, 20, 20);

    // Set font for tree view
    QFont font = ui_->objPropertiesTree->font();
    QString fontFam = font.family();
    font.setPixelSize(12);
    font.setWeight(QFont::DemiBold);
    ui_->objPropertiesTree->setFont(font);

    // Make small left padding for child items
    ui_->objPropertiesTree->setIndentation(0);

    connect(&Facade::Instance(), &Facade::itemPropertiesChanged, [model, tree = ui_->objPropertiesTree]()
    {
        if (model)
        {
            model->update();
        }
        tree->expandAll();
    });

    connect(&Facade::Instance(), &Facade::itemSelected,
            [model, tree = ui_->objPropertiesTree, this](bool selected, const QUuid &id)
            {
                if (model)
                {
                    if (selected)
                    {
                        model->setup(id);
                    }
                    else
                    {
                        model->setup(QUuid {});
                        updateProjectStatusBar();
                    }
                }
                tree->expandAll();
            });

    connect(this, &Platform::itemSelected, [model, tree = ui_->objPropertiesTree, this](const QUuid &id)
    {
        if (model)
        {
            auto nodesModel = Facade::Instance().model();
            if (nodesModel->nodeDeploymentType(nodesModel->nodeIndex(id)) == Item::DeploymentType::None)
            {
                model->setup({});
            }
            else
            {
                model->setup(id);
                updateProjectStatusBar();
            }
        }
        tree->expandAll();
    });

    connect(&Facade::Instance(), &Facade::itemRemoved, [model]()
    {
        if (model)
        {
            model->setup({});
        }
    });

    model->setup(QUuid {});

    connect(ui_->propFilterEdit, &QLineEdit::textChanged, this, [button = ui_->searchButton](const QString &text)
    {
        if (text.isEmpty() || text == tr(""))
        {
            button->setIcon(QIcon {":/images/resources/icons/search.svg"});
        }
        else
        {
            button->setIcon(QIcon {":/images/resources/icons/clear-filter.svg"});
        }
    });

    connect(ui_->searchButton, &QToolButton::clicked, this, [button = ui_->searchButton, edit = ui_->propFilterEdit]()
    {
        if (auto text = edit->text(); !text.isEmpty())
        {
            edit->clearFocus();
            edit->setText("");
            button->setIcon(QIcon {":/images/resources/icons/search.svg"});
        }
    });
}

void Platform::prepareProjectTrees()
{
    prepareLayersProjectTree();
    prepareNestingProjectTree();
}

void Platform::projectTreeClicked(const QModelIndex &index)
{
    switch (index.data(ProjectModel::Roles::ProjectItem).value<int>())
    {
        case ProjectModel::MainProjectItems::NestedItem:
        {
            if (manager_ == nullptr)
            {
                break;
            }
            auto projectModel = dynamic_cast<ProjectModel*>(manager_->projectModel());
            if (nullptr != projectModel)
            {
                auto projectItem = projectModel->getProjectItem(index);
                if (nullptr != projectItem)
                {
                    auto id = projectItem->id();
                    if (!id.isNull())
                    {
                        projectItemClicked(id);
                        checkTabsForDeletedNodes();
                    }
                }
            }
            break;
        }
        default:
        {
            projectItemClicked({});
            break;
        }
    }
}

void Platform::projectTreeCustomContextMenuRequested(const QPoint &point)
{
    const auto* senderProjectTree = sender();
    ProjectTreeType projectTreeType {ProjectTreeType::None};
    if (senderProjectTree == ui_->projectTree)
    {
        projectTreeType = ProjectTreeType::ProjectByLayers;
        checkTabsForDeletedNodes();
    }
    else if (senderProjectTree == ui_->newProjectTree)
    {
        projectTreeType = ProjectTreeType::ProjectByNesting;
    }

    QMenu* contextMenu = createProjectTreeContextMenu(projectTreeType, point);
    if (nullptr != contextMenu)
    {
        contextMenu->exec(QCursor::pos());
        contextMenu->deleteLater();
    }
}

void Platform::projectTreeHeaderCustomContextMenuRequested(const QPoint &)
{
    QMenu* renameProjectMenu = new QMenu;
    auto changeProjectNameAction = renameProjectMenu->addAction(QAction::tr("Change project name"));

    QObject::connect(changeProjectNameAction, &QAction::triggered, [this]()
    {
        auto project = manager_->currentProject();
        if (nullptr == project)
        {
            return;
        }

        RenameDialog renameProjectDialog {project->name()};
        bool hasRenamed = renameProjectDialog.exec() == QDialog::DialogCode::Accepted;
        if (hasRenamed)
        {
            project->setName(renameProjectDialog.currentName());
        }
    });

    renameProjectMenu->exec(QCursor::pos());
    renameProjectMenu->deleteLater();
}

void Platform::connectFlowView(FlowView* view)
{
    connect(view, &FlowView::itemsUnselected, this, &Platform::updateProjectStatusBar);
    connect(view, &FlowView::scaleChanged, this,
            [label = ui_->lblCurrentScale, slider = ui_->zoomSlider_, view](qreal scale)
            {
                if (Facade::Instance().sceneModel()->getTopView() == view)
                {
                    label->setText("Zoom: " + QString::number(static_cast<int>(scale)) + "%");
                    slider->setValue(static_cast<int>(scale / 10));
                }
            });
    connect(view, &FlowView::areaCutted, this, [manager = manager_](const QRect &area)
    {
        if (!manager)
        {
            return;
        }

        DSMessageBox msg {
        DSMessageBox::Icon::Question, tr("Copy selected area"), tr("Where do you want to copy to?")
        , DSMessageBox::StandardButtons::File | DSMessageBox::StandardButtons::Clipboard | DSMessageBox::StandardButtons::Cancel
        };

        auto res = static_cast<DSMessageBox::StandardButtons>(msg.exec());
        switch (res)
        {
            case DSMessageBox::StandardButtons::Clipboard:
            {
                manager->viewToClipboard(area);
                break;
            }
            case DSMessageBox::StandardButtons::File:
            {
                QString fileName = QFileDialog::getSaveFileName(nullptr, tr("Save File"), "",
                                                                tr("Images (*.png *.jpg)"));

                if (fileName.isEmpty())
                {
                    return;
                }

                auto pixmap = manager->exportToImage(area);

                QFileInfo fileInfo(fileName);
                if (fileInfo.suffix().isEmpty())
                {
                    fileName += ".png";
                }

                pixmap.save(fileName);
                break;
            }
            default:
            {
                break;
            }
        }
    });
}

void Platform::deleteNode(const QUuid &id)
{
    auto dataNodesModel = Facade::Instance().model();
    auto nodeIndex = dataNodesModel->nodeIndex(id);
    dataNodesModel->removeNode(nodeIndex);
    if (manager_)
    {
        manager_->projectTreeModel()->update();
    }
    ui_->newProjectTree->expandAll();
}

void Platform::moveAssociatedItems(const QUuid &id, const QPointF &offset)
{
    auto sceneModel = Facade::Instance().sceneModel();
    if (nullptr == sceneModel)
    {
        return;
    }

    auto topScene = sceneModel->getTopScene();
    if (nullptr == topScene)
    {
        return;
    }

    for (auto item : topScene->associatedItems(id))
    {
        if (auto it = dynamic_cast<ResizableItem*>(topScene->graphicsItem(item)))
        {
            /**
             * FIXME
             * Change on moveBy(offset.x(), offset.y())
            **/
            it->setPos(it->scenePos() + offset);
        }
    }
}

// TODO Dangerous code; if because of something
// item will not be erased from selectedAlignableItems_ list on item delete,
// it will cause crash, may be try-catch or uid instead of ResizableItem*?

void Platform::alignItems(AlignType type)
{
    auto scene = Facade::Instance().sceneModel()->getSelectedScene();
    if (scene == nullptr)
    {
        return;
    }

    auto items = scene->selectedAlignableItems();
    if (items.size() < 2)
    {
        return;
    }

    auto alignItem = items.back();

    UndoController::Instance().begin("Platform: align items");
    for (auto item : items)
    {
        auto resItem = dynamic_cast<ResizableItem*>(item);
        // TODO !
        // if (!resItem || resItem->isMainForAlignment())
        if (!resItem || !resItem->isMainForAlignment())
        {
            continue;
        }

        switch (type)
        {
            case Left:
            case Right:
            case Top:
            case Bottom:
            case CenterHorizontally:
            case CenterVertically:
            {
                QPointF pos;
                double value;
                auto posBefore = resItem->scenePos();
                switch (type)
                {
                    case Left:
                    {
                        pos = {alignItem->scenePos().rx(), posBefore.ry()};
                        break;
                    }
                    case Right:
                    {
                        value = alignItem->scenePos().rx() + alignItem->boundingRect().width();
                        pos = {value - resItem->boundingRect().width(), posBefore.ry()};
                        break;
                    }
                    case Top:
                    {
                        pos = {posBefore.rx(), alignItem->scenePos().ry()};
                        break;
                    }
                    case Bottom:
                    {
                        value = alignItem->scenePos().ry() + alignItem->boundingRect().height();
                        pos = {posBefore.rx(), value - resItem->boundingRect().height()};
                        break;
                    }
                    case CenterHorizontally:
                    {
                        value = alignItem->scenePos().ry() + alignItem->boundingRect().height() / 2;
                        pos = {posBefore.rx(), value - resItem->boundingRect().height() / 2};
                        break;
                    }
                    case CenterVertically:
                    {
                        value = alignItem->scenePos().rx() + alignItem->boundingRect().width() / 2;
                        pos = {value - resItem->boundingRect().width() / 2, posBefore.ry()};
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
                resItem->setPos(pos);

                auto offset = resItem->scenePos() - posBefore;

                moveAssociatedItems(resItem->id(), offset);

                break;
            }
            case Width:
            case Height:
            case Size:
            {
                QSizeF size;
                switch (type)
                {
                    case Width:
                    {
                        size = {alignItem->boundingRect().width(), resItem->boundingRect().height()};
                        break;
                    }
                    case Height:
                    {
                        size = {resItem->boundingRect().width(), alignItem->boundingRect().height()};
                        break;
                    }
                    case Size:
                    {
                        size = {alignItem->boundingRect().width(), alignItem->boundingRect().height()};
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
                resItem->resize(size);

                break;
            }
        }
    }

    UndoController::Instance().end();
}

void Platform::alignByGrid()
{
    auto model = Facade::Instance().sceneModel();
    if (model)
    {
        auto scene = model->getSelectedScene();
        if (scene)
        {
            scene->alignByGrid();
        }
    }
}

void Platform::checkTabsForDeletedNodes()
{
    for (auto i = 0; i < ui_->tabWidget->count(); i++)
    {
        auto widget = ui_->tabWidget->widget(i);
        auto flowView = dynamic_cast<QtNodes::FlowView*>(widget);
        if (flowView && !flowView->flowScene())
        {
            ui_->tabWidget->removeTab(i--);
        }
    }
}

void Platform::prepareProjectTree(Platform::ProjectTreeType type)
{
    switch (type)
    {
        case Platform::ProjectTreeType::ProjectByLayers:
        {
            prepareLayersProjectTree();
            break;
        }
        case Platform::ProjectTreeType::ProjectByNesting:
        {
            prepareNestingProjectTree();
            break;
        }
        default:
        {
            break;
        }
    }
}

void Platform::prepareLayersProjectTree()
{
    ui_->projectTree->setContextMenuPolicy(Qt::CustomContextMenu);
    ui_->projectTree->setDragEnabled(true);
    ui_->projectTree->setDropIndicatorShown(true);
    ui_->projectTree->setAcceptDrops(false);
    auto projectTreeHeader = ui_->projectTree->header();
    projectTreeHeader->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui_->projectTree, &QTreeView::activated, this, &Platform::projectItemDoubleClicked);
    connect(ui_->projectTree, &QTreeView::customContextMenuRequested, this,
            &Platform::projectTreeCustomContextMenuRequested);
    connect(ui_->projectTree, &QTreeView::clicked, this, &Platform::projectTreeClicked);
    connect(projectTreeHeader, &QHeaderView::customContextMenuRequested, this,
            &Platform::projectTreeHeaderCustomContextMenuRequested);
}

void Platform::prepareNestingProjectTree()
{
    ui_->newProjectTree->setContextMenuPolicy(Qt::CustomContextMenu);
    ui_->newProjectTree->setDragEnabled(true);
    ui_->newProjectTree->setDropIndicatorShown(true);
    ui_->newProjectTree->setAcceptDrops(false);
    auto newProjectTreeHeader = ui_->newProjectTree->header();
    newProjectTreeHeader->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui_->newProjectTree, &QTreeView::activated, this, &Platform::projectItemDoubleClicked);
    connect(ui_->newProjectTree, &QTreeView::customContextMenuRequested, this,
            &Platform::projectTreeCustomContextMenuRequested);
    connect(ui_->newProjectTree, &QTreeView::clicked, [this](const QModelIndex &index)
    {
        auto id = index.data(ProjectModel::Roles::Uid).toUuid();
        projectItemClicked(id);
    });
    connect(newProjectTreeHeader, &QHeaderView::customContextMenuRequested, this,
            &Platform::projectTreeHeaderCustomContextMenuRequested);
}

Item::NodeType Platform::nodeTypeFrom(ProjectModel::MainProjectItems type) const
{
    switch (type)
    {
        case ProjectModel::MainProjectItems::Systems:
        {
            return Item::NodeType::SystemNode;
        }
        case ProjectModel::MainProjectItems::SubSystems:
        {
            return Item::NodeType::SubSystemNode;
        }
        case ProjectModel::MainProjectItems::Computers:
        {
            return Item::NodeType::ComputerNode;
        }
        case ProjectModel::MainProjectItems::Applications:
        {
            return Item::NodeType::ApplicationNode;
        }
        default:
        {
            return Item::NodeType::OtherType;
        }
    }
}

void Platform::nestedItemsOption(ProjectTreeType projectTreeType, QMenu* mainMenu, const QModelIndex &index)
{
    if (nullptr == manager_ || nullptr == mainMenu)
    {
        return;
    }
    const auto dataNodesModel = Facade::Instance().model();
    if (nullptr == dataNodesModel)
    {
        return;
    }
    const auto projectModel = dynamic_cast<ProjectTreeModel*>(manager_->projectTreeModel());
    if (nullptr == projectModel)
    {
        return;
    }
    const auto* projectItem = projectModel->getProjectItem(index);
    if (nullptr == projectItem)
    {
        return;
    }
    const auto &nodeIndex = dataNodesModel->nodeIndex(projectItem->id());
    if (!nodeIndex.isValid())
    {
        return;
    }

    auto renameItemAction = mainMenu->addAction(QAction::tr("Rename"));
    connect(renameItemAction, &QAction::triggered, [nodeIndex]()
    {
        Facade::Instance().renameNode(nodeIndex);
    });

    auto deleteItemAction = mainMenu->addAction(QIcon(":/delete.png"), QAction::tr("Delete"));
    connect(deleteItemAction, &QAction::triggered,
            [projectTreeType, nodeIndex, projectModel, dataNodesModel, tree = ui_->newProjectTree]()
            {
                dataNodesModel->removeNode(nodeIndex);
                if (ProjectTreeType::ProjectByNesting == projectTreeType)
                {
                    projectModel->update();
                    tree->expandAll();
                }
            });
}

void Platform::mainItemsOption(QMenu* mainMenu, ProjectModel::MainProjectItems type)
{
    if (nullptr == mainMenu)
    {
        return;
    }

    auto newItemAction = mainMenu->addAction(QAction::tr("Add new"));
    connect(newItemAction, &QAction::triggered, [type, this]()
    {
        const auto nodeType = nodeTypeFrom(type);
        if (nodeType != Item::NodeType::SystemNode)
        {
            Facade::Instance().createNode(nodeType, Item::DeploymentType::None, {}, {});
            return;
        }

        const auto deployType = MenuBuilder::createDeployTypeMenu();
        if (deployType != Item::DeploymentType::None)
        {
            auto idx = Facade::Instance().createNode(Item::NodeType::SystemNode, deployType, {}, {});
            Facade::Instance().createComposite(idx, {1, 1}, Facade::Instance().sceneMain()->id());
        }
    });
}

QMenu* Platform::createProjectTreeContextMenu(Platform::ProjectTreeType projectTreeType, const QPoint &point)
{
    if (ProjectTreeType::None == projectTreeType)
    {
        return nullptr;
    }
    QModelIndex index = ui_->projectTree->indexAt(point);
    if (!index.isValid())
    {
        return nullptr;
    }
    auto type = index.data(ProjectModel::Roles::ProjectItem).value<ProjectModel::MainProjectItems>();

    QMenu* mainMenu = new QMenu;

    switch (type)
    {
        case ProjectModel::MainProjectItems::NestedItem:
        {
            nestedItemsOption(projectTreeType, mainMenu, index);
            break;
        }

        case ProjectModel::MainProjectItems::Systems:
        case ProjectModel::MainProjectItems::SubSystems:
        case ProjectModel::MainProjectItems::Computers:
        case ProjectModel::MainProjectItems::Applications:
        {
            if (ProjectTreeType::ProjectByLayers == projectTreeType)
            {
                mainItemsOption(mainMenu, type);
            }
            break;
        }

        default:
        {
            break;
        }
    }

    auto projectInfoAction = mainMenu->addAction(QAction::tr("Project info"));
    connect(projectInfoAction, &QAction::triggered, this, &Platform::showProjectInfo);

    return mainMenu;
}

void Platform::mimeButtonLambdaBody(Item::NodeType nodeType, const QAction* action, QObject* obj)
{
    if (nullptr == action || !action->isEnabled())
    {
        return;
    }
    switch (nodeType)
    {
        case Item::NodeType::SystemNode:
        {
            return MimeGenerator::generateSystemDragMime(obj);
        }
        case Item::NodeType::SubSystemNode:
        {
            return MimeGenerator::generateSubsystemDragMime(obj);
        }
        case Item::NodeType::ComputerNode:
        {
            return MimeGenerator::generateComputerDragMime(obj);
        }
        case Item::NodeType::ApplicationNode:
        {
            return MimeGenerator::generateApplicationDragMime(obj);
        }
        case Item::NodeType::ExternalNode:
        {
            return MimeGenerator::generateExternalDragMime(obj);
        }
        case Item::NodeType::Frame:
        {
            return MimeGenerator::generateFrameDragMime(obj);
        }
        case Item::NodeType::Text:
        {
            return MimeGenerator::generateTextDragMime(obj);
        }
        default:
        {
            break;
        }
    }
}

void Platform::prepareRepositoryWidget()
{
    if (!manager_)
    {
        return;
    }
    auto dirmodel = manager_->repositoryManager()->fileSystemModel();
    if (!dirmodel)
    {
        return;
    }

    dirmodel->setNameFilterDisables(false);
    connect(ui_->repoSearchEdit, &QLineEdit::textChanged, [dirmodel, tree = ui_->locDirTreeView](const QString &text)
    {
        dirmodel->setNameFilters({"*" + text + "*"});
        tree->expandAll();
    });

    connect(ui_->btnRepoSearch, &QPushButton::clicked, [edit = ui_->repoSearchEdit]
    {
        edit->setText("");
    });

    ui_->locDirTreeView->setModel(dirmodel);
    ui_->locDirTreeView->setRootIndex(dirmodel->index(manager_->repositoryManager()->fileSystemModel()->rootPath()));
    ui_->locDirTreeView->setSelectionModel(manager_->repositoryManager()->topSelectionModel());

    ui_->remDBTableView->setModel(manager_->repositoryManager()->remoteDataBaseModel());
    ui_->remDBTableView->setSelectionModel(manager_->repositoryManager()->topSelectionModel());

    ui_->repoTabWidget->tabBar()->setMouseTracking(true);
    ui_->repoTabWidget->tabBar()->setAcceptDrops(true);
    ui_->repoTabWidget->tabBar()->installEventFilter(new RepoTabEventFilter(ui_->repoTabWidget->tabBar()));

    ui_->locDirTreeView->setDragEnabled(true);
    ui_->locDirTreeView->setDropIndicatorShown(true);
    ui_->locDirTreeView->viewport()->setAcceptDrops(true);
    ui_->locDirTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui_->locDirTreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui_->locDirTreeView->setDragDropMode(QAbstractItemView::DragDrop);
    ui_->locDirTreeView->setAcceptDrops(true);
    ui_->locDirTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui_->locDirTreeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui_->remDBTableView->setDragEnabled(true);
    ui_->remDBTableView->setDropIndicatorShown(true);
    ui_->remDBTableView->viewport()->setAcceptDrops(true);
    ui_->remDBTableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui_->remDBTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui_->remDBTableView->setDragDropMode(QAbstractItemView::DragDrop);
    ui_->remDBTableView->setAcceptDrops(true);
    ui_->remDBTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui_->remDBTableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    connect(ui_->addComponentBtn, &QPushButton::pressed, this, [&manager = manager_, tabWidget = ui_->repoTabWidget]()
    {
        switch (tabWidget->currentIndex())
        {
            case RepoTab::LocalDir:
            {
                //TODO check this! it seems to cause multiple copying or crash
                //manager->repositoryManager()->addNewComponent(ModelType::FileSystem);
                break;
            }
            case RepoTab::DataBase:
            {
                manager->repositoryManager()->addNewComponent(ModelType::DataBase);
                break;
            }
            default:
            {
                break;
            }
        }
    });

    connect(ui_->remComponentBtn, &QPushButton::pressed, this, [&manager = manager_, tabWidget = ui_->repoTabWidget]()
    {
        switch (tabWidget->currentIndex())
        {
            case RepoTab::LocalDir:
            {
                manager->repositoryManager()->removeComponent(ModelType::FileSystem);
                break;
            }
            case RepoTab::DataBase:
            {
                manager->repositoryManager()->removeComponent(ModelType::DataBase);
                break;
            }
            default:
            {
                break;
            }
        }
    });

    // connect(ui_->compareByBytesBtn, &QPushButton::pressed, manager_->repositoryManager(),
    //         &RepositoryManager::compareByBytes);

    connect(manager_->repositoryManager(), &RepositoryManager::bytesCompare, [this](auto res, auto filenames)
    {
        QString pattern {"%1 is %2 "};
        QString names;

        for (const auto &name : filenames)
        {
            names.append(name.name_ + ", ");
        }

        QString result = res ? "equal" : "not equal";

        pattern = pattern.arg(names).arg(result);
        DSMessageBox::information(this, "Info", pattern);
    });

    connect(ui_->locDirTreeView, &QTreeView::customContextMenuRequested, this,
            [&manager = manager_, locDirTreeView = ui_->locDirTreeView, this](const QPoint &point)
            {
                QModelIndex index = locDirTreeView->indexAt(point);
                if (!index.isValid())
                {
                    return;
                }
                auto name = tools::componentToConfig(manager->repositoryManager()->fileSystemModel()->filePath(index));
                auto components = manager->repositoryManager()->loader()->components();
                auto found = components.find(name);
                if (found == components.end())
                {
                    return;
                }
                auto component = found->second;
                auto info = component.info_;

                QMenu menu;
                menu.addAction(QIcon(":/componentMenu/add.png"), QAction::tr("Add"));
                menu.addAction(QIcon(":/componentMenu/update.png"), QAction::tr("Update"));
                menu.addAction(QAction::tr("Remove"));
                auto propsAction = menu.addAction(QAction::tr("Properties"));

                connect(propsAction, &QAction::triggered, [info]()
                {
                    PropertiesDialog dialog(info);
                    dialog.exec();
                });

                // TODO
                // auto extractAction = menu.addAction(QAction::tr("Extract code"));
                // connect(extractAction, &QAction::triggered, this, [name, index, this]
                // {
                //     if (manager_ == nullptr)
                //     {
                //         return;
                //     }
                //     manager_->extractComponentCode(name);
                // });

                menu.exec(locDirTreeView->viewport()->mapToGlobal(point));
            });

}
