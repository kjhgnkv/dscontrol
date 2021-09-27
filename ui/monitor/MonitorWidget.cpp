#include "MonitorWidget.hpp"

#include "models/AbstractItemModelMonitor.hpp"
#include "models/AbstractNodeItemMonitor.hpp"
#include "node/NodeItemMonitorApplication.hpp"
#include "node/NodeItemMonitorComputer.hpp"
#include "node/NodeItemMonitorTree.hpp"

#include "ui_MonitorWidget.h"

#include <QGridLayout>
#include <QJsonArray>
#include <QMenu>
#include <QPropertyAnimation>
#include <QScrollBar>
#include <QTableView>
#include <QTimer>
#include <QToolButton>

#include "component/NodeItemComponent.hpp"
#include "component/NodeItemObject.hpp"
#include "../deploy/DestData.hpp"

#include "node/Computer/Computer.hpp"
#include "node/Application/Application.hpp"
#include "node/Thread/Thread.hpp"

using namespace monitor;

MonitorWidget* MonitorWidget::monitorWidget_ = nullptr;

MonitorWidget::MonitorWidget(QWidget* parent)
: QDialog(parent)
, ui_(new Ui::MonitorWidget)
, computer_(nullptr)
{
    ui_->setupUi(this);

    ui_->tableViewComputer->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui_->tableViewComputer->horizontalHeader()->setStretchLastSection(true);
    ui_->tableViewComputer->verticalHeader()->hide();
    ui_->tableViewComputer->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui_->tableViewComputer->setSelectionMode(QAbstractItemView::SingleSelection);
    ui_->tableViewComputer->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui_->tableViewComputer->setFocusPolicy(Qt::NoFocus);

    ui_->tableViewApplications->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui_->tableViewApplications->horizontalHeader()->setStretchLastSection(true);
    ui_->tableViewApplications->verticalHeader()->hide();
    ui_->tableViewApplications->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui_->tableViewApplications->setSelectionMode(QAbstractItemView::SingleSelection);
    ui_->tableViewApplications->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui_->tableViewApplications->setFocusPolicy(Qt::NoFocus);

    ui_->tableViewThreads->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui_->tableViewThreads->horizontalHeader()->setStretchLastSection(true);
    ui_->tableViewThreads->verticalHeader()->hide();
    ui_->tableViewThreads->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui_->tableViewThreads->setSelectionMode(QAbstractItemView::SingleSelection);
    ui_->tableViewThreads->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui_->tableViewThreads->setFocusPolicy(Qt::NoFocus);

    ui_->monitorTreeView->setFocusPolicy(Qt::NoFocus);

    modelTreeView_ = new AbstractItemModelMonitor(new NodeItemMonitorTree());
    modelTreeView_->appendQtFlag(Qt::ItemFlag::ItemIsUserCheckable);
    ui_->monitorTreeView->setModel(modelTreeView_);

    modelTableComputer_ = new AbstractItemModelMonitor(new NodeItemMonitorComputer());
    modelTableComputer_->appendTitle({"Computer", "OS", "CPU", "Memory", "Storage"});
    ui_->tableViewComputer->setModel(modelTableComputer_);

    ui_->toolButtonClose->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    closeIcon_ = new QAction();
    ui_->toolButtonClose->setDefaultAction(closeIcon_);

    prepareControlEvents();
    onContextMenuRequested();
    onViewEvent();

    ui_->stackedWidget->setCurrentIndex(0);
    updatePushButtonClose();
    updateButton();

    connect(ui_->toolButtonStartSelected, &QToolButton::clicked, [this]()
    {
        startTableApp();
    });

    connect(ui_->toolButtonStopSelected, &QToolButton::clicked, [this]()
    {
        stopTableApp();
    });

    ui_->toolButtonStartAll->setVisible(false);
    ui_->toolButtonStopAll->setVisible(false);
    ui_->toolButtonStartSelected->setVisible(false);
    ui_->toolButtonStopSelected->setVisible(false);
}

