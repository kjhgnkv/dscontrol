#include "CommunicationsDialog.hpp"

#include "DSMessageBox/DSMessageBox.hpp"
#include "delegates/CommComboBoxDelegate.hpp"
#include "delegates/CommLineEditDelegate.hpp"
#include "manager/models/CommunicationModel.hpp"
#include "nodeeditor/singletones/Facade.hpp"
#include "ui_CommunicationsDialog.h"
#include <QCloseEvent>

namespace dscontrol
{
CommunicationsDialog::CommunicationsDialog(CommunicationModel* model, QWidget* parent)
: QDialog(parent)
, ui_(new Ui::CommunicationsDialog)
, model_ {model}
{
    ui_->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);

    ui_->communicationView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
    ui_->communicationView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    ui_->communicationView->verticalHeader()->hide();
    ui_->communicationView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui_->communicationView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui_->communicationView->setFocusPolicy(Qt::NoFocus);
    ui_->communicationView->setEditTriggers(QAbstractItemView::DoubleClicked);
    ui_->communicationView->setModel(model_);

    // TODO FIX ip:port
    QString ipRange = "(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)";
    QString portRange = "([1-9]{1,4}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5])";

    QRegExp reg {"^" + ipRange + ":" + portRange + "$"};

    auto* lineIpDelegate = new CommLineEditDelegate {reg, ui_->communicationView};
    auto* lineNameDelegate = new CommLineEditDelegate {
    QRegExp {"([A-Za-z_\\-#№!0-9\\s]){1,30}"}, ui_->communicationView
    };

    auto comboMethodDelegate = new CommComboBoxDelegate {{tr("tcp"), tr("udp")}, ui_->communicationView};

    auto comboComponentDelegate = new CommComboBoxDelegate{{tr("ds.host.transport.provider.tcp_1.0.0"), tr("ds.host.transport.provider.udp_1.0.0")}, ui_->communicationView};

    auto comboDeviceTypeDelegate = new CommComboBoxDelegate {QtNodes::Device::types, ui_->communicationView};

    ui_->communicationView->setItemDelegateForColumn(CommunicationModel::Columns::Method, comboMethodDelegate);
    ui_->communicationView->setItemDelegateForColumn(CommunicationModel::Columns::Component, comboComponentDelegate);

    ui_->communicationView->setItemDelegateForColumn(CommunicationModel::Columns::DeviceType, comboDeviceTypeDelegate);

    ui_->communicationView->setItemDelegateForColumn(CommunicationModel::Columns::DeviceName, lineNameDelegate);
    ui_->communicationView->setItemDelegateForColumn(CommunicationModel::Columns::CommName, lineNameDelegate);

    ui_->communicationView->setItemDelegateForColumn(CommunicationModel::Columns::Sender, lineIpDelegate);
    ui_->communicationView->setItemDelegateForColumn(CommunicationModel::Columns::Recv, lineIpDelegate);

    connect(reinterpret_cast<CommunicationModel*>(model_), &CommunicationModel::updateError, this,
            &CommunicationsDialog::editError);

    connect(ui_->insertCommBtn_, &QPushButton::clicked, this, []()
    {
        QtNodes::Facade::Instance().addCommunication();
    });
    connect(ui_->removeCommBtn_, &QPushButton::clicked, this, [view = ui_->communicationView]()
    {
        if (view->model())
        {
            auto rows = view->selectionModel()->selectedRows();
            // here we remove rows from last to first, because
            // otherwise we change order of rows
            for (auto iter = rows.rbegin(); iter != rows.rend(); ++iter)
            {
                QtNodes::Facade::Instance().removeCommunication(iter->row());
            }
        }
    });
    connect(ui_->btnClose_, &QPushButton::clicked, this, &QDialog::close);

    initQSS();
}

CommunicationsDialog::~CommunicationsDialog()
{
    delete ui_;
}

void CommunicationsDialog::editError(const QString &error)
{
    DSMessageBox::warning(this, tr("Warning"), error);
}

void CommunicationsDialog::initQSS()
{
    setStyleSheet(R"(
                  QDialog{background: #33333A;}
                  QLabel{
                      color: white;}
                  QPushButton#insertCommBtn_{
                  background: #605F6E;
                  border: 1px solid #3B3E3D;
                  border-radius: 10px;
                  font-size: 18px;
                  color: #FFFFFF
                  }
                  QPushButton#removeCommBtn_{
                  background: #605F6E;
                  border: 1px solid #3B3E3D;
                  border-radius: 10px;
                  font-size: 18px;
                  color: #FFFFFF;
                  }
                  QPushButton#insertCommBtn_::hover{
                  background: #605F6E;
                  border: 1px solid #7B59FF;
                  border-radius: 10px;
                  }
                  QPushButton#removeCommBtn_::hover{
                  background: #605F6E;
                  border: 1px solid #7B59FF;
                  border-radius: 10px;
                  }
                  QPushButton#insertCommBtn_::pressed{
                  background: #7B59FF;
                  border-radius: 10px;
                  }
                  QPushButton#removeCommBtn_::pressed{
                  background: #7B59FF;
                  border-radius: 10px;
                  }
                  QPushButton#btnClose_{
                  background: #EB5757;
                  border: 1px solid #3B3E3D;
                  border-radius: 10px;
                  font-size: 18px;
                  color: #FFFFFF;
                  }
                  QTableView::item:selected
                  {
                    background: #484853;
                  QHeaderView
                  {
                      font-size: 15px;
                      font-style: normal;
                      font-weight: normal;

                  background-color: #252434;
                  }
                  }
                  )");
}

void CommunicationsDialog::closeEvent(QCloseEvent* event)
{
    emit closed();
    event->accept();
}

void CommunicationsDialog::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
    {
        this->close();
    }
}

void CommunicationsDialog::changeEvent(QEvent* event)
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
        QDialog::changeEvent(event);
    }
}
} // namespace dscontrol
