#include "MsgCommDialog.hpp"

#include "nodeeditor/nodes/Connection.hpp"
#include "DSMessageBox/DSMessageBox.hpp"
#include "nodeeditor/lists/Communication.hpp"
#include "nodeeditor/lists/CommunicationList.hpp"
#include "nodeeditor/lists/MessageList.hpp"
#include "manager/models/CommunicationModel.hpp"
#include "manager/models/TinyMapModel.hpp"
#include "nodeeditor/singletones/Facade.hpp"
#include "ui_MsgCommDialog.h"
#include <QCloseEvent>
#include <QDebug>
#include <QPushButton>
#include "nodeeditor/lists/Message.hpp"
#include "nodeeditor/nodes/ConnectionID.hpp"

QStringList operator&(QStringList const &c1, QStringList const &c2)
{
    QStringList ret {};

    if (c1 == c2)
        return {};

    return c2;
}

namespace dscontrol
{
MsgCommDialog::MsgCommDialog(QtNodes::CommunicationList* commList, QtNodes::MessageList* msgList,
                             const QModelIndex &index, QWidget* parent)
: QDialog(parent)
, ui(new Ui::MsgCommDialog)
, commList_(commList)
, msgList_(msgList)
, msgModelIndex(index)
{
    qDebug() << "MsgCommDialog: konstructor";

    ui->setupUi(this);
    if (nullptr != commList_ && msgModelIndex.isValid() && nullptr != msgList_)
    {
        QStringList msgComms;
        for (const auto &comm : msgList_->messageComms(index.row()))
        {
            msgComms.append(comm.name_);
        }
        if (!msgComms.isEmpty())
        {
            savedCommNames.append(msgComms);
        }

        QStringList allCommNames;
        for (const auto &comm : commList_->communications())
        {
            allCommNames << comm->commName_;
        }

        if (!savedCommNames.isEmpty())
        {
            helperModel = new TinyMapModel(savedCommNames, allCommNames, this);
        }
        else
        {
            helperModel = new TinyMapModel({}, allCommNames, this);
        }

        ui->treeView->header()->hide();
        ui->treeView->setRootIsDecorated(false);
        ui->treeView->setSelectionMode(QAbstractItemView::NoSelection);
        ui->treeView->setFocusPolicy(Qt::NoFocus);
        ui->treeView->header()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
        ui->treeView->setModel(helperModel);

        /** Connecting the buttons clicks to the appropriate slots **/
        connect(ui->btnSave_, &QPushButton::clicked, this, [this]()
        {
            auto lastComm = helperModel->getLastCommNames();
            if (!(savedCommNames & lastComm).isEmpty() || lastComm.isEmpty())
            {
                savedCommNames = helperModel->getLastCommNames();
                qDebug() << "last comms: " << helperModel->getLastCommNames().size();
                qDebug() << "saved comms: " << savedCommNames.size();

                QVariantList updates {};
                for (const auto &comm : savedCommNames)
                {
                    QtNodes::MessageCommunication msgComm;
                    auto comms = commList_->communications();
                    auto found = std::find_if(comms.begin(), comms.end(), [=](const auto &val)
                    {
                        if (val->commName_ == comm)
                        {
                            return true;
                        }
                        return false;
                    });
                    if (found != comms.end())
                    {
                        updates.append(QVariant::fromValue<QtNodes::MessageCommunication>({found->get()->id_, comm}));
                    }
                }
                msgList_->updateMessage(msgModelIndex.row(), updates, QtNodes::MessageList::CommCount);
            }
            this->close();
        });

        connect(ui->treeView, &QTreeView::clicked, [this](const QModelIndex &index)
        {
            auto val = index.data(
            Qt::CheckStateRole) == Qt::CheckState::Checked ? Qt::CheckState::Unchecked : Qt::CheckState::Checked;

            if (val == Qt::CheckState::Unchecked)
                if (!deleteExistingConnections(index.data().toString()))
                    return;

            helperModel->setData(index, val, Qt::EditRole);
        });

        connect(ui->btnCancel_, &QPushButton::clicked, this, [this]()
        {
            this->close();
        });

        connect(commList_, qOverload<QString>(&QtNodes::CommunicationList::communicationToBeRemoved), this,
                &MsgCommDialog::removeComm);
    }
    initQSS();
}

bool MsgCommDialog::deleteExistingConnections(const QString &commName)
{
    auto comms = commList_->communications();
    auto found = std::find_if(comms.begin(), comms.end(), [=](const auto &val)
    {
        if (val->commName_ == commName)
        {
            return true;
        }
        return false;
    });
    if (found != comms.end())
    {
        int msgId = msgList_->messageId(msgModelIndex.row());

        std::vector<QtNodes::ConnectionID> conns;
        for (auto &conn : QtNodes::Facade::Instance().model()->connections())
        {
            if (conn.second->id().commId_ == found->get()->id_ && conn.second->id().msgId_ == msgId)
            {
                conns.push_back(conn.first);
            }
        }
        if (conns.size() > 0)
        {
            // TODO make custom message box
            QString sConns = conns.size() == 1 ? tr(" connection") : tr(" connections");
            QString text = tr("This communication is used in ") + QString::number(conns.size()) + sConns + tr(
            "! If you delete it, you will delete this") + sConns + tr(". Do you want to do this?");

            DSMessageBox dialog {{}, tr("Warning!"), text, DSMessageBox::Yes | DSMessageBox::No};

            switch (dialog.exec())
            {
                case DSMessageBox::No:
                    return false;
                case DSMessageBox::Yes:
                    break;
                default:
                    return false;
            }

            for (auto conn : conns)
            {
                QtNodes::Facade::Instance().removeConnection(
                QtNodes::Facade::Instance().model()->nodeIndex(conn.lNodeID_), conn.id_);
            }
            return true;
        }
        return true;
    }
    return false;
}

void MsgCommDialog::closeEvent(QCloseEvent* event)
{
    //    auto lastCommNames = helperModel->getLastCommNames();

    //    if (!lastCommNames.isEmpty())
    //    {
    //        QVariantList updates{lastCommNames};
    //        msgList_->updateMessage(msgModelIndex.row(),
    //                                updates,
    //                                QVector<int>()
    //                                    << QtNodes::MessageList::CommNames);
    //    }
    QDialog::closeEvent(event);
}

void MsgCommDialog::initQSS()
{
    this->setStyleSheet(R"(
                        QDialog{
                            background: #33333A;
                        }
                        QLabel{
                            color: white;}
                        QTreeView
                        {
                        background: #484853;
                        border: 1px solid #33333A;
                        }
                        QTreeView::item
                        {
                            background: #605F6E;
                            border: 1px solid #33333A;
                            padding: 13px 13px;
                            color: #FFFFFF;
                            font-size: 14px;
                        }

                        QTreeView::item
                        {
                        background: #605F6E;
                        border: 1px solid #33333A;
                        color: #FFFFFF;
                        font-size: 14px;
                        }
                        QTreeView::item:selected
                        {
                        background: #484853;
                        border: 1px solid #33333A;
                        }
                        QTreeView::item:hover
                        {
                        background: #595866;
                        border: 1px solid #33333A;
                        }

                        QTreeView::indicator:unchecked{
                        image: url(:/images/resources/images/unchecked.png)
                        }
                        QTreeView::indicator:checked{
                        image: url(:/images/resources/images/checked.png)
                        }
                        QPushButton#btnSave_{
                        background: #27AE60;
                        border-radius: 8px;
                        font-size: 14px;
                        color: #FFFFFF;
                        }
                        QPushButton#btnCancel_{
                        background: #EB5757;
                        border-radius: 8px;
                        font-size: 14px;
                        color: #FFFFFF;
                        }
                        )");
}

void MsgCommDialog::removeComm(QString name)
{
    savedCommNames.removeAll(name);
    helperModel->setLastCommNames(savedCommNames);
}

void MsgCommDialog::changeEvent(QEvent* event)
{
    if (event)
    {
        switch (event->type())
        {
            case QEvent::LanguageChange:
            {
                ui->retranslateUi(this);
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

MsgCommDialog::~MsgCommDialog()
{
    qDebug() << "MsgCommDialog: destructor";
    delete helperModel;
    delete ui;
}
} // namespace dscontrol
