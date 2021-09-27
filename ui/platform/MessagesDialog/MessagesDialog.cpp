#include "MessagesDialog.hpp"

#include "DSMessageBox/DSMessageBox.hpp"
#include "MsgCommDialog/MsgCommDialog.hpp"
#include "nodeeditor/lists/CommunicationList.hpp"
#include "manager/models/MessageModel.hpp"
#include "nodeeditor/singletones/Facade.hpp"
#include "ui_MessagesDialog.h"
#include <QCloseEvent>

namespace dscontrol
{
MessagesDialog::MessagesDialog(QAbstractItemModel* model, QWidget* parent)
: QDialog(parent)
, ui_(new Ui::MessagesDialog)
, model_ {model}
{
    qDebug() << "MessagesDialog::MessagesDialog";

    ui_->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);

    ui_->messageView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
    ui_->messageView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);

    ui_->messageView->verticalHeader()->hide();
    ui_->messageView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui_->messageView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui_->messageView->setFocusPolicy(Qt::NoFocus);
    ui_->messageView->setModel(model_);

    connect(ui_->messageView, &QTableView::doubleClicked, this, [ui = ui_, this](const QModelIndex &index)
    {
        if (index.column() == MessageModel::Columns::CommNames)
        {
            if (QtNodes::Facade::Instance().communicationList()->size() == 0)
            {
                dscontrol::DSMessageBox::critical(ui->messageView, tr("Error"),
                                                  tr("You cannot have any communications!"));
                return;
            }
            MsgCommDialog dialog(QtNodes::Facade::Instance().communicationList(),
                                 QtNodes::Facade::Instance().messageList(), index);
            dialog.exec();
        }
    });

    connect(ui_->btnClose_, &QPushButton::clicked, this, &QDialog::close);

    initQSS();
}

MessagesDialog::~MessagesDialog()
{
    delete ui_;
}

void MessagesDialog::initQSS()
{
    setStyleSheet(R"(
                     QDialog{background: #33333A;}
                  QLabel{
                      color: white;}
                      QPushButton#btnClose_{
                      background: #EB5757;
                      border: 1px solid #3B3E3D;
                      border-radius: 10px;
                      font-size: 18px;
                  color: #FFFFFF
                      }
                  )");
}

void MessagesDialog::closeEvent(QCloseEvent* event)
{
    qDebug() << "close";
    emit closed();
    event->accept();
}

void MessagesDialog::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
    {
        this->close();
    }
}

void MessagesDialog::changeEvent(QEvent* event)
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