MonitorWidget::~MonitorWidget()
{
    delete ui_;
}

MonitorWidget* MonitorWidget::init(QWidget* parent)
{
    if (monitorWidget_ == nullptr)
    {
        monitorWidget_ = new MonitorWidget {parent};
    }

    return monitorWidget_;
}

void MonitorWidget::updateData(const QString &ip, const QString &value)
{
//     qDebug() << "MonitorWidget::updateData";
//     qDebug() << "IP: " << ip;
    //qDebug() << "Value: " << value;
    QModelIndex modelIndexTable;

    auto root = dynamic_cast<NodeItemMonitorComputer*>(modelTableComputer_->root());
    auto itTable = std::find_if(root->childs().begin(),
                                root->childs().end(),
                                [&ip](const std::pair<QString, AbstractNodeItemMonitor*> &child)
                                {
                                    return child.first == ip;
                                });

    if (itTable != root->childs().end())
    {
        //qDebug() << "Update model: ip " << ip;
        modelTableComputer_->update(itTable->second->modelIndex(), QVariant(value));
        modelIndexTable = itTable->second->modelIndex();
    }
    else
    {
        //qDebug() << "new item: ip " << ip;
        AbstractNodeItemMonitor* itemAdd = new NodeItemMonitorComputer(value);
        itemAdd->appendItemDataRole(0, Qt::DecorationRole);
        modelIndexTable = modelTableComputer_->insert(std::make_pair(ip, itemAdd));
    }

    auto itTree = std::find_if(modelTreeView_->root()->childs().begin(),
                               modelTreeView_->root()->childs().end(),
                               [&](const std::pair<QString, AbstractNodeItemMonitor*> &child)
                               {
                                   return child.first == ip;
                               });

    if (itTree != modelTreeView_->root()->childs().end())
    {// find app
        //qDebug() << "Update tree view: ip " << ip;
        auto tableComp = dynamic_cast<NodeItemMonitorComputer*>(modelTableComputer_->child(modelIndexTable));
        auto itemComp = dynamic_cast<NodeItemMonitorTree*>((*itTree).second);

        for (const auto &tableApp:  tableComp->modelApp()->root()->childs())
        {// find don't app
            auto itApp = std::find_if(itemComp->childs().begin(),
                                      itemComp->childs().end(),
                                      [&](const std::pair<QString, AbstractNodeItemMonitor*> &node)
                                      {
                                          return node.first == tableApp.first;
                                      });

            NodeItemMonitorApplication* findApp = dynamic_cast<NodeItemMonitorApplication*>(tableApp.second);
            if (itApp != itemComp->childs().end() && findApp->application()->status() == 0)
            {// remove app
                auto removeItApp = dynamic_cast<NodeItemMonitorTree*>(itApp->second);
                modelTreeView_->remove(removeItApp->modelIndex());
            }
            else if (itApp == itemComp->childs().end() && findApp->application()->status())
            {// add app
                QModelIndex modelItemApp = addApplication(findApp, itemComp->modelIndex());
                ui_->monitorTreeView->expand(modelItemApp);
            }
        }
    }
    else
    {
        //qDebug() << "new item tree: ip " << ip;
        auto tableComp = dynamic_cast<NodeItemMonitorComputer*>(modelTableComputer_->child(modelIndexTable));
        QModelIndex modelItemComp = addComputer(tableComp, ip);
        ui_->monitorTreeView->expand(modelItemComp);
        for (const auto &itTableApp : tableComp->modelApp()->root()->childs())
        {
            NodeItemMonitorApplication* tableApp = dynamic_cast<NodeItemMonitorApplication*>(itTableApp.second);
            QModelIndex modelItemApp = addApplication(tableApp, modelItemComp);
            ui_->monitorTreeView->expand(modelItemApp);
        }
    }

    updateDataTable(modelIndexTable);
    ui_->tableViewComputer->resizeRowsToContents();
}

