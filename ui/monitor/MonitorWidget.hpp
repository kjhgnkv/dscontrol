#pragma once

#include <QDialog>
#include <QLabel>
#include <QTreeWidgetItem>
#include <memory>
#include "../../network/DSServerComm.h"

namespace Ui
{
class MonitorWidget;
}

namespace monitor
{
class NodeItemMonitorComputer;
class NodeItemMonitorApplication;
class NodeItemComponent;
class NodeItemObject;
}

class NodeItemMonitorTree;
class AbstractItemModelMonitor;
class AbstractNodeItemMonitor;
class QTimer;
struct DestData;

class MonitorWidget : public QDialog
{
Q_OBJECT

protected:
    explicit MonitorWidget(QWidget* parent = nullptr);

public:
    static MonitorWidget* init(QWidget* parent = nullptr);

    ~MonitorWidget();
    void killComputer(const QString &ip);
    std::vector<DestData> targetDeploy();

    std::vector<std::pair<QString, std::vector<std::pair<QString, std::vector<QString>>>>> update();
    QString systemName(const QString &ip);
    void assignServerComm(DSServerComm* _server);

public slots:
    void updateData(const QString &ip, const QString &value);

private:
    QModelIndex addComputer(const monitor::NodeItemMonitorComputer* item, const QString &ipKey);
    QModelIndex addApplication(const monitor::NodeItemMonitorApplication* item, const QModelIndex &parent);
    QModelIndex addComponent(const monitor::NodeItemComponent* item, QString pid, const QModelIndex &parent);
    QModelIndex addObject(const monitor::NodeItemObject* item, QString pid, const QModelIndex &parent);

    void updateDataTable(const QModelIndex &modelIndex);
    std::map<QString, QStringList> startApps();
    std::map<QString, QStringList> stopApps();

signals:
    void selStarted(std::map<QString, QStringList>);
    void allStarted();
    void selStoped(std::map<QString, QStringList>);
    void allStoped();

private slots:
    void on_connect_stateChanged(int arg1);

private:
    void updatePushButtonClose();
    void updateComputer(const monitor::NodeItemMonitorComputer* comp);
    void updateApp(const monitor::NodeItemMonitorApplication* app);

    void prepareControlEvents();
    void onContextMenuRequested();

    void onViewEvent();
    void updateButton();

private:
    void updateTableViewComputer(const QModelIndex &index);
    void updateTableViewApplication(const QModelIndex &index);
    void startTableApp();
    void stopTableApp();

private:
    static MonitorWidget* monitorWidget_;

private:
    AbstractItemModelMonitor* modelTreeView_;
    AbstractItemModelMonitor* modelTableComputer_;

    monitor::NodeItemMonitorComputer* computer_;
    monitor::NodeItemMonitorApplication* application_;
    QModelIndex applicationIndex_;
    QAction* closeIcon_;
    DSServerComm* _serverComm;

private:
    Ui::MonitorWidget* ui_;
};
