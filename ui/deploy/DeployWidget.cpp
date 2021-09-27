#include "DeployWidget.hpp"
#include "ui_DeployWidget.h"
#include <QDebug>
#include <QTableView>
#include <QMessageBox>

#include "DeployTargetModel.hpp"
#include "DeployDestModel.hpp"
#include "delegates/CommComboBoxDelegate.hpp"

DeployWidget* DeployWidget::deployWidget_ = nullptr;

DeployWidget::DeployWidget(QWidget* parent)
: QDialog(parent)
, ui_(new Ui::DeployWidget)
{
    ui_->setupUi(this);

    targetModel_ = new dscontrol::models::DeployTargetModel {this};
    destModel_ = new dscontrol::models::DeployDestModel {this};

    delegate_ = new dscontrol::CommComboBoxDelegate {{}, ui_->treeViewProjectDesing};

    ui_->tableViewTargetDeployMachine->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui_->tableViewTargetDeployMachine->horizontalHeader()->setStretchLastSection(false);
    ui_->tableViewTargetDeployMachine->verticalHeader()->hide();
    ui_->tableViewTargetDeployMachine->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui_->tableViewTargetDeployMachine->setSelectionMode(QAbstractItemView::SingleSelection);
    ui_->tableViewTargetDeployMachine->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui_->treeViewProjectDesing->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui_->treeViewProjectDesing->header()->setStretchLastSection(false);
    ui_->treeViewProjectDesing->setSelectionMode(QAbstractItemView::SingleSelection);
    ui_->treeViewProjectDesing->header()->setSectionResizeMode(QHeaderView::Stretch);

    ui_->treeViewProjectDesing->setItemDelegateForColumn(dscontrol::models::DeployTargetModel::Column::Ip, delegate_);

    ui_->treeViewProjectDesing->setModel(targetModel_);
    ui_->tableViewTargetDeployMachine->setModel(destModel_);
}

DeployWidget* DeployWidget::init()
{
    if (deployWidget_ == nullptr)
    {
        deployWidget_ = new DeployWidget {};
    }

    return deployWidget_;
}

DeployWidget::~DeployWidget()
{
    qDebug() << "DeployWidget::~DeployWidget";

    delete targetModel_;
    delete destModel_;
    delete delegate_;

    delete ui_;
}

void DeployWidget::setupData(const std::vector<std::tuple<int, QString, QString>> &targetData,
                             const std::vector<DestData> &listDSServer)
{
    QStringList list {};
    for (const auto &host : listDSServer)
    {
        list.append(host.ip_);
    }

    delegate_->setValidValue(list);
    targetModel_->setupModelData(targetData);
    destModel_->setupModelData(listDSServer);

    ui_->treeViewProjectDesing->expandAll();
}

void DeployWidget::on_deployBtn_clicked()
{
    // TODO Deploy
    // по нажатию deployBtn в виджете Deployment Precess
    // вызывает окно с PregressBar Deploy
    std::map<QString, QString> xmls = targetModel_->selectedApplications();
    if (!xmls.empty())
    {
        emit deploySelectedBtn(ui_->autoRunCheckBox->isChecked(), xmls);
    }
    else
    {
        QMessageBox::information(this, "Ошибка деплоя", "Вы не выбрали компоненты!!!");
    }
}