QModelIndex MonitorWidget::addComputer(const NodeItemMonitorComputer* item, const QString &ip)
{
    Computer* computer = item->computer();

    AbstractNodeItemMonitor* itemComp = new NodeItemMonitorTree(ip,
                                                                computer->network()->ipAddress(),
                                                                computer->network()->hostName(),
                                                                LevelTreeMonitor::TYPE::Computer);
    QModelIndex modelIndex = modelTreeView_->insert(std::make_pair(ip, itemComp));

    return modelIndex;
}

QModelIndex MonitorWidget::addApplication(const NodeItemMonitorApplication* item, const QModelIndex &parent)
{
    Application* application = item->application();

    AbstractNodeItemMonitor* itemApp = new NodeItemMonitorTree(application->pid(),
                                                               application->pid(),
                                                               application->name(),
                                                               LevelTreeMonitor::TYPE::Application);

    QModelIndex modelIndex = modelTreeView_->insert(std::make_pair(application->pid(), itemApp), parent);

    return modelIndex;
}

QModelIndex MonitorWidget::addComponent(const NodeItemComponent* item, QString pid, const QModelIndex &parent)
{
    AbstractNodeItemMonitor* itemComponent = new NodeItemMonitorTree(pid,
                                                                     item->name_,
                                                                     item->json_,
                                                                     LevelTreeMonitor::TYPE::Component);
    QModelIndex modelIndex = modelTreeView_->insert(std::make_pair(pid, itemComponent), parent);

    return modelIndex;
}

QModelIndex MonitorWidget::addObject(const NodeItemObject* item, QString pid, const QModelIndex &parent)
{
    AbstractNodeItemMonitor* itemObj = new NodeItemMonitorTree(pid,
                                                               item->name_,
                                                               item->json_,
                                                               LevelTreeMonitor::TYPE::Object);
    QModelIndex modelIndex = modelTreeView_->insert(std::make_pair(pid, itemObj), parent);

    return modelIndex;
}

void MonitorWidget::updateDataTable(const QModelIndex &modelIndex)
{
    switch (ui_->stackedWidget->currentIndex())
    {
        case 1:
        {
            updateTableViewComputer(modelIndex);
            break;
        }
        case 2:
        {
            updateTableViewApplication(modelIndex);
            break;
        }
    }
}

void MonitorWidget::startTableApp()
{
    QModelIndex selectAppIndex = ui_->tableViewApplications->currentIndex();

    QModelIndex selectCompIndex = ui_->tableViewComputer->currentIndex();

    if (selectAppIndex.isValid() && selectCompIndex.isValid())
    {
        auto comp = modelTableComputer_->child(selectCompIndex);
        NodeItemMonitorComputer* itemComp = dynamic_cast<NodeItemMonitorComputer*>(comp);
        auto app = modelTableComputer_->child(selectAppIndex);
        NodeItemMonitorApplication* itemApp = dynamic_cast<NodeItemMonitorApplication*>(app);

        Computer* computer = itemComp->computer();
        Application* application = itemApp->application();

        if (!application->status())
        {
            std::map<QString, QStringList> returnApps;

            QString nameApp = application->name();
            #ifdef  _WIN32
            int indexOf = nameApp.indexOf('.');
            nameApp = nameApp.mid(0, indexOf);
            #endif

            returnApps.insert(std::make_pair(computer->network()->ipAddress(), nameApp));

            // emit selStarted(returnApps);
        }
    }
}

void MonitorWidget::stopTableApp()
{
    QModelIndex selectAppIndex = ui_->tableViewApplications->currentIndex();

    QModelIndex selectCompIndex = ui_->tableViewComputer->currentIndex();

    if (selectAppIndex.isValid() && selectCompIndex.isValid())
    {
        auto comp = modelTableComputer_->child(selectCompIndex);
        NodeItemMonitorComputer* itemComp = dynamic_cast<NodeItemMonitorComputer*>(comp);
        auto app = modelTableComputer_->child(selectAppIndex);
        NodeItemMonitorApplication* itemApp = dynamic_cast<NodeItemMonitorApplication*>(app);

        Computer* computer = itemComp->computer();
        Application* application = itemApp->application();

        if (application->status())
        {
            std::map<QString, QStringList> returnApps;

            QString nameApp = application->name();
            #ifdef  _WIN32
            int indexOf = nameApp.indexOf('.');
            nameApp = nameApp.mid(0, indexOf);
            #endif

            returnApps.insert(std::make_pair(computer->network()->ipAddress(), nameApp));

            // emit selStoped(returnApps);
        }
    }
}

void MonitorWidget::prepareControlEvents()
{
    connect(ui_->toolButtonStartAll, &QToolButton::clicked, [this]()
    {
        // переделать
        // должен принимать все апп, которые были запущены
        // allStarted();
    });
    connect(ui_->toolButtonStartSelected, &QToolButton::clicked, [this]()
    {
        emit selStarted(startApps());
    });
    connect(ui_->toolButtonStopAll, &QToolButton::clicked, [this]()
    {
        // переделать
        // должен принимать все апп, которые были запущены
        // allStoped();
    });
    connect(ui_->toolButtonStopSelected, &QToolButton::clicked, [this]()
    {
        emit selStoped(stopApps());
    });
    connect(ui_->toolButtonClose, &QToolButton::clicked, [this]()
    {
        switch (ui_->stackedWidget->currentIndex())
        {
            case 2:
            {
                ui_->stackedWidget->setCurrentIndex(1);
                break;
            }
            case 1:
            {
                ui_->stackedWidget->setCurrentIndex(0);
                break;
            }
            case 0:
            {
                close();
                break;
            }
        }

        updatePushButtonClose();
    });
}

std::map<QString, QStringList> MonitorWidget::startApps()
{
    std::map<QString, QStringList> returnApps;

    return returnApps;
}

std::map<QString, QStringList> MonitorWidget::stopApps()
{
    std::map<QString, QStringList> returnApps;
    for (const auto &itComp : modelTreeView_->root()->childs())
    {
        NodeItemMonitorTree* comp = dynamic_cast<NodeItemMonitorTree*>(itComp.second);
        for (const auto &app : comp->childs())
        {
            NodeItemMonitorTree* item = dynamic_cast<NodeItemMonitorTree*>(app.second);
            if (item->getCheckState() == Qt::CheckState::Checked)
            {
                QString nameApp = item->getNameApp();
                #ifdef  _WIN32
                int indexOf = nameApp.indexOf('.');
                nameApp = nameApp.mid(0, indexOf);
                #endif

                auto it = returnApps.find(itComp.first);
                if (it != returnApps.end())
                {
                    it->second.push_back(nameApp);
                }
                else
                {
                    QStringList list;
                    list.push_back(nameApp);
                    returnApps.insert(std::make_pair(itComp.first, list));
                }
            }
        }
    }

    return returnApps;
}

void MonitorWidget::onContextMenuRequested()
{
    ui_->monitorTreeView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui_->monitorTreeView, &QTreeView::customContextMenuRequested, [this](const QPoint &point)
    {
        QModelIndex index = ui_->monitorTreeView->indexAt(point);
        if (index.isValid())
        {
            auto nodeItem = dynamic_cast<NodeItemMonitorTree*>(modelTreeView_->child(index));
            if (nodeItem != nullptr)
            {
                QMenu menuApplication;
                auto actionStart = menuApplication.addAction(QAction::tr("Start"));
                auto actionStop = menuApplication.addAction(QAction::tr("Stop"));
                auto actionUndo = menuApplication.addAction(QAction::tr("Undo"));
                auto actionUpdate = menuApplication.addAction(QAction::tr("Update"));
                auto actionAutoUpdate = menuApplication.addAction(QAction::tr("AutoUpdate"));

                QMenu menuCompObj;
                auto actionProperties = menuCompObj.addAction(QAction::tr("Properties"));

                QModelIndexList modelIndexList = ui_->monitorTreeView->selectionModel()->selectedIndexes();

                connect(actionStart, &QAction::triggered, [=]()
                {
                    // emit selStarted(getStopApps(modelIndexList));
                });

                connect(actionStop, &QAction::triggered, [=]()
                {
                    // emit selStoped(getStopApps(modelIndexList));
                });

                connect(actionUndo, &QAction::triggered, [=]()
                {
                });

                connect(actionUpdate, &QAction::triggered, [=]()
                {
                });

                connect(actionAutoUpdate, &QAction::triggered, [=]()
                {
                });

                connect(actionProperties, &QAction::triggered, [=]()
                {
                    if (nodeItem->getLevel() == LevelTreeMonitor::TYPE::Component)
                    {
                        // open WidgetComponent
                    }
                    else if (nodeItem->getLevel() == LevelTreeMonitor::TYPE::Object)
                    {
                        // open WidgetObject
                    }
                });

                switch (nodeItem->getLevel())
                {
                    case LevelTreeMonitor::TYPE::Application:
                    {
                        menuApplication.exec(ui_->monitorTreeView->viewport()->mapToGlobal(point));
                        break;
                    }
                    case LevelTreeMonitor::TYPE::Component:
                    case LevelTreeMonitor::TYPE::Object:
                    {
                        menuCompObj.exec(ui_->monitorTreeView->viewport()->mapToGlobal(point));
                        break;
                    }
                }
            }
        }
    });
}

void MonitorWidget::onViewEvent()
{
    connect(ui_->monitorTreeView, &QTreeView::clicked, [=](const QModelIndex &index)
    {
        if (index.isValid())
        {
            auto nodeItem = dynamic_cast<NodeItemMonitorTree*>(modelTreeView_->child(index));
            ui_->monitorTreeView->setCurrentIndex(index);
            updateButton();
        }
    });

    connect(ui_->tableViewComputer, &QTableView::doubleClicked, [=](const QModelIndex &index)
    {
        if (index.isValid())
        {
            auto computer_ = dynamic_cast<NodeItemMonitorComputer*>(modelTableComputer_->child(index));
            if (computer_ != nullptr)
            {
                updateComputer(computer_);
                ui_->stackedWidget->setCurrentIndex(1);
                updatePushButtonClose();
                ui_->tableViewApplications->setModel(computer_->modelApp());
            }
        }
    });

    connect(ui_->tableViewApplications, &QTableView::doubleClicked, [=](const QModelIndex &index)
    {
        if (index.isValid())
        {
            auto application_ = dynamic_cast<NodeItemMonitorApplication*>(computer_->modelApp()->child(index));
            if (application_ != nullptr)
            {
                applicationIndex_ = index;
                updateApp(application_);
                ui_->stackedWidget->setCurrentIndex(2);
                updatePushButtonClose();
            }
        }
    });
}

void MonitorWidget::updateTableViewComputer(const QModelIndex &index)
{
    auto child = modelTableComputer_->child(index);
    if (computer_ != nullptr && child->modelIndex().internalPointer() == computer_->modelIndex().internalPointer())
    {
        updateComputer(computer_);
        updatePushButtonClose();
        ui_->tableViewApplications->resizeRowsToContents();
    }
}

void MonitorWidget::updateTableViewApplication(const QModelIndex &index)
{
    auto app = computer_->modelApp()->child(applicationIndex_);
    if (application_ != nullptr && app->modelIndex().internalPointer() == application_->modelIndex().internalPointer())
    {
        updateApp(application_);
        updatePushButtonClose();
    }
}

void MonitorWidget::updatePushButtonClose()
{
    switch (ui_->stackedWidget->currentIndex())
    {
        case 0:
        {
            QString style = "QToolButton {\n"
                            "background: #EB5757;\n"
                            "border: 1px solid #3B3E3D;\n"
                            "border-radius: 10px;\n"
                            "color: rgb(255, 255, 255);\n"
                            "}\n"
                            "\n"
                            ":hover{\n"
                            "background: #F87E7E;\n"
                            "border: 1px solid #3B3E3D;\n"
                            "border-radius: 10px;\n"
                            "color: rgb(255, 255, 255);\n"
                            "}";

            closeIcon_->setText("Close");
            // closeIcon_->setIcon(QIcon(""));

            ui_->toolButtonClose->setStyleSheet(style);
            break;
        }
        case 1:
        case 2:
        {
            QString style = "QToolButton {\n"
                            "background: #3D3D46;\n"
                            "border: 1px solid #AAAAC5;\n"
                            "border-radius: 10px;\n"
                            "color: #AAAAC5;\n"
                            "}\n"
                            "\n"
                            ":hover{\n"
                            "background: #3D3D46;\n"
                            "border: 1px solid #FFFFFF;\n"
                            "border-radius: 10px;\n"
                            "color: #FFFFFF;\n"
                            "}";

            closeIcon_->setText("Back");
            // closeIcon_->setIcon(QIcon(":/back/resources/back/back_hide.png"));

            ui_->toolButtonClose->setStyleSheet(style);
            break;
        }
    }
}

void MonitorWidget::updateComputer(const NodeItemMonitorComputer* comp)
{
    Computer* computer = comp->computer();

    Network* network = computer->network();
    Cpu* cpu = computer->cpu();
    Memory* memory = computer->memory();
    Storage* storage = computer->storage();
    System* system = computer->system();

    ui_->labelHostNameText->setText(network->hostName());
    ui_->labelIPv4AddresText->setText(network->ipAddress());

    ui_->labelCPUNameText->setText(cpu->name());
    ui_->labelCPUMinMaxText->setText(cpu->minMaxbHz());
    ui_->labelCPUCacheText->setText(cpu->caches());
    ui_->labelCPUCoresText->setText(cpu->cores());
    ui_->labelCPUThreadsText->setText(cpu->threads());
    ui_->labelCPUUsageText->setText(cpu->usage());

    ui_->labelMemoryLoadText->setText(memory->load());
    ui_->labelUsedPhysMBText->setText(memory->total());
    ui_->labelFreePhysMBText->setText(memory->free());
    ui_->labelTotalPhysMBText->setText(memory->used());

    ui_->labelLoadText->setText(storage->load());
    ui_->labelAvailableSpaceMBText->setText(storage->free());
    ui_->labelTotalSpaceMBText->setText(storage->total());
    ui_->labelUsedSpaceMBText->setText(storage->used());
    ui_->labelDeviceText->setText(storage->device());
    ui_->labelFileSystemTypeText->setText(storage->type());
    ui_->labelRootPathText->setText(storage->path());

    ui_->labelOSNameText->setText(system->productName());
    ui_->labelKernelTypeText->setText(system->kernelType());
    ui_->labelArchitectureText->setText(system->architecture());

    ui_->tableViewApplications->setModel(comp->modelApp());
}

void MonitorWidget::updateApp(const NodeItemMonitorApplication* app)
{
    Application* application = app->application();

    ui_->labelNameText->setText(application->name());
    ui_->labelPIDText->setText(application->pid());
    ui_->labelUserNameText->setText(application->userName());
    ui_->labelStatusText->setText((application->status() ? "true" : "false"));
    ui_->labelThreadsCountText->setText(application->countOfThreads());
    ui_->labelWorkingSetSizeText->setText(application->workingSetSize());
    ui_->labelCPUUsageAppText->setText(application->cpuUsage());
    ui_->labelMemoryLoadAppText->setText(application->memoryLoad());
    ui_->labelPathText->setText(application->processPath());

    ui_->tableViewThreads->setModel(app->modelApp());
}

void MonitorWidget::updateButton()
{
    if (!stopApps().empty())
    {
        ui_->toolButtonStartSelected->setEnabled(true);
        ui_->toolButtonStopSelected->setEnabled(true);
    }
    else
    {
        ui_->toolButtonStartSelected->setEnabled(false);
        ui_->toolButtonStopSelected->setEnabled(false);
    }
}

void MonitorWidget::killComputer(const QString &ip)
{
    auto root = dynamic_cast<NodeItemMonitorComputer*>(modelTableComputer_->root());
    auto it = std::find_if(root->childs().begin(),
                           root->childs().end(),
                           [&ip](const std::pair<QString, AbstractNodeItemMonitor*> &comp)
                           {
                               return comp.first == ip;
                           });
    if (it != root->childs().end())
    {
        auto model = dynamic_cast<NodeItemMonitorComputer*>(it->second);
        auto computer = model->computer();
        computer->setIconColor(Icon::IconColor::Red);

        modelTableComputer_->updateData(model->modelIndex());

        auto vecChilds = dynamic_cast<NodeItemMonitorComputer*>(it->second)->modelApp()->root()->childs();
        for (const auto &app : vecChilds)
        {
            auto application = dynamic_cast<NodeItemMonitorApplication*>(app.second)->application();
            application->setIconColor(Icon::IconColor::Red);

            auto comp = dynamic_cast<NodeItemMonitorComputer*>(it->second);
            auto appModel = dynamic_cast<NodeItemMonitorApplication*>(app.second)->modelIndex();
            comp->modelApp()->updateData(appModel);
        }
    }
}

std::vector<DestData> MonitorWidget::targetDeploy()
{
    std::vector<DestData> deploy;

    for (const auto &comp : modelTableComputer_->root()->childs())
    {
        QString ip = comp.first;
        NodeItemMonitorComputer* item = dynamic_cast<NodeItemMonitorComputer*>(comp.second);
        Computer* computer = item->computer();

        DestData destData {
        computer->network()->hostName(), ip, computer->system()->productName(), computer->system()->architecture()
        };

        deploy.push_back(destData);
    }
    qDebug() << "MonitorWidget::targetDeploy";
    for (const auto &it : deploy)
    {
        qDebug() << "ip: " << it.ip_;
    }

    return deploy;
}

std::vector<std::pair<QString, std::vector<std::pair<QString, std::vector<QString>>>>> MonitorWidget::update()
{
    std::vector<std::pair<QString, std::vector<std::pair<QString, std::vector<QString>>>>> vecComp;

    for (const auto &comp : modelTreeView_->root()->childs())
    {
        std::vector<std::pair<QString, std::vector<QString>>> vecApp;
        for (const auto &app : comp.second->childs())
        {
            std::vector<QString> vecObjs;
            for (const auto &obj : app.second->childs())
            {
                vecObjs.push_back(obj.second->getValue().value(0));
            }
            vecApp.emplace_back(app.second->getValue().value(0), vecObjs);
        }

        vecComp.emplace_back(comp.second->getValue().value(0), vecApp);
    }

    return vecComp;
}

QString MonitorWidget::systemName(const QString &ip)
{
    auto root = dynamic_cast<NodeItemMonitorComputer*>(modelTableComputer_->root());
    auto it = std::find_if(root->childs().begin(),
                           root->childs().end(),
                           [&ip](const std::pair<QString, AbstractNodeItemMonitor*> &comp)
                           {
                               return comp.first == ip;
                           });

    if (it != root->childs().end())
    {
        Computer* computer = dynamic_cast<NodeItemMonitorComputer*>(it->second)->computer();
        return computer->system()->id();
    }

    return QString("");
}
void MonitorWidget::assignServerComm(DSServerComm* serverComm)
{
    _serverComm=serverComm;
}
void MonitorWidget::on_connect_stateChanged(int arg1)
{
    if(arg1)
            _serverComm->SetMonitor(true);
        else
        {
            _serverComm->SetMonitor(false);
        }
}

